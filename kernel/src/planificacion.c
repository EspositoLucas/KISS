#include "planificacion.h"

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

//................................. LARGO PLAZO.........................................................................................



// MANEJO DE ESTADOS PROCESOS


// void agregarANewProceso(pcb proceso) {

// 	pthread_mutex_lock(&mutexNew);

// 	   list_add(procesosNew, proceso);
//     proceso->estado = NUEVO;
// 	   log_info(logger, "[NEW] Entra el proceso de ID: %d a la cola.", proceso->id_proceso);

// 	pthread_mutex_unlock(&mutexNew);

// 	sem_post(&analizarSuspension); // Despierta al planificador de mediano plazo
// 	sem_wait(&suspensionFinalizada); // Espera a que ya se haya hecho, o no, la suspension

// 	sem_post(&contadorNew); // Despierta al planificador de largo plazo
// 	sem_post(&largoPlazo);
// }

// pcb sacarDeNew(){

// 	sem_wait(&contadorNew);
// 	pthread_mutex_lock(&mutexNew);

// 	pcb proceso  proceso = list_remove(colaNew);
// 	log_info(logger, "[NEW] Se saca el  proceso de ID: %d de la cola de NEW",  proceso-> id_proceso);

// 	pthread_mutex_unlock(&mutexNew);

// 	return  proceso;
// }

// void agregarAReady(pcb proceso){

//    int grado_multiprogramacion  = config_valores.grado_multiprogramacion ;
//    int tamanio_ready = list_size(colaReady);
// 	pthread_mutex_lock(&mutexReady);
//    if(tamanio_ready < grado_multiprogramacion ) {
//            proceso->suspendido = 0;
//            proceso->estado = LISTO;
//            list_add(colaReady,  proceso);
//            log_info(logger, "[READY] Entra el  proceso de ID: %d a la cola.",  proceso-> proceso_id);

//            pthread_mutex_unlock(&mutexReady);

//            sem_post(&contadorReady);
//		avisarAMemoriaInicializarEstructuras(pcb proceso);
//

//    }

// }
//  void finalizarProceso(pcb proceso){

//  	pthread_mutex_lock(&mutexExit);

//  	list_add(colaExit, proceso);
//     proceso.estado = "EXIT";
// 	log_info(logger, "[EXIT] Finaliza el  proceso de ID: %d", proceso-> proceso_id);
//      if(proceso->instrucciones[proceso->program_counter] == "EXIT") {
//          pthread_mutex_lock(&mutexExit);
// 	    list_add(colaExit, proceso);
//         proceso->estado = "EXIT";
//    log_info(logger, "[EXIT] Finaliza el  proceso de ID: %d", proceso-> proceso_id);

// 	    pthread_mutex_unlock(&mutexExit);
//          avisarAMemoriaLiberarEstructuras(proceso) ;
//          avisarAConsola(proceso) ;
//     }



// }


// // Hilo que maneja pasar los procesos de new a ready	

// void hiloNew_Ready(){

// 	while(1){

// 		sem_wait(&largoPlazo);

// 		if(list_size(colaBlocked) != 0){

// 			sem_post(&medianoPlazo);
// 		}else{

// 			pcb proceso = sacarDeNew();

// 			proceso->rafagaAnterior = 0;
// 			 proceso->estimacion_rafaga = config_valores_kernel.estimacion_inicial;
// 			 proceso->tiempoEspera = 0;
//          sem_wait(&multiprogramacion);
// 			agregarAReady( proceso);
// 			sem_post(&contadorProcesosEnMemoria);
// 		}
// 	}
// }




// AVISOS A MEMORIA - CONSOLA



// void avisarAMemoriaInicializarEstructuras(pcb proceso){
//     int socket_cliente;
//     socket_cliente= crear_conexion(config_valores->ip_ram,config_valores->puerto_ram);
//     enviar_mensaje("Inicializar estructuras Memoria",socket_cliente);
//     proceso->valor_tabla_paginas = recibir_paquete(socket_cliente)  ; // Ver como se recibe el paquete si el valor va a ser un int o una cola y en que posicion
//	   proceso->valor_tabla_paginas = tabla_paginas(socket_cliente);
//	   log_info(logger, "Obtuve el valor de la tabla de paginas ");

//     liberar_conexion(socket_cliente);

//     int tabla_paginas(int socket-cliente) {
//     t_list valores = recibir_paquete(socket_cliente) ;
//     return list_get(valores,0);

// }

// }

// void avisarAMemoriaLiberarEstructuras(pcb proceso){

// 	int socket_cliente;
// 	socket_cliente= crear_conexion(config_valores.ip_ram,config_valores.puerto_ram);
//     enviar_mensaje("Liberar estructuras Memoria",socket_cliente);

// 	liberar_conexion(socket_cliente);

// }

// void avisarAConsola(pcb proceso){
// 	int socket_cliente;
// 	socket_cliente= crear_conexion(IP_KERNEL,PUERTO_KERNEL);
//   enviar_mensaje("Proceso Finalizado",socket_cliente);

// 	liberar_conexion(socket_cliente);

// }




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


// Inicializar colas

// void inicializarColas(){
// 	colaReady = list_create();
// 	colaExec= list_create();
// }


// HILOS


// void hiloReady_Exec(){
// 	while (1){
// 		pcb* procesoAEjecutar = obtenerSiguienteReady();

// 		if (procesoAEjecutar != NULL){
// 			list_add(colaExec,procesoAEjecutar);
// 			//enviarPcbACpu(procesoAEjecutar);
// 		}
// 	}
// }




// TRANSICIONES



// pcb* obtenerSiguienteReady(){
// 	pcb* procesoSeleccionado;

// 	int tamanioReady;
// 	tamanioReady = list_size(colaReady);
// 	int gradoMultiprogramacion;
// 	t_algoritmo_planificacion algoritmo = obtener_algoritmo();
// 	int ejecutando = list_size(colaExec);


// 	if (tamanioReady > 0 && ejecutando < gradoMultiprogramacion){
// 		switch(algoritmo){
// 		case FIFO:
// 			procesoSeleccionado = obtenerSiguienteFIFO();
// 			break;
// 		case SRT:
// 			procesoSeleccionado = obtenerSiguienteSRT();
// 			break;
// 		}
// 	}
// 	return procesoSeleccionado;
// }

// pcb* obtenerSiguienteFIFO(){

// 	log_info(logger,"Inicio la planificacion FIFO");

// 	pcb* procesoSeleccionado = list_remove(colaReady,0);


// 	return procesoSeleccionado;

// }

// pcb* obtenerSiguienteSRT(){
// 	log_info(logger,"Inicio la planificacion SRT");
// 	asignarEstimacionesAProcesos();
// 	pcb* procesoElegido = elegirElDeMenorEstimacion();
// 	return procesoElegido;
// }

// pcb* elegirElDeMenorEstimacion(){
// 	int tamanioReady = list_size(colaReady);
// 	pcb* procesoSeleccionado;
// 	pcb* procesoAux;
// 	int indiceElegido = 0;

// 	float procesoMasCorto;

// 	procesoMasCorto = procesoAux->estimacion_rafaga;

// 	for(int i = 0; i < tamanioReady; i++){
// 		pcb* procesoAux = list_get(colaReady,i);

// 		if(procesoMasCorto > procesoAux->estimacion_rafaga){
// 			procesoMasCorto = procesoAux->estimacion_rafaga;
// 			indiceElegido = i;
// 		}

// 	}

// 	procesoSeleccionado = list_remove(colaReady,indiceElegido);

// 	return procesoSeleccionado;
// }

// void asignarEstimacionesAProcesos(){

// 	int tamanioReady = list_size(colaReady);

	/*for(int i = 0; i < tamanioReady; i++){
		pcb* proceso = list_get(colaReady,i);
		float realAnterior = proceso->instrucciones[(proceso->program_counter)-1]
		proceso->estimacion_rafaga = calculoEstimacionProceso(realAnterior); // le paso a calculoEstimacionProceso la rafaga real anterior
	}*/
//}  revisar bien esta funcion

// float calculoEstimacionProceso(float realAnterior){

// 	float alfa;
// 	float estimacionInicial;

// 	float estimacion_rafaga = alfa * realAnterior + (1 - alfa) * estimacionInicial;

// 	return estimacion_rafaga;
// }


// void agregarABlocked(pcb* proceso){		

// 	sem_wait(&contadorExec);

// 	pthread_mutex_lock(&mutexBlocked);

// 	list_add(listaBlock, proceso);
// 	log_info(logger, "[BLOCKED] Entra el procso de PID: %d a la cola.", proceso->id_proceso);

