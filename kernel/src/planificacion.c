#include "planificacion.h"

int proceso_ejecutando;

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
	pcb->rafaga_anterior = 0;
	pcb->instrucciones = consola->instrucciones;

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
	pthread_create(&thread_exit, NULL, (void *)finalizarPcb, NULL);
	pthread_create(&thread_admitir, NULL, (void *)transicion_admitir_por_prioridad, NULL);
	pthread_detach(thread_exit);
	pthread_detach(thread_admitir);
}



 void agregarANewPcb(proceso* proceso) {

 	pthread_mutex_lock(&mutex_new);

 	   list_add(colaNew, proceso);
 	   printf("PCB agregada a colaNew\n");
 	   proceso->pcb->estado_proceso = NUEVO;
 	   printf("PCB estado Nuevo\n");
 	   printf("[NEW] Entra el pcb de ID: %d a la cola.\n", proceso->pcb->id_proceso);
 	   chequear_lista_pcbs(colaNew);

 	pthread_mutex_unlock(&mutex_new);

 	sem_post(&sem_admitir);
 }



void transicion_admitir_por_prioridad(void) {

	while(1) {
		sem_wait(&sem_admitir); // IDEM ANTERIOR
		sem_wait(&sem_grado_multiprogramacion);
		proceso* proceso;

		pthread_mutex_lock(&mutex_suspended_ready);
		if(!list_is_empty(colaSuspendedReady)) {
			proceso = list_remove(colaSuspendedReady,list_size(colaSuspendedReady)- 1);
			pthread_mutex_unlock(&mutex_suspended_ready);
			printf( "PID[%d] ingresa a READY desde SUSPENDED-READY", proceso->pcb->id_proceso);
		} else {
			pthread_mutex_unlock(&mutex_suspended_ready);
			pthread_mutex_lock(&mutex_new);
			proceso = list_remove(colaNew,list_size(colaNew) -1 );
			pthread_mutex_unlock(&mutex_new);
			//pcb = obtener_entrada_tabla_de_pagina(socket_memoria,pcb); // lo comento hasta que funcione memoria
			printf("PID[%d] ingresa a READY desde NEW", proceso->pcb->id_proceso);
		}

		pthread_mutex_lock(&mutex_ready);  //Se agrega a Ready el proceso
		list_add(colaReady, proceso);
		pthread_mutex_unlock(&mutex_ready);
		sem_post(&sem_ready);
	}
}

void finalizarPcb(void){
	sem_wait(&sem_exit);
	proceso* proceso ;
  	pthread_mutex_lock(&mutex_exit);
  	proceso = list_remove(colaExit, 0);
  	printf("Finalizo el pid: %d\n", proceso->pcb->id_proceso);
  	pthread_mutex_unlock(&mutex_exit);
// 	printf("[EXIT] Finaliza el  pcb de ID: %d", proceso->pcb-> id_proceso);
	enviar_pcb_a_memoria(proceso->pcb, socket_memoria,LIBERAR_ESTRUCTURAS);

	op_code codigo = esperar_respuesta_memoria(socket_memoria);
	if(codigo != ESTRUCTURAS_LIBERADAS) {
		log_error(kernel_logger, "No se pudo eliminar memoria de PID[%d]", proceso->pcb->id_proceso);
	}
	avisarAModulo(proceso->socket,FINALIZAR_CONSOLA) ;
 	eliminar_pcb(proceso->pcb);
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


 void estadoReady(void){
 	while(1){
 		sem_wait(&sem_ready);
 		algoritmo algoritmo = obtener_algoritmo();
 		if(algoritmo == SRT){
 			if(proceso_ejecutando == 1){
 				interrumpir_cpu();
 				sem_wait(&sem_desalojo);
 			}
 		}

 		chequear_lista_pcbs(colaReady);

 		proceso* siguiente_proceso = obtenerSiguienteReady();
 	 	printf("proceso a ejecutar: %d\n", siguiente_proceso->pcb->id_proceso);
 		pthread_mutex_lock(&mutex_exec);
 		list_add(colaExec, siguiente_proceso);
 		pthread_mutex_unlock(&mutex_exec);
 		sem_post(&sem_exec);
 	}
 }


void estadoExec(void){
	while(1){
		sem_wait(&sem_exec);

		pthread_mutex_lock(&mutex_exec);

		printf("Arrancamos con exec \n");
		proceso* proceso = list_remove(colaExec,0);

		proceso_ejecutando = 1;
		pthread_mutex_unlock(&mutex_exec);
		printf( "PID[%d] ingresa a EXEC\n", proceso->pcb->id_proceso);
		uint32_t inicio_cpu = get_time(); // logueo el tiempo en el que se va

		enviarPcb(socket_dispatch, proceso->pcb);
		printf("PCB enviada \n");

		op_code respuesta_cpu = recibir_operacion_nuevo(socket_dispatch);
		proceso->pcb = recibirPcb(socket_dispatch); // aca rompe el hilo porque no se hizo la conexion a cpu

		printf("PCB recibida \n");
		uint32_t finalizacion_cpu = get_time();
		pthread_mutex_lock(&mutex_exec);
		proceso_ejecutando = 0;
		pthread_mutex_unlock(&mutex_exec);

		proceso->pcb->rafaga_anterior = inicio_cpu - finalizacion_cpu;

		instruccion *instruccion_ejecutada = list_get(proceso->pcb->instrucciones, (proceso->pcb->program_counter - 1)); // agarro la instruccion ya ejecutada por cpu
		printf("Ultima instruccion ejecutada: %d \n",instruccion_ejecutada->codigo);


		switch(instruccion_ejecutada->codigo){
		case IO:
			printf("Me meto en IO \n");
			pthread_mutex_lock(&mutex_blocked);
			proceso->pcb->estado_proceso = BLOQUEADO;
			proceso->tiempo_inicio_bloqueo = get_time();
			list_add(colaBlocked, proceso);
			chequear_lista_pcbs(colaBlocked);
			pthread_mutex_unlock(&mutex_blocked);
			sem_post(&sem_blocked); // despertar bloqueado
			break;
		case EXIT:
			printf("Me meto en EXIT\n");
			pthread_mutex_lock(&mutex_exit);
			proceso->pcb->estado_proceso = FINALIZADO;
			list_add(colaExit, proceso);
			chequear_lista_pcbs(colaExit);
			pthread_mutex_unlock(&mutex_exit);
			sem_post(&sem_exit); // despertar exit
			break;
		default: // ready write o noop
			pthread_mutex_lock(&mutex_ready);
			proceso->pcb->estado_proceso = LISTO;
			list_add(colaReady, proceso);
			pthread_mutex_unlock(&mutex_ready);
			sem_post(&sem_desalojo);
			sem_post(&sem_ready);
			break;
		}

}

}

void estadoBlockeado(void){

	while(1){
		sem_wait(&sem_blocked);
		uint32_t tiempoMaxDeBloqueo = config_valores_kernel.tiempo_maximo_bloqueado;
		printf("Tiempo max de bloqueo segun config: %d\n",tiempoMaxDeBloqueo);
		pthread_mutex_lock(&mutex_blocked);
		proceso* proceso = list_remove(colaBlocked,0);
		pthread_mutex_unlock(&mutex_blocked);
		chequear_lista_pcbs(colaBlocked);
		printf("Ejecuto io del pid: %d\n", proceso->pcb->id_proceso);
//		printf("PID[%d] ingresa a BLOCKED", proceso->pcb->id_proceso);

		uint32_t tiempoQueLLevaEnBlock = get_time() - proceso->tiempo_inicio_bloqueo;

		if (tiempoQueLLevaEnBlock > tiempoMaxDeBloqueo){ // suspendo de entrada
			printf("caso 1\n");
			transicion_suspender(proceso); //suspender para mediano plazo
			ejecutarIO(proceso->pcb->tiempo_de_bloqueo);

		} else if (tiempoQueLLevaEnBlock + proceso->pcb->tiempo_de_bloqueo > tiempoMaxDeBloqueo){ // suspendo en el medio
			printf("caso 2\n");
			uint32_t tiempoIOAntesDeSuspender = tiempoMaxDeBloqueo - tiempoQueLLevaEnBlock;
			ejecutarIO(tiempoIOAntesDeSuspender); // ejecutar hasta que sea necesario suspender
			printf("Ejecute %d de IO\n", tiempoIOAntesDeSuspender);
			transicion_suspender(proceso); //suspender para mediano plazo
			printf("Suspendi el proceso\n");
			ejecutarIO(proceso->pcb->tiempo_de_bloqueo - tiempoIOAntesDeSuspender); // ejecuto el io restante
			printf("Ejecuto IO pendiente: %d\n", proceso->pcb->tiempo_de_bloqueo - tiempoIOAntesDeSuspender);

		} else { // la ejecucion de io + el tiempo que lleva en block es menor al tiempo max de blockeo
			printf("caso 3\n");
			ejecutarIO(proceso->pcb->tiempo_de_bloqueo);
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

 void transicion_suspender(proceso *proceso) {

 	printf("PID[%d] ingresa a SUSPENDED-BLOCKED", proceso->pcb->id_proceso);
 	proceso->pcb->estado_proceso = LISTO_SUSPENDIDO;
	enviar_pcb_a_memoria(proceso->pcb, socket_memoria, SUSPENDER_PCB);
	proceso->pcb = recibirPcb(socket_memoria);
 	op_code codigo = esperar_respuesta_memoria(socket_memoria);
 	if(codigo != ESPACIO_PCB_LIBERADO) {
 		log_error(kernel_logger, "No se pudo liberar la memoria de PID[%d]", proceso->pcb->id_proceso);
 	}
 	pthread_mutex_lock(&mutex_suspended_blocked);
 	list_add(colaSuspendedBlocked, proceso);
 	pthread_mutex_unlock(&mutex_suspended_blocked);

 	sem_post(&sem_grado_multiprogramacion);
 }

void estado_suspended_ready(void ) {

	while(1) {
		sem_wait(&sem_suspended_ready);
		pthread_mutex_lock(&mutex_suspended_blocked);
		proceso *proceso= list_remove(colaSuspendedBlocked,list_size(colaSuspendedBlocked));
		pthread_mutex_unlock(&mutex_suspended_blocked);

 		pthread_mutex_lock(&mutex_suspended_ready);
 		list_add(colaSuspendedReady, proceso);
 		pthread_mutex_unlock(&mutex_suspended_ready);

 		log_info(kernel_logger, "PID[%d] ingresa a SUSPENDED-READY...", proceso->pcb->id_proceso);
 		sem_post(&sem_admitir);
	}
}

