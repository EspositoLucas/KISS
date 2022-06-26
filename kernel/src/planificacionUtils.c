#include "planificacionUtils.h"

//--------------------------------------------------- FUNCIONES PLANIFICACION -------------------------------

void inciar_planificacion(){
	iniciar_planificador_largo_plazo();
	iniciar_planificador_corto_plazo();
	iniciar_planificador_mediano_plazo();
}



// --------------------------------------------------- FUNCIONES LARGO PLAZO ----------------------------------------------------------

// AVISOS y RECIBOS de MEMORIA - CONSOLA

 void avisarAModulo(int socket_fd,op_code codigo){
	 enviar_datos(socket_fd, &codigo, sizeof(op_code));

 }

 void enviar_pcb_a_memoria(pcb *pcb, int socket_memoria, op_code codigo) {
	 t_paquete* paquete = crear_paquete_con_codigo_de_operacion(LIBERAR_ESTRUCTURAS);
	 enviar_paquete(paquete, socket_memoria);
	 enviarPcb(socket_memoria,pcb);
	 eliminar_paquete(paquete);
 }


uint32_t obtener_entrada_tabla_de_pagina(int socket_fd) {
	uint32_t numero;
	t_paquete *paquete = crear_paquete_con_codigo_de_operacion(INICIALIZAR_ESTRUCTURAS);
	enviar_paquete(paquete, socket_fd);
	eliminar_paquete(paquete);

	recibir_datos(socket_fd, &numero, sizeof(uint32_t));

	return numero;
 }

 op_code esperar_respuesta_memoria(int socket_memoria) {
 	op_code codigo;
 	recibir_datos(socket_memoria, &codigo, sizeof(op_code));
 	return codigo;
 }



// ---------------------------------------------------- TIMER ----------------------------------------
 void iniciar_timer() {
 	pthread_mutex_init(&mutex_timer, NULL);
 	pthread_create(&th_timer, NULL, (void *)timer, NULL);
 	pthread_detach(th_timer);
 }

 void timer(void *data) {
	 tiempo = 0;
 	while(1) {
 		sleep(1);
 		pthread_mutex_lock(&mutex_timer);
 		tiempo++;
 		pthread_mutex_unlock(&mutex_timer);
 	}
 }

 uint32_t get_time() {
 	pthread_mutex_lock(&mutex_timer);
 	uint32_t tiempo_actual = tiempo * 1000;
 	pthread_mutex_unlock(&mutex_timer);
 	return tiempo_actual;
 }



// --------------------------------------------------- FUNCIONES CORTO PLAZO ----------------------------------------------------------


// CONFIG_ALGORITMO

algoritmo obtener_algoritmo(){

 	 algoritmo switcher;
 	 char* algoritmo = config_valores_kernel.algoritmo_planificacion;
 	    //FIFO
 	 if (strcmp(algoritmo,"FIFO") == 0)
 	 {
 		 switcher = FIFO;
 	     log_info(logger, "El algoritmo de planificacion elegido es FIFO.");
 	 }

 	    //SFJ SIN DESALOJO
 	 if (strcmp(algoritmo,"SRT") == 0)
 	 {
 		 switcher = SRT;
 	     log_info(logger, "El algoritmo de planificacion elegido es SRT.");
 	 }
 	 return switcher;
}

pcb* obtenerSiguienteReady(){
	pcb* procesoSeleccionado;
	int tamanioReady;
 	tamanioReady = list_size(colaReady);
 	int gradoMultiprogramacion = config_valores_kernel.grado_multiprogramacion;
 	algoritmo algoritmo = obtener_algoritmo();
 	int ejecutando = list_size(colaExec);


 	if (tamanioReady > 0 && ejecutando < gradoMultiprogramacion){
 		switch(algoritmo){
 		case FIFO:
 			procesoSeleccionado = obtenerSiguienteFIFO();
 			break;
 		case SRT:
 			procesoSeleccionado = obtenerSiguienteSRT();
 			break;
 		}
 	}
 	return procesoSeleccionado;
 }


 pcb* obtenerSiguienteFIFO(){

 	log_info(logger,"Inicio la planificacion FIFO");
 	pcb* procesoSeleccionado = list_remove(colaReady,0);
	return procesoSeleccionado;

}