// 	pthread_mutex_unlock(&mutexBlocked);
// 	sem_post(&contadorBlock);

// 	sem_post(&analizarSuspension);
// 	sem_wait(&suspensionFinalizada);
// }

//................................. MEDIANO PLAZO.........................................................................................


// TRANSICIONES_ ESTADOS


// void sacarDeBlockedASuspendedBlocked(pcb* proceso){   // Se saca un proceso de bloqueado a suspendido

// 	sem_wait(&contadorBlock);

// 	bool suspensionMayorAtiempoMaximo(void* elemento){ 

// 		if(proceso->tiempo_bloqueado > config_valores_kernel->tiempo_maximo_bloqueado)
// 			return true;
// 		else
// 			return false;
// 	}

// 	pthread_mutex_lock(&mutexBlock);

// 	list_remove_by_condition(colaBlocked, suspensionMayorAtiempoMaximo);
// 	log_info(logger, "[BLOCKED] Sale el proceso de PID: %d de la procesos.", proceso->id_proceso);

// 	pthread_mutex_unlock(&mutexBlock);
// agregarAEstadoSuspendedBlocked(pcb* proceso);
// }


// void agregarAEstadoSuspendedBlocked(pcb* proceso){ // Para que entre en supension pero sigue en la cola de bloqueado


	// pthread_mutex_lock(&mutexBlockSuspended);

	// proceso->suspendido = 1;
	// list_add(colaSuspendedBlocked, proceso);

	// proceso->estado = SUPENDIDO_BLOQUEADO ;

	// log_info(logger, "[BLOCK-SUSPENDED] Ingresa el proceso de PID: %d a la cola.", proceso->proceso_id_proceso);

	// pthread_mutex_unlock(&mutexSuspendedBlocked);



// avisarAMemoriaProcesoSuspendido(pcb proceso);

//}


// void avisarAMemoriaProcesoSuspendido(pcb proceso){

// 	int socket_cliente;
// 	socket_cliente= crear_conexion(config_valores.ip_ram,config_valores.puerto_ram);
//  enviar_mensaje("Proceso paso a estado SUPENDED_BLOCKED",socket_cliente);
// 	liberar_conexion(socket_cliente);

// }


// SWITCH ENTRE ESTADOS PARA CUANDO SE RECIBA PCB DE CPU

// switch(estado) {
// 	case : BLOCKED 
// 	// aca iria un hilo ?
// medir cuanto tiempo en la cola  - timestamp
//medir tiempo maximo-bloqueado
// }
//
// usleep(tiempo_maximo_bloqueado)  // proceso ya esta bloqueado y entra en la cola de blocked
// if(sigue en en la cola de bloqueo){
//         supender()
// }


// void suspender( pcb* proceso){
	//sacarDeBlockedASuspendedBlocked(pcb* proceso);

// }
//}

// FUNCION PARA MANEJAR SUSPENSIONES CUANDO UN PCB ESTE BLOQUEADO

// void manejo_suspensiones(pcb* proceso) {
// 	if (pcb->tiempo_de_bloqueo > config_valores_kernel.tiempo_maximo_bloqueado) {
// 		suspender(proceso);
// 	}
// }


// HILOS PARA LA SUSPENSION DE PROCESOS



// void hiloBlockedASuspension(){

// 	while(true){

// 		sem_wait(&analizarSuspension);

// 		if(condiciones_de_suspension()){

// 			sem_wait(&contadorProcesosEnMemoria);

// 			pcb* pcb = list_get(colaBlocked, list_size(colaBlocked) - 1);
// 			sacarDeBlocked(pcb);

// 			agregarASuspendedBlocked(pcb);

// 			sem_post(&multiprogramacion);
// 		}

// 		sem_post(&suspensionFinalizada);
// 	}
// }

// void hiloSuspensionAReady(){

// 	while(1){

// 		sem_wait(&medianoPlazo);

// 		if(list_size(colaSuspendedReady) == 0){

// 			sem_post(&largoPlazo);
// 		}else{

// 		pcb* proceso = sacarDeSuspendedReady();

// 		sem_wait(&multiprogramacion);

// 		agregarAReady(proceso);

// 		sem_post(&contadorProcesosEnMemoria);
// 		}
// 	}
// }


// void hilo_IO () {
// 	sem_wait(&proceso_blocked) ;
// 	usleep(proceso->tiempo_bloqueo);
// 	sem_post(&proceso_termino_IO);

// }
