#include "planificacion.h"


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
	printf("Estimacion incial de config valores kernel: %f\n", config_valores_kernel.estimacion_inicial);
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
 	   proceso->pcb->estado_proceso = NUEVO;
 	  log_info(kernel_logger_info, "PID[%d] ingresa a NEW \n", proceso->pcb->id_proceso);
 	   chequear_lista_pcbs(colaNew);

 	pthread_mutex_unlock(&mutex_new);
 	log_info(kernel_logger_info,"SE HACE UN SEM POST ADMITIR");
 	sem_post(&sem_admitir);
 }



void transicion_admitir_por_prioridad(void) {

	while(1) {
		sem_wait(&sem_admitir);
		log_info(kernel_logger_info,"SE HACE UN SEM WAIT ADMITIR");
		sem_wait(&sem_grado_multiprogramacion);
		proceso* proceso;

		pthread_mutex_lock(&mutex_suspended_ready);
		if(!list_is_empty(colaSuspendedReady)) {
			proceso = list_remove(colaSuspendedReady,list_size(colaSuspendedReady)- 1);
			pthread_mutex_unlock(&mutex_suspended_ready);
			log_info(kernel_logger_info, "PID[%d] ingresa a READY desde SUSPENDED-READY \n", proceso->pcb->id_proceso);
		} else {
			pthread_mutex_unlock(&mutex_suspended_ready);
			pthread_mutex_lock(&mutex_new);
			proceso = list_remove(colaNew,list_size(colaNew) -1 );
			pthread_mutex_unlock(&mutex_new);
			proceso->pcb = obtener_entrada_tabla_de_pagina(socket_memoria,proceso->pcb); // lo comento hasta que funcione memoria
			log_info(kernel_logger_info, "PID[%d] ingresa a READY desde NEW \n", proceso->pcb->id_proceso);
		}



		pthread_mutex_lock(&mutex_ready);  //Se agrega a Ready el proceso
		list_add(colaReady, proceso);
		pthread_mutex_unlock(&mutex_ready);
		transicion_interrupcion();
//		algoritmo algoritmo = obtener_algoritmo();
//		//printf("HAY O NO UN PROCESO EJECUTANDO CUANDO SE DESPIERTA READY: %d\n",proceso_ejecutando);
//
//		if(algoritmo == SRT){
//			 pthread_mutex_lock(&mutex_exec);
//			 log_info(kernel_logger_info,"VALOR PROCESO_EJECUTANDO %d \n ",list_size(colaExec) );
//		 			if(!list_is_empty(colaExec)){
//		 				pthread_mutex_unlock(&mutex_exec);
//		 				pthread_mutex_lock(&mutex_interrupcion);
//		 				 interrupcion = 1;
//		 				 pthread_mutex_unlock(&mutex_interrupcion);
//		 				interrumpir_cpu();
//		 			} else {
//		 				printf("NO HAY PROCESO EJECUTANDO \n" );
//		 				pthread_mutex_unlock(&mutex_exec);
//		 			}
//		 		}
		log_info(kernel_logger_info,"SE HACE UN SEM POST READY DESPUES DE ADMITIR POR PRIORIDAD");
		sem_post(&sem_ready);
	}
}

void finalizarPcb(void){

	while(1) {
	sem_wait(&sem_exit);
	proceso* proceso ;
  	pthread_mutex_lock(&mutex_exit);
  	proceso = list_remove(colaExit, 0);
  	pthread_mutex_unlock(&mutex_exit);
  	log_info(kernel_logger_info, "[EXIT]Sale de EXIT y Finaliza el  pcb de ID: %d\n", proceso->pcb-> id_proceso);
	enviarPcb(socket_memoria,proceso->pcb);
	log_info(kernel_logger_info, "Enviando a memoria liberar estructuras del proceso \n");
	op_code codigo = esperar_respuesta_memoria(socket_memoria);

	log_info(kernel_logger_info, "Respuesta memoria de estructuras liberadas del proceso recibida \n");
	if(codigo != ESTRUCTURAS_LIBERADAS) {
		log_error(kernel_logger_info, "No se pudo eliminar memoria de PID[%d]\n", proceso->pcb->id_proceso);
	}
	avisarAModulo(proceso->socket,FINALIZAR_CONSOLA) ;
	log_info(kernel_logger_info, "Enviando a consola que finalizo el proceso\n");
 	eliminar_pcb(proceso->pcb);
 	sem_post(&sem_grado_multiprogramacion);

	}
}




//................................. CORTO PLAZO.........................................................................................

void iniciar_planificador_corto_plazo(void) {

	pthread_mutex_init(&mutex_ready, NULL);
	pthread_mutex_init(&mutex_blocked, NULL);
	pthread_mutex_init(&mutex_exec, NULL);
	pthread_mutex_init(&mutex_interrupcion, NULL);
	sem_init(&sem_ready, 0, 0);
	sem_init(&sem_exec, 0, 0);
	sem_init(&sem_blocked, 0, 0);
	sem_init(&sem_desalojo, 0, 1);
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
		log_info(kernel_logger_info,"SE HACE UN SEM WAIT READY");
		if(obtener_algoritmo()==SRT){
			sem_wait(&sem_desalojo); // solo si la lista no es vacia
		}
		log_info(kernel_logger_info,"SE HACE UN SEM WAIT DESALOJO");
		pthread_mutex_lock(&mutex_ready);
		if(list_is_empty(colaReady)){
			pthread_mutex_unlock(&mutex_ready);
			continue;
		}
		pthread_mutex_unlock(&mutex_ready);
 		proceso* siguiente_proceso = obtenerSiguienteReady();
 		pthread_mutex_lock(&mutex_exec);
 		list_add(colaExec, siguiente_proceso);
 		pthread_mutex_unlock(&mutex_exec);
 		log_info(kernel_logger_info, "PID[%d] ingresa a EXEC\n", siguiente_proceso->pcb->id_proceso);
		log_info(kernel_logger_info,"SE HACE UN SEM POST EXEC");
 		sem_post(&sem_exec);
 	}
 }


void estadoExec(void){
	while(1){
		sem_wait(&sem_exec);
		log_info(kernel_logger_info,"SE HACE UN SEM WAIT EXEC");
		pthread_mutex_lock(&mutex_exec);
		proceso* proceso = list_get(colaExec,0);
		pthread_mutex_unlock(&mutex_exec);
//		log_info(kernel_logger_info, "PID[%d] sale de EXEC\n", proceso->pcb->id_proceso);
		int inicio_cpu = get_time(); // logueo el tiempo en el que se va

		enviarPcb(socket_dispatch, proceso->pcb);
		log_info(kernel_logger_info, "PCB enviada cpu para ejecucion");
		op_code respuesta_cpu = recibir_operacion_nuevo(socket_dispatch);

		proceso->pcb = recibirPcb(socket_dispatch);

		int finalizacion_cpu = get_time();
		log_info(kernel_logger_info,"VALOR INTERRUPCION %d \n ", interrupcion);
		instruccion *instruccion_exec = list_get(proceso->pcb->instrucciones, (proceso->pcb->program_counter - 1));
		if(interrupcion && instruccion_exec->codigo != IO && instruccion_exec->codigo != EXIT  ){
			proceso->pcb->estimacion_rafaga = proceso->pcb->estimacion_rafaga - (finalizacion_cpu - inicio_cpu);
			pthread_mutex_lock(&mutex_ready);
			list_add(colaReady,proceso);
			pthread_mutex_unlock(&mutex_ready);
			log_info(kernel_logger_info, "PID[%d] ingresa a READY despues de interrupcion \n", proceso->pcb->id_proceso);
			pthread_mutex_lock(&mutex_exec);
			procesoAux = list_remove(colaExec,0);
			pthread_mutex_unlock(&mutex_exec);
			log_info(kernel_logger_info,"SE HACE UN SEM POST DESALOJO");
			sem_post(&sem_desalojo);
			continue;
		}else{
			interrupcion = 0;
			log_info(kernel_logger_info,"SE HACE UN SEM POST DESALOJO");
			sem_post(&sem_desalojo);
		}


		log_info(kernel_logger_info, "PCB recibida de cpu para calcular estimaciones (SRT) \n");

		log_info(kernel_logger_info,"El tiempo de inicio de cpu es: %d y el tiempo de fin %d\n", inicio_cpu, finalizacion_cpu);

		proceso->pcb->rafaga_anterior = finalizacion_cpu - inicio_cpu;
		calculoEstimacionProceso(proceso);

		instruccion *instruccion_ejecutada = list_get(proceso->pcb->instrucciones, (proceso->pcb->program_counter - 1)); // agarro la instruccion ya ejecutada por cpu
		log_info(kernel_logger_info, "PID[%d] Ultima instruccion ejecutada: %d",proceso->pcb->id_proceso,instruccion_ejecutada->codigo);

		pthread_mutex_lock(&mutex_exec);
		procesoAux = list_remove(colaExec,0);
		pthread_mutex_unlock(&mutex_exec);

		switch(instruccion_ejecutada->codigo){
		case IO:
			pthread_mutex_lock(&mutex_blocked);
			proceso->pcb->estado_proceso = BLOQUEADO;
			proceso->tiempo_inicio_bloqueo = get_time();
			printf("Tiempo que el pid[%d] ingresa a la cola de blocked: %d\n",proceso->pcb->id_proceso, proceso->tiempo_inicio_bloqueo);
			pthread_mutex_lock(&mutex_exec);
			//proceso_ejecutando = 0;
			pthread_mutex_unlock(&mutex_exec);
			list_add(colaBlocked, proceso);
			chequear_lista_pcbs(colaBlocked);
			pthread_mutex_unlock(&mutex_blocked);
			log_info(kernel_logger_info, "PID[%d] Sale de EXEC y entra en BLOCKED \n", proceso->pcb->id_proceso);
			log_info(kernel_logger_info,"SE HACE UN SEM POST BLOCKED");
			sem_post(&sem_blocked); // despertar bloqueado
			break;
		case EXIT:
			pthread_mutex_lock(&mutex_exit);
			proceso->pcb->estado_proceso = FINALIZADO;
			pthread_mutex_lock(&mutex_exec);
			//proceso_ejecutando = 0;
			pthread_mutex_unlock(&mutex_exec);
			list_add(colaExit, proceso);
			chequear_lista_pcbs(colaExit);
			pthread_mutex_unlock(&mutex_exit);
			log_info(kernel_logger_info, "PID[%d] Sale de EXEC y entra a EXIT \n", proceso->pcb->id_proceso);
			printf("proceso antes de salir de exit \n");
			log_info(kernel_logger_info,"SE HACE UN SEM POST EXIT");
			sem_post(&sem_exit); // despertar exit
			printf("proceso antes de semaforo post exit  \n");
			sem_post(&sem_ready);
			break;
		default: // read, write o noop
			pthread_mutex_lock(&mutex_ready);
			proceso->pcb->estado_proceso = LISTO;
			pthread_mutex_lock(&mutex_exec);
			//proceso_ejecutando = 0;
			pthread_mutex_unlock(&mutex_exec);
			list_add(colaReady, proceso);
			pthread_mutex_unlock(&mutex_ready);
			log_info(kernel_logger_info, "PID[%d] Sale de Exec y entra a READY \n", proceso->pcb->id_proceso);
			log_info(kernel_logger_info,"SE HACE UN SEM POST READY EN EL CASE DE R,W O NOOP");
			sem_post(&sem_ready);
			sem_post(&sem_desalojo);
			break;
		}
//		log_info(kernel_logger_info,"SE HACE UN SEM POST DESALOJO");
		//sem_post(&sem_desalojo);
		//sem_post(&semHayParaEjecutar);

	}

}

