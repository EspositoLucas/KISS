#include "planificacionUtils.h"

// CONFIG_ALGORITMO

//t_algoritmo_planificacion obtener_algoritmo(){
//
// 	 t_algoritmo_planificacion switcher;
// 	 char* algoritmo = obtener_de_config(config, "ALGORITMO_PLANIFICACION");
//
// 	    //FIFO
// 	 if (strcmp(algoritmo,"FIFO") == 0)
// 	 {
// 		 switcher = FIFO;
// 	     log_info(logger, "El algoritmo de planificacion elegido es FIFO.");
// 	 }
//
// 	    //SFJ SIN DESALOJO
// 	 if (strcmp(algoritmo,"SRT") == 0)
// 	 {
// 		 switcher = SRT;
// 	     log_info(logger, "El algoritmo de planificacion elegido es SRT.");
// 	 }
// 	 return switcher;
//}

// AVISOS A MEMORIA - CONSOLA

// void avisarAModulo(int socket_fd,op_code codigo){
	// enviar_datos(socket_fd, &codigo, sizeof(op_code));

// }

// void enviar_pcb_a_memoria(pcb *pcb, int socket_memoria) {
// enviar_pcb(pcb,socket_memoria);
// }


//uint32_t obtener_entrada_tabla_de_pagina(int socket_fd) {
//	uint32_t numero;
//	t_paquete *paquete = crear_paquete_con_codigo_de_operacion(INICIALIZAR_ESTRUCTURAS);
//	enviar_paquete(paquete, socket_fd);
//	eliminar_paquete(paquete);
//
//	recibir_datos(socket_fd, &numero, sizeof(uint32_t));
//
//	return numero;
//}


// }


//void eliminar_pcb(pcb *pcb) {
//	list_destroy_and_destroy_elements(pcb->instrucciones, free);
//	free(pcb);
//}


// LISTAS

// void inicializar_listas(void){

// 	colaNew =list_create();
// 	colaReady = list_create();
// 	colaExec = list_create();
// 	colaBlocked = list_create();
//  colaSuspendedBlocked = list_create();
//  colaReadySuspended=list_create();
// 	colaExit = list_create();
// }


// SEMAFOROS

// void inicializar_semaforos(void){

// 	pthread_mutex_init(&mutexBlockSuspended, NULL);
// 	pthread_mutex_init(&mutexReadySuspended, NULL);
// 	pthread_mutex_init(&mutexNew, NULL);
// 	pthread_mutex_init(&mutexReady, NULL);
// 	pthread_mutex_init(&mutexBlocked, NULL);
// 	pthread_mutex_init(&mutexExec, NULL);
// 	pthread_mutex_init(&mutexExit, NULL);

// 	sem_init(&analizarSuspension, 0, 0);
// 	sem_init(&suspensionFinalizada, 0, 0);
// 	sem_init(&contadorNew, 0, 0); // Estado New
// 	sem_init(&contadorReady, 0, 0); // Estado Ready
// 	sem_init(&contadorExec, 0, 0); // Estado Exec
// 	sem_init(&contadorProcesosEnMemoria, 0, 0);	
// 	sem_init(&multiprogramacion, 0, gradoMultiprogramacion); // Memoria
// 	sem_init(&contadorBlocked, 0, 0);
// 	sem_init(&largoPlazo, 0, 1);
// 	sem_init(&contadorReadySuspended, 0, 0);
// 	sem_init(&medianoPlazo, 0, 1);
// }


//..................................... DESTRUCCIONES............................................................................ 



// SEMAFOROS

// void destruir_semaforos(void){

// 	pthread_mutex_destroy(&mutexNew);
// 	pthread_mutex_destroy(&mutexReady);
// 	pthread_mutex_destroy(&mutexBlocked);
// 	pthread_mutex_destroy(&mutexExec);
// 	pthread_mutex_destroy(&mutexExit);
// 	pthread_mutex_destroy(&mutexBlockSuspended);
// 	pthread_mutex_destroy(&mutexReadySuspended);

// 	sem_destroy(&contadorNew);
// 	sem_destroy(&contadorReady);
// 	sem_destroy(&contadorExec);
// 	sem_destroy(&multiprogramacion);
// 	sem_destroy(&contadorBlocked);
// 	sem_destroy(&analizarSuspension);
// 	sem_destroy(&suspensionFinalizada);
// 	sem_destroy(&largoPlazo);
// 	sem_destroy(&contadorReadySuspended);
// 	sem_destroy(&medianoPlazo);

// }


// LISTAS

// void destruir_listas(void){

// 	destruirListaYElementos(colaNew);
// 	destruirListaYElementos(colaReady);
// 	destruirListaYElementos(colaExec);
// 	destruirListaYElementos(colaBlocked);
// 	destruirListaYElementos(colaExit);
	
// }

// void destruirListaYElementos(t_list* unaLista){
//     list_clean_and_destroy_elements(unaLista, free);
//     list_destroy(unaLista);
// }