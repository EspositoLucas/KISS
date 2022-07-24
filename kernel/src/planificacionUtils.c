#include "planificacionUtils.h"

//--------------------------------------------------- FUNCIONES PLANIFICACION -------------------------------

void inciar_planificacion(){
	iniciar_planificador_largo_plazo();
	log_info(kernel_logger_info, "Estructuras largo plazo creadas \n");
	iniciar_planificador_corto_plazo();
	log_info(kernel_logger_info, "Estructuras corto plazo creadas \n");
	iniciar_planificador_mediano_plazo();
	log_info(kernel_logger_info, "Estructuras mediano plazo creadas \n");
}



// --------------------------------------------------- FUNCIONES LARGO PLAZO ----------------------------------------------------------

// AVISOS y RECIBOS de MEMORIA - CONSOLA

 void avisarAModulo(int socket_fd,op_code codigo){
	 enviar_datos(socket_fd, &codigo, sizeof(op_code));

 }

 void enviar_pcb_a_memoria(pcb *pcb, int socket_memoria, op_code codigo) {
	 t_paquete* paquete = crear_paquete_con_codigo_de_operacion(codigo);
	 armarPaquete(paquete,pcb);
	 enviar_paquete(paquete, socket_memoria);
	 eliminar_paquete(paquete);
 }


pcb* obtener_entrada_tabla_de_pagina(int socket_fd,pcb* pcb) {
	t_paquete *paquete = crear_paquete_con_codigo_de_operacion(INICIALIZAR_ESTRUCTURAS);
	armarPaquete(paquete,pcb);
	printf("valor socket %d \n",socket_fd);
	enviar_paquete(paquete, socket_fd);
	log_info(kernel_logger_info, "envio a memoria mensaje para inicializar estructuras del proceso \n");
	//printf("tamanio_proceso %"PRIu32"\n",pcb->tamanio_proceso);
	eliminar_paquete(paquete);
	uint32_t valorTP1;
	printf("antes de recibir paquete de memoria  \n");
	recibir_datos(socket_fd,&valorTP1,sizeof(uint32_t));
	pcb->valor_tabla_paginas = valorTP1;
	printf("valor tabla recibido  %"PRIu32" \n",pcb->valor_tabla_paginas);
	log_info(kernel_logger_info, "Recibi paquete PCB de memoria \n ");
	return pcb;

//	while(1){
//		int codigo = recibir_operacion_nuevo(socket_fd);
//		switch(codigo){
//				case PAQUETE:
//					printf("antes de recibir paquete de memoria  \n");
//					t_list* valores = recibir_paquete(socket_fd);
//					log_info(kernel_logger_info, "Recibi paquete PCB de memoria \n ");
//					// ver que no se recibe bien paquete al conectar otra consola
//					printf("valor tabla \n");
//					//pcb->valor_tabla_paginas = *(uint32_t*) list_remove(valores,0);
//					//pcb->valor_tabla_paginas = list_remove(valores,0);
//					pcb->valor_tabla_paginas = *(uint32_t*)list_get(valores,0);
//					printf("valor tabla recibido %d \n",pcb->valor_tabla_paginas);
//					list_destroy(valores);
//
//					log_info(kernel_logger_info, "entrada de tabla de paginas de primer nivel del proceso recibida \n ");
//					return pcb;
//				case -1:
//					            log_error(kernel_logger_info, "Fallo la comunicacion. Abortando \n");
//					            exit(30);
//			 	default:
//			          log_warning(kernel_logger_info, "Operacion desconocida \n");
//			          break;
//
//			  	}
//		}
	}



// op_code esperar_respuesta_memoria(int socket_memoria) {
// 	op_code codigo;
// 	codigo = recibir_operacion_nuevo(socket_memoria);
// 	return codigo;
// }
op_code esperar_respuesta_memoria(int socket_memoria) {
 	op_code codigo;
 	recibir_datos(socket_memoria,&codigo,sizeof(op_code));
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
 		log_info(kernel_logger_info, "El algoritmo de planificacion elegido es FIFO \n");
 	 }

 	    //SFJ SIN DESALOJO
 	 if (strcmp(algoritmo,"SRT") == 0)
 	 {
 		 switcher = SRT;
 		log_info(kernel_logger_info, "El algoritmo de planificacion elegido es SRT \n");
 	 }
 	 return switcher;
}