void estadoBlockeado(void){

	while(1){
		sem_wait(&sem_blocked);
		log_info(kernel_logger_info,"SE HACE UN SEM WAIT BLOCKED");
		uint32_t tiempoMaxDeBloqueo = config_valores_kernel.tiempo_maximo_bloqueado;
		pthread_mutex_lock(&mutex_blocked);
		proceso* proceso = list_remove(colaBlocked,0);
		pthread_mutex_unlock(&mutex_blocked);
		log_info(kernel_logger_info, "PID[%d] sale de BLOCKED \n", proceso->pcb->id_proceso);
		int tiempo_que_sale_de_block = get_time();
		printf("El tiempo que el pid[%d] sale de la cola blocked es: %d\n",proceso->pcb->id_proceso, tiempo_que_sale_de_block);
//		chequear_lista_pcbs(colaBlocked);
		log_info(kernel_logger_info, "PID[%d] ejecuta IO \n", proceso->pcb->id_proceso);
		int tiempoQueLLevaEnBlock =  tiempo_que_sale_de_block - proceso->tiempo_inicio_bloqueo;
		printf("Tiempo que el pid[%d] lleva en block: %d y el tiempo de io que tiene que hacer de las instrucciones: %f \n", proceso->pcb->id_proceso, tiempoQueLLevaEnBlock, proceso->pcb->tiempo_de_bloqueo);

		if (tiempoQueLLevaEnBlock > tiempoMaxDeBloqueo){ // suspendo de entrada
			log_info(kernel_logger_info, "PID[%d] suspendido por superar tiempo max de bloqueo \n", proceso->pcb->id_proceso);
			transicion_suspender(proceso); //suspender para mediano plazo
			ejecutarIO(proceso->pcb->tiempo_de_bloqueo);
			log_info(kernel_logger_info,"SE HACE UN SEM POST SUSPENDED READY");
			sem_post(&sem_suspended_ready);

		} else if (tiempoQueLLevaEnBlock + proceso->pcb->tiempo_de_bloqueo > tiempoMaxDeBloqueo){ // suspendo en el medio
			log_info(kernel_logger_info, "Mandar a suspension al proceso %d por superar tiempo max de bloqueo \n", proceso->pcb->id_proceso);
			uint32_t tiempoIOAntesDeSuspender = tiempoMaxDeBloqueo - tiempoQueLLevaEnBlock;
			ejecutarIO(tiempoIOAntesDeSuspender); // ejecutar hasta que sea necesario suspender
			log_info(kernel_logger_info, "PID[%d] ejecutar io antes de suspender \n",proceso->pcb->id_proceso);
			transicion_suspender(proceso); //suspender para mediano plazo
			log_info(kernel_logger_info, "PID[%d] suspendido, iniciar IO restante\n", proceso->pcb->id_proceso);
			ejecutarIO(proceso->pcb->tiempo_de_bloqueo - tiempoIOAntesDeSuspender); // ejecuto el io restante
			log_info(kernel_logger_info, "PID[%d] hizo IO restante\n", proceso->pcb->id_proceso);
			log_info(kernel_logger_info,"SE HACE UN SEM POST SUSPENDED READY");
			sem_post(&sem_suspended_ready);

		} else { // la ejecucion de io + el tiempo que lleva en block es menor al tiempo max de blockeo
			log_info(kernel_logger_info, "PID[%d] no supero el tiempo max bloqueo, ejecutar solo IO \n", proceso->pcb->id_proceso);
			ejecutarIO(proceso->pcb->tiempo_de_bloqueo);
			pthread_mutex_lock(&mutex_ready);
			list_add(colaReady,proceso);
			pthread_mutex_unlock(&mutex_ready);
			log_info(kernel_logger_info, "PID[%d] ingresa a READY desde IO \n", proceso->pcb->id_proceso);

			transicion_interrupcion();
			log_info(kernel_logger_info,"SE HACE UN SEM POST READY CUANDO EL IO NO SUPERA AL TIEMPO MAX DE BLOQUEO");
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

 	proceso->pcb->estado_proceso = LISTO_SUSPENDIDO;
	enviar_pcb_a_memoria(proceso->pcb, socket_memoria, SUSPENDER_PROCESO);
	log_info(kernel_logger_info, "Enviando a memoria para suspender proceso \n");
 	op_code codigo = esperar_respuesta_memoria(socket_memoria);
 	log_info(kernel_logger_info, "Respuesta de suspension de memoria recibida \n");
 	if(codigo != ESPACIO_PCB_LIBERADO) {
 		log_error(kernel_logger_info, "No se pudo liberar la memoria de PID[%d] \n", proceso->pcb->id_proceso);
 	}
 	pthread_mutex_lock(&mutex_suspended_blocked);
 	list_add(colaSuspendedBlocked, proceso);
 	pthread_mutex_unlock(&mutex_suspended_blocked);
 	log_info(kernel_logger_info, "PID[%d] ingresa a SUSPENDED-BLOCKED \n", proceso->pcb->id_proceso);
 	sem_post(&sem_grado_multiprogramacion);
 }

void estado_suspended_ready(void ) {

	while(1) {
		sem_wait(&sem_suspended_ready);
		pthread_mutex_lock(&mutex_suspended_blocked);
		proceso *proceso= list_remove(colaSuspendedBlocked,list_size(colaSuspendedBlocked) -1 );
		pthread_mutex_unlock(&mutex_suspended_blocked);
		log_info(kernel_logger_info, "PID[%d] sale de SUSPENDED-BLOCKED \n", proceso->pcb->id_proceso);

 		pthread_mutex_lock(&mutex_suspended_ready);
 		list_add(colaSuspendedReady, proceso);
 		pthread_mutex_unlock(&mutex_suspended_ready);

 		log_info(kernel_logger_info, "PID[%d] ingresa a SUSPENDED-READY \n", proceso->pcb->id_proceso);
		log_info(kernel_logger_info,"SE HACE UN SEM POST ADMITIR");
 		sem_post(&sem_admitir);
	}
}

