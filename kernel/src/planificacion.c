#include "planificacion.h"

int proceso_ejecutando;
uint32_t tiempoInicioBlock;

//................................. LARGO PLAZO.........................................................................................

// CREAR PCB

pcb *crear_estructura_pcb(t_consola *consola) {
	pcb *pcb = malloc(sizeof(pcb));
	pthread_mutex_lock(&mutex_generador_id);
	pcb->id_proceso = generador_de_id;
	generador_de_id++;
	pthread_mutex_unlock(&mutex_generador_id);
	pcb->tamanio_proceso =consola->tamanio_proceso;
	pcb->instrucciones = list_duplicate(consola->instrucciones);
	pcb->program_counter = 0;
	pcb->estimacion_rafaga = config_valores_kernel.estimacion_inicial;
	pcb->tiempo_de_bloqueo = 0;

	return pcb;
}


// MANEJO DE ESTADOS PCBs


// Hilos para las transiciones


void iniciar_planificador_largo_plazo(void) {

	generador_de_id = 0;
	pthread_mutex_init(&mutex_new, NULL);
	pthread_mutex_init(&mutex_exit, NULL);
	pthread_mutex_init(&mutex_generador_id, NULL);
	sem_init(&sem_admitir, 0, 0);
	sem_init(&sem_exit, 0, 0);
	sem_init(&sem_grado_multiprogramacion, 0, config_valores_kernel.grado_multiprogramacion);
	colaNew = list_create();
	colaExit = list_create();
	pthread_create(&thread_exit, NULL, (void *)finalizarPcb, NULL); // (?) NO ESTA DEFINIDA UNA PCB ACA, VA A HABER QUE HACER QUE FINALIZARPCB AGARRE LA PCB DESDE LA LISTA
	pthread_create(&thread_admitir, NULL, (void *)transicion_admitir_por_prioridad, NULL);
	pthread_detach(thread_exit);
	pthread_detach(thread_admitir);
}



 void agregarANewPcb(pcb* pcb) {

 	pthread_mutex_lock(&mutex_new);

 	   list_add(colaNew, pcb);
 	   pcb->estado_proceso = NUEVO;
 	   log_info(logger, "[NEW] Entra el pcb de ID: %d a la cola.", pcb->id_proceso);

 	pthread_mutex_unlock(&mutex_new);

 	sem_post(&sem_admitir);
 }



void transicion_admitir_por_prioridad(void) {

	while(1) {
		sem_wait(&sem_admitir); // IDEM ANTERIOR
		sem_wait(&sem_grado_multiprogramacion);
		pcb *pcb;

		pthread_mutex_lock(&mutex_suspended_ready);
		if(!list_is_empty(colaSuspendedReady)) {
			pcb = list_remove(colaSuspendedReady,list_size(colaSuspendedReady));
			pthread_mutex_unlock(&mutex_suspended_ready);
			log_info(kernel_logger, "PID[%d] ingresa a READY desde SUSPENDED-READY", pcb->id_proceso);
		} else {
			pthread_mutex_unlock(&mutex_suspended_ready);
			pthread_mutex_lock(&mutex_new);
			pcb = list_remove(colaNew,list_size(colaNew));
			pthread_mutex_unlock(&mutex_new);
			pcb->valor_tabla_paginas = obtener_entrada_tabla_de_pagina(socket_memoria);
			log_info(kernel_logger, "PID[%d] ingresa a READY desde NEW", pcb->id_proceso);
		}

		pthread_mutex_lock(&mutex_ready);  //Se agrega a Ready el proceso
		list_add(colaReady, pcb);
		pthread_mutex_unlock(&mutex_ready);
		avisarAModulo(socket_memoria,INICIALIZAR_ESTRUCTURAS) ;
		sem_post(&sem_ready);
	}
}