proceso* obtenerSiguienteReady(){
	proceso* procesoSeleccionado;
	int tamanioReady;
 	tamanioReady = list_size(colaReady);
 	int gradoMultiprogramacion = config_valores_kernel.grado_multiprogramacion;
 	algoritmo algoritmo = obtener_algoritmo();
 	int ejecutando = list_size(colaExec);

 	chequear_lista_pcbs(colaReady);

 	if (tamanioReady > 0 && ejecutando < gradoMultiprogramacion){
 		switch(algoritmo){
 		case FIFO:
 			procesoSeleccionado = obtenerSiguienteFIFO();
 			break;
 		case SRT:
 			procesoSeleccionado = obtenerSiguienteSRT();
 			break;
 		default:break ;
 		}

 	}
// 	printf("proceso a ejecutar: %d\n", procesoSeleccionado->pcb->id_proceso);
 	return procesoSeleccionado;
 }


proceso* obtenerSiguienteFIFO(){

	log_info(kernel_logger_info, "Inicio la planificacion FIFO");
 	chequear_lista_pcbs(colaReady);
 	proceso* procesoSeleccionado = list_remove(colaReady,0);
 	log_info(kernel_logger_info, "PID[%d] sale de READY para plan FIFO \n", procesoSeleccionado->pcb->id_proceso);
	return procesoSeleccionado;

}

proceso* obtenerSiguienteSRT(){

	log_info(kernel_logger_info, "Inicio la planificacion SRT \n");
 	asignarEstimacionesAProcesos();
 	proceso* procesoElegido = elegirElDeMenorEstimacion();
 	log_info(kernel_logger_info, "PID[%d] con menor estimacion sale de READY para plan SRT \n", procesoElegido->pcb->id_proceso);
 	return procesoElegido;
 }

proceso* elegirElDeMenorEstimacion(){

	int tamanioReady = list_size(colaReady);
	proceso* procesoSeleccionado;
	proceso* procesoAux = list_get(colaReady,0);
	int indiceElegido = 0;
	float procesoMasCorto;
	procesoMasCorto = procesoAux->pcb->estimacion_rafaga;
	for(int i = 0; i < tamanioReady; i++){
		proceso* procesoAux = list_get(colaReady,i);
		if(procesoMasCorto > procesoAux->pcb->estimacion_rafaga){
			procesoMasCorto = procesoAux->pcb->estimacion_rafaga;
			indiceElegido = i;
		}
	}

	procesoSeleccionado = list_remove(colaReady,indiceElegido);

	return procesoSeleccionado;
}

void asignarEstimacionesAProcesos(){
 	int tamanioReady = list_size(colaReady);
	for(int i = 0; i < tamanioReady; i++){
		proceso* proceso = list_get(colaReady,i);
		calculoEstimacionProceso(proceso);
	};
}  //revisar bien esta funcion

void calculoEstimacionProceso(proceso *proceso){
	float alfa = config_valores_kernel.alfa;
	float estimacionInicial = config_valores_kernel.estimacion_inicial;
	float realAnterior = proceso->pcb->rafaga_anterior;
	float nuevaEstimacion = alfa * realAnterior + (1 - alfa) * estimacionInicial;
	proceso->pcb->estimacion_rafaga = nuevaEstimacion;
}

void interrumpir_cpu(){
	t_paquete *paqueteAEnviar = crear_paquete_con_codigo_de_operacion(INTERRUPCION);
	enviar_paquete(paqueteAEnviar, socket_interrupt);
	log_info(kernel_logger_info, "se envia mensaje de interrupcion a cpu \n");
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

