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
	pcb->tiempo_de_bloqueo = 0;

	return pcb;
}


// MANEJO DE ESTADOS PCBs


// Hilos para las transiciones


//void iniciar_planificador_largo_plazo(void) {

//	generador_de_id = 0;
//	pthread_mutex_init(&mutex_new, NULL);
//	pthread_mutex_init(&mutex_exit, NULL);
//	pthread_mutex_init(&mutex_generador_id, NULL);
//	sem_init(&sem_admitir, 0, 0);
//	sem_init(&sem_exit, 0, 0);
//	sem_init(&sem_grado_multiprogramacion, 0, config_valores_kernel->grado_multiprogramacion);
//	colaNew = list_create();
//	colaExit = list_create();
//	pthread_create(&thread_exit, NULL, (void *)estado_exit, NULL);
//	pthread_create(&thread_admitir, NULL, (void *)transicion_admitir, NULL);
//	pthread_detach(thread_exit);
//	pthread_detach(thread_admitir);
//}



// void agregarANewPcb(pcb* pcb) {

// 	pthread_mutex_lock(&mutexNew);

// 	   list_add(pcbsNew, pcb);
//     pcb->estado = NUEVO;
// 	   log_info(logger, "[NEW] Entra el pcb de ID: %d a la cola.", pcb->id_pcb);

// 	pthread_mutex_unlock(&mutexNew);

//sem_post(&sem_admitir_transicion);
// }



//void transicion_admitir_por_prioridad(void) {
//
//	while(1) {
//		sem_wait(&sem_admitir_transicion);
//		sem_wait(&sem_grado_multiprogramacion);
//		pcb *pcb;
//
//		pthread_mutex_lock(&mutex_suspended_ready);
//		if(!list_is_empty(cola_suspended_ready)) {
//			pcb = list_remove(colaSuspendedReady,colaSuspendedReady[list_size(colaSuspendedReady)]);
//			pthread_mutex_unlock(&mutex_suspended_ready);
//			log_info(kernel_logger, "PID[%d] ingresa a READY desde SUSPENDED-READY", pcb->id_pcb);
//		} else {
//			pthread_mutex_unlock(&mutex_suspended_ready);
//			pthread_mutex_lock(&mutex_new);
//			pcb = list_remove(colaNew,list_size(colaNew));
//			pthread_mutex_unlock(&mutex_new);
//			pcb->pcb->tabla_paginas = obtener_entrada_tabla_de_pagina(socket_memoria);
//			log_info(kernel_logger, "PID[%d] ingresa a READY desde NEW", pcb->id_pcb);
//		}
//
//		pthread_mutex_lock(&mutex_ready);  //Se agrega a Ready elproce
//		list_add(colaReady, pcb);
//		pthread_mutex_unlock(&mutex_ready);
//		avisarAModulo(socket_memoria,INICIALIZAR_ESTRUCTURAS) ;
//		sem_post(&sem_ready);
//	}
//}

//  void finalizarPcb(pcb* pcb){
//       sem_wait(&sem_exit);
//  	pthread_mutex_lock(&mutexExit);
//  	list_add(colaExit, pcb);
// 	    pthread_mutex_unlock(&mutex_new);
// 		log_info(logger, "[EXIT] Finaliza el  pcb de ID: %d", pcb-> id_pcb);
//          obtener_entrada_tabla_de_pagina(socket_memoria) ;
//          avisarAModulo(socket_consola,FINALIZAR_CONSOLA) ;
// 			eliminar_pcb(pcb);
//     }




//................................. CORTO PLAZO.........................................................................................


// ENVIO PCB A CPU

// void enviarPcbACpu(pcb* pcb){

// 		int socket_cliente;
// 		socket_cliente= crear_conexion(config_valores.ip_cpu,config_valores.puerto_cpu_dispatch);
//		t_buffer* buffer
// 		t_paquete* paquete_pcb = crear_paquete();

// buffer = serializar_pcb(pcb);

// 		agregar_a_paquete(paquete_pcb,buffer,buffer->stream_size));


// 		enviar_paquete(nuevoPaquete, socket_cliente);

// 	eliminar_paquete(nuevoPaquete);

// 	liberar_conexion(socket_cliente);

// }



// HILOS


// void hiloReady_Exec(){
// 	while (1){
// 		pcb* pcbAEjecutar = obtenerSiguienteReady();

// 		if (pcbAEjecutar != NULL){
// 			list_add(colaExec,pcbAEjecutar);
// 			//enviarPcbACpu(pcbAEjecutar);
// 		}
// 	}
// }




// TRANSICIONES



// pcb* obtenerSiguienteReady(){
// 	pcb* pcbSeleccionado;

// 	int tamanioReady;
// 	tamanioReady = list_size(colaReady);
// 	int gradoMultiprogramacion;
// 	t_algoritmo_planificacion algoritmo = obtener_algoritmo();
// 	int ejecutando = list_size(colaExec);