void finalizarPcb(){
	sem_wait(&sem_exit);
  	pthread_mutex_lock(&mutex_exit);
  	pcb* pcb = list_remove(colaExit, 0);
  	pthread_mutex_unlock(&mutex_exit);
 	log_info(logger, "[EXIT] Finaliza el  pcb de ID: %d", pcb-> id_proceso);
	enviar_pcb_a_memoria(pcb, socket_memoria,LIBERAR_ESTRUCTURAS);

	op_code codigo = esperar_respuesta_memoria(socket_memoria);
	if(codigo != ESTRUCTURAS_LIBERADAS) {
		log_error(kernel_logger, "No se pudo eliminar memoria de PID[%d]", pcb->id_proceso);
	}
	avisarAModulo(socket_consola,FINALIZAR_CONSOLA) ;
 	eliminar_pcb(pcb);
}




//................................. CORTO PLAZO.........................................................................................

void iniciar_planificador_corto_plazo(void) {

	pthread_mutex_init(&mutex_ready, NULL);
	pthread_mutex_init(&mutex_blocked, NULL);
	pthread_mutex_init(&mutex_exec, NULL);
	sem_init(&sem_ready, 0, 0);
	sem_init(&sem_exec, 0, 0);
	sem_init(&sem_blocked, 0, 0);
	sem_init(&sem_desalojo, 0, 0);
	colaReady = list_create();
	colaExec = list_create();
	colaBlocked = list_create();
	pthread_create(&thread_ready, NULL, (void *)estadoReady, NULL);
	pthread_create(&thread_exec, NULL, (void *)estadoExec, NULL);
	pthread_create(&thread_blocked, NULL, (void *)estadoBlockeado, NULL);
	pthread_detach(thread_ready);
	pthread_detach(thread_exec);
	pthread_detach(thread_blocked);

}


 void estadoReady(){
 	while(1){
 		sem_wait(&sem_ready);
 		algoritmo algoritmo = obtener_algoritmo();
 		if(algoritmo == SRT){
 			if(proceso_ejecutando == 1){
 				interrumpir_cpu();
 				sem_wait(&sem_desalojo);
 			}
 		}
 		// semaforo

 		pcb* siguiente_proceso = obtenerSiguienteReady();

 		pthread_mutex_lock(&mutex_exec);
 		list_add(colaExec, siguiente_proceso);
 		pthread_mutex_unlock(&mutex_exec);

 		sem_post(&sem_exec);
 	}
 }


void estadoExec(){
	while(1){
		sem_wait(&sem_exec);

		pthread_mutex_lock(&mutex_exec);
		pcb* proceso = list_remove(colaExec,0);
		proceso_ejecutando = 1;
		pthread_mutex_unlock(&mutex_exec);

		uint32_t inicio_cpu = get_time(); // logueo el tiempo en el que se va

		enviarPcb(socket_dispatch, proceso);
		eliminar_pcb(proceso);

		//esperar a que vuelva el pcb
		//recibir pcb de cpu por dispatch;


		proceso = recibirPcb(socket_dispatch);


		uint32_t finalizacion_cpu = get_time();
		pthread_mutex_lock(&mutex_exec);
		proceso_ejecutando = 0;
		pthread_mutex_unlock(&mutex_exec);

		proceso->rafaga_anterior = inicio_cpu - finalizacion_cpu;

		instruccion *instruccion_ejecutada = list_get(proceso->instrucciones, (proceso->program_counter - 1)); // agarro la instruccion ya ejecutada por cpu

		switch(instruccion_ejecutada->codigo){
		case IO:
			pthread_mutex_lock(&mutex_blocked);
			proceso->estado_proceso = BLOQUEADO;
			tiempoInicioBLock = get_time(); // VIG => RESOLVER ESTO
			list_add(colaBlocked, proceso);
			pthread_mutex_unlock(&mutex_blocked);
			sem_post(&sem_blocked); // despertar bloqueado
			break;
		case EXIT:
			pthread_mutex_lock(&mutex_exit);
			proceso->estado_proceso = FINALIZADO;
			list_add(colaExit, proceso);
			pthread_mutex_unlock(&mutex_exit);
			sem_post(&sem_exit); // despertar exit
			break;
		default: // ready write o noop
			pthread_mutex_lock(&mutex_ready);
			proceso->estado_proceso = LISTO;
			list_add(colaReady, proceso);
			pthread_mutex_unlock(&mutex_ready);
			sem_post(&sem_desalojo);
			sem_post(&sem_ready);
			break;
		}

}

void estadoBlockeado(){

	while(1){
		sem_wait(&sem_blocked);
		uint32_t tiempoMaxDeBloqueo = config_valores_kernel.tiempo_maximo_bloqueado;
		pthread_mutex_lock(&mutex_blocked);
		pcb* proceso = list_remove(colaBlocked,0);
		pthread_mutex_unlock(&mutex_blocked);
		uint32_t tiempoQueLLevaEnBlock = get_time() - tiempoInicioBlock; // tiempoInicioBlock es variable global de planificacion.c

		if (tiempoQueLLevaEnBlock > tiempoMaxDeBloqueo){ // suspendo de entrada

          transicion_suspender(proceso); //suspender para mediano plazo
			ejecutarIO(proceso->tiempo_de_bloqueo);

		} else if (tiempoQueLLevaEnBlock + proceso->tiempo_de_bloqueo > tiempoMaxDeBloqueo){ // suspendo en el medio

			uint32_t tiempoIOAntesDeSuspender = tiempoMaxDeBloqueo - tiempoQueLLevaEnBlock;
			ejecutarIO(tiempoIOAntesDeSuspender); // ejecutar hasta que sea necesario suspender
			transicion_suspender(proceso); //suspender para mediano plazo
			ejecutarIO(proceso->tiempo_de_bloqueo - tiempoIOAntesDeSuspender); // ejecuto el io restante

		} else { // la ejecucion de io + el tiempo que lleva en block es menor al tiempo max de blockeo
			ejecutarIO(proceso->tiempo_de_bloqueo);
			pthread_mutex_lock(&mutex_ready);
			list_add(colaReady,proceso);
			pthread_mutex_unlock(&mutex_ready);
			sem_post(&sem_ready);
		}
	}
}



//................................. MEDIANO PLAZO.........................................................................................


// TRANSICIONES_ ESTADOS


 void iniciar_planificador_mediano_plazo(void) {

 	pthread_mutex_init(&mutex_suspended_ready, NULL);
 	pthread_mutex_init(&mutex_suspended_blocked, NULL);
 	sem_init(&sem_suspended_ready, 0, 0);
 	colaSuspendedBlocked =list_create();
 	colaSuspendedReady = list_create();
 	pthread_create(&thread_suspended_ready, NULL, (void *)estado_suspended_ready, NULL);
 	pthread_detach(thread_suspended_ready);
 }

 void transicion_suspender(pcb *pcb) {

 	log_info(kernel_logger, "PID[%d] ingresa a SUSPENDED-BLOCKED", pcb->id_proceso);
 	pcb->estado_proceso = LISTO_SUSPENDIDO;
	enviar_pcb_a_memoria(pcb, socket_memoria, LIBERAR_ESPACIO_PCB);
 	op_code codigo = esperar_respuesta_memoria(socket_memoria);
 	if(codigo != ESPACIO_PCB_LIBERADO) {
 		log_error(kernel_logger, "No se pudo liberar la memoria de PID[%d]", pcb->id_proceso);
 	}
 	pthread_mutex_lock(&mutex_suspended_blocked);
 	list_add(colaSuspendedBlocked, pcb);
 	pthread_mutex_unlock(&mutex_suspended_blocked);

 	sem_post(&sem_grado_multiprogramacion);
 }

void estado_suspended_ready(void ) {

	while(1) {
		sem_wait(&sem_suspended_ready);
		pthread_mutex_lock(&mutex_suspended_blocked);
		pcb *pcb = list_remove(colaSuspendedBlocked,list_size(colaSuspendedBlocked));
		pthread_mutex_unlock(&mutex_suspended_blocked);

 		pthread_mutex_lock(&mutex_suspended_ready);
 		list_add(colaSuspendedReady, pcb);
 		pthread_mutex_unlock(&mutex_suspended_ready);

 		log_info(kernel_logger, "PID[%d] ingresa a SUSPENDED-READY...", pcb->id_proceso);
 		sem_post(&sem_admitir);
	}
}// QUE ESTA PASANDO ACA? RESOLVER