pcb* obtenerSiguienteSRT(){

 	log_info(logger,"Inicio la planificacion SRT");
 	asignarEstimacionesAProcesos();
 	pcb* procesoElegido = elegirElDeMenorEstimacion();
 	return procesoElegido;
 }

pcb* elegirElDeMenorEstimacion(){

	int tamanioReady = list_size(colaReady);
	pcb* procesoSeleccionado;
	pcb* procesoAux = list_get(colaReady,0);
	int indiceElegido = 0;
	float procesoMasCorto;
	procesoMasCorto = procesoAux->estimacion_rafaga;
	for(int i = 0; i < tamanioReady; i++){
		pcb* procesoAux = list_get(colaReady,i);
		if(procesoMasCorto > procesoAux->estimacion_rafaga){
			procesoMasCorto = procesoAux->estimacion_rafaga;
			indiceElegido = i;
		}
	}

	procesoSeleccionado = list_remove(colaReady,indiceElegido);

	return procesoSeleccionado;
}

void asignarEstimacionesAProcesos(){
 	int tamanioReady = list_size(colaReady);
	for(int i = 0; i < tamanioReady; i++){
		pcb* proceso = list_get(colaReady,i);
		calculoEstimacionProceso(proceso);
	};
}  //revisar bien esta funcion

void calculoEstimacionProceso(pcb *proceso){
	float alfa = config_valores_kernel.alfa;
	float estimacionInicial = config_valores_kernel.estimacion_inicial;
	float realAnterior = proceso->rafaga_anterior;
	float nuevaEstimacion = alfa * realAnterior + (1 - alfa) * estimacionInicial;
	proceso->estimacion_rafaga = nuevaEstimacion;
}

void interrumpir_cpu(){
	t_paquete *paqueteAEnviar = crear_paquete_con_codigo_de_operacion(INTERRUPCION);
	enviar_paquete(paqueteAEnviar, socket_interrupt);
	eliminar_paquete(paqueteAEnviar);

}

void ejecutarIO(uint32_t tiempoIO){
	usleep(tiempoIO * 1000);
}



//..................................... INICIALIZACIONES BASE............................................................................

// LISTAS

 void inicializar_listas(void){

 	colaNew =list_create();
 	colaReady = list_create();
 	colaExec = list_create();
 	colaBlocked = list_create();
 	colaSuspendedBlocked = list_create();
 	colaSuspendedReady=list_create();
 	colaExit = list_create();
 }


// SEMAFOROS

 void inicializar_semaforos(void){

 	pthread_mutex_init(&mutex_suspended_ready, NULL);
 	pthread_mutex_init(&mutex_suspended_blocked, NULL);
 	pthread_mutex_init(&mutex_new, NULL);
 	pthread_mutex_init(&mutex_ready, NULL);
 	pthread_mutex_init(&mutex_blocked, NULL);
 	pthread_mutex_init(&mutex_exec, NULL);
 	pthread_mutex_init(&mutex_exit, NULL);


 }


//..................................... DESTRUCCIONES............................................................................



// SEMAFOROS


 void destruir_semaforos(void){

 	pthread_mutex_destroy(&mutex_new);
 	pthread_mutex_destroy(&mutex_ready);
 	pthread_mutex_destroy(&mutex_blocked);
 	pthread_mutex_destroy(&mutex_exec);
 	pthread_mutex_destroy(&mutex_exit);
 	pthread_mutex_destroy(&mutex_suspended_blocked);
 	pthread_mutex_destroy(&mutex_suspended_ready);


 }


// LISTAS

 void destruir_listas(void){

 	destruirListaYElementos(colaNew);
 	destruirListaYElementos(colaReady);
 	destruirListaYElementos(colaExec);
 	destruirListaYElementos(colaBlocked);
 	destruirListaYElementos(colaExit);

 }

 void destruirListaYElementos(t_list* unaLista){
     list_clean_and_destroy_elements(unaLista, free);
     list_destroy(unaLista);
 }