// 	if (tamanioReady > 0 && ejecutando < gradoMultiprogramacion){
// 		switch(algoritmo){
// 		case FIFO:
// 			pcbSeleccionado = obtenerSiguienteFIFO();
// 			break;
// 		case SRT:
// 			pcbSeleccionado = obtenerSiguienteSRT();
// 			break;
// 		}
// 	}
// 	return pcbSeleccionado;
// }

// pcb* obtenerSiguienteFIFO(){

// 	log_info(logger,"Inicio la planificacion FIFO");

// 	pcb* pcbSeleccionado = list_remove(colaReady,0);


// 	return pcbSeleccionado;

// }

// pcb* obtenerSiguienteSRT(){
// 	log_info(logger,"Inicio la planificacion SRT");
// 	asignarEstimacionesApcbs();
// 	pcb* pcbElegido = elegirElDeMenorEstimacion();
// 	return pcbElegido;
// }

// pcb* elegirElDeMenorEstimacion(){
// 	int tamanioReady = list_size(colaReady);
// 	pcb* pcbSeleccionado;
// 	pcb* pcbAux;
// 	int indiceElegido = 0;

// 	float pcbMasCorto;

// 	pcbMasCorto = pcbAux->estimacion_rafaga;

// 	for(int i = 0; i < tamanioReady; i++){
// 		pcb* pcbAux = list_get(colaReady,i);

// 		if(pcbMasCorto > pcbAux->estimacion_rafaga){
// 			pcbMasCorto = pcbAux->estimacion_rafaga;
// 			indiceElegido = i;
// 		}

// 	}

// 	pcbSeleccionado = list_remove(colaReady,indiceElegido);

// 	return pcbSeleccionado;
// }

// void asignarEstimacionesApcbs(){

// 	int tamanioReady = list_size(colaReady);

	/*for(int i = 0; i < tamanioReady; i++){
		pcb* pcb = list_get(colaReady,i);
		float realAnterior = pcb->instrucciones[(pcb->program_counter)-1]
		pcb->estimacion_rafaga = calculoEstimacionpcb(realAnterior); // le paso a calculoEstimacionpcb la rafaga real anterior
	}*/
//}  revisar bien esta funcion

// float calculoEstimacionpcb(float realAnterior){

// 	float alfa;
// 	float estimacionInicial;

// 	float estimacion_rafaga = alfa * realAnterior + (1 - alfa) * estimacionInicial;

// 	return estimacion_rafaga;
// }


// void agregarABlocked(pcb* pcb){		

// 	sem_wait(&contadorExec);

// 	pthread_mutex_lock(&mutexBlocked);

// 	list_add(listaBlock, pcb);
// 	log_info(logger, "[BLOCKED] Entra el procso de PID: %d a la cola.", pcb->id_pcb);

// 	pthread_mutex_unlock(&mutexBlocked);
// 	sem_post(&contadorBlock);

// 	sem_post(&analizarSuspension);
// 	sem_wait(&suspensionFinalizada);
// }

//................................. MEDIANO PLAZO.........................................................................................


// TRANSICIONES_ ESTADOS


// void iniciar_planificador_mediano_plazo(void) {
// 	pthread_mutex_init(&mutex_suspended_ready, NULL);
// 	pthread_mutex_init(&mutex_suspended_blocked, NULL);
// 	sem_init(&sem_suspended_ready, 0, 0);
// 	colaSuspendedBlocked =list_create();
// 	colaSuspendedready = list_create();
// 	pthread_create(&thread_suspended_ready, NULL, (void *)estado_suspended_ready, NULL);
// 	pthread_detach(thread_suspended_ready);
// }

// void transicion_suspender(pcb *pcb) {
// 	log_info(kernel_logger, "PID[%d] ingresa a SUSPENDED-BLOCKED", pcb->id_proceso);
// 	pcb->estado = SUSPENDED_BLOCKED;
// 	enviar_pcb_a_memoria(pcb, socket_memoria);
// 	recibir_datos(socket_memoria, &codigo, sizeof(op_code));  // ver como recibir codigo de operacion de memoria
// 	if(codigo != PCB_LIBERADO) {
// 		log_error(kernel_logger, "No se pudo liberar la memoria de PID[%d]", pcb->id_proceso);
// 	}
// 	pthread_mutex_lock(&mutex_suspended_blocked);
// 	list_add(colaSuspendedBlocked, pcb);
// 	pthread_mutex_unlock(&mutex_suspended_blocked);

// 	sem_post(&sem_grado_multiprogramacion);
// }

// void estado_suspended_ready(void ) {
// 	while(1) {
// 		sem_wait(&sem_suspended_ready);
// 		pthread_mutex_lock(&mutex_suspended_blocked);
// 		pcb *pcb = list_remove(colaSuspendedBlocked,colaSuspendedBlocked[list_size(colaSuspendedBlocked)]);
// 		pthread_mutex_unlock(&mutex_suspended_blocked);

// 		pthread_mutex_lock(&mutex_suspended_ready);
// 		list_add(colaSuspendedReady, pcb);
// 		pthread_mutex_unlock(&mutex_suspended_ready);

// 		log_info(kernel_logger, "PID[%d] ingresa a SUSPENDED-READY...", pcb->id_proceso);
// 		sem_post(&sem_admitir);
// 	}
// }

