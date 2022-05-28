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

// list_add(procesosNew, proceso);
//     proceso->estado = "NEW";
// 	log_info(logger, "[NEW] Entra el proceso de ID: %d a la procesos.", proceso->id_proceso);

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
//    int tamanio_ready = list_size(procesosReady);
// 	pthread_mutex_lock(&mutexReady);
//    if(tamanio_ready < grado_multiprogramacion ) {
//            proceso->suspendido = false;
//            proceso->estado = "READY";
//            list_add(procesosReady,  proceso);
//            log_info(logger, "[READY] Entra el  proceso de ID: %d a la cola.",  proceso-> proceso_id);

//            pthread_mutex_unlock(&mutexReady);
//            sem_post(&contadorReady);

//    }

// }
// void finalizarProceso(pcb proceso){

// 	pthread_mutex_lock(&mutexExit);

// 	list_add(listaExit, proceso);
//     proceso.estado = "EXIT";
// 	log_info(logger, "[EXIT] Finaliza el  proceso de ID: %d", proceso-> proceso_id);

//     if(proceso->instrucciones[proceso->program_counter] == "EXIT") {
//         pthread_mutex_lock(&mutexExit);
// 	    list_add(listaExit, proceso);
//         proceso->estado = "EXIT";
// 	    log_info(logger, "[EXIT] Finaliza el  proceso de ID: %d", proceso-> proceso_id);

// 	    pthread_mutex_unlock(&mutexExit);
        // avisarAMemoriaLiberarEstructuras(proceso) ;
        // avisarAConsola(proceso) ;
//     }



// }


// // Hilo que maneja pasar los procesos de new a ready	-	CASO FIFO

// void hiloNew_Ready(){

// 	while(1){

// 		sem_wait(&largoPlazo);

// 		if(list_size(procesosReadySuspended) != 0){

// 			sem_post(&medianoPlazo);
// 		}else{

// 			pcb proceso = sacarDeNew();

// 			// proceso->rafagaAnterior = 0;
// 			 proceso->estimacion_rafaga = estimacion_anterior;
// 			//  proceso->estimacion_rafaga= estimacion_actual;	//"estimacio_inicial" va a ser una variable que vamos a obtener del cfg
// 			 proceso->tiempoEspera = 0;

// 			sem_wait(&multiprogramacion);
// 			agregarAReady( proceso);
// 			sem_post(&contadorProcesosEnMemoria);
// 		}
// 	}
// }




// AVISOS A MEMORIA - CONSOLA



// void avisarAMemoriaInicializarEstructuras(pcb proceso){
// 	//CREAR CONEXION
//     int socket_cliente;
//     socket_cliente= crear_conexion(config_valores->ip_ram,config_valores->puerto_ram);
//     enviar_mensaje("Inicializar estructuras Memoria",socket_cliente);
//     proceso->valor_tabla_paginas = recibir_paquete(socket_cliente)  ; // Ver como se recibe el paquete si el valor va a ser un int o una lista y en que posicion
//	   proceso->valor_tabla_paginas = tabla_paginas(socket_cliente);
//log_info(logger, "Obtuve el valor de la tabla de paginas ");

//     // LIBERAR CONEXION
//     liberar_conexion(socket_cliente);

//  int tabla_paginas(int socket-cliente) {
//     t_list valores = recibir_paquete(socket_cliente) ;
//     return list_get(valores,0);

// }

// }
// void avisarAMemoriaLiberarEstructuras(pcb proceso){
// 	//CREAR CONEXION

// 	int socket_cliente;
// 	socket_cliente= crear_conexion(config_valores.ip_ram,config_valores.puerto_ram);
//     enviar_mensaje("Liberar estructuras Memoria",socket_cliente);

// 	// LIBERAR CONEXION
// 	liberar_conexion(socket_cliente);

// }

// void avisarAConsola(pcb proceso){
// 	//CREAR CONEXION
// 	int socket_cliente;
// 	socket_cliente= crear_conexion(IP_KERNEL,PUERTO_KERNEL);
//     enviar_mensaje("Proceso Finalizado",socket_cliente);

// 	// LIBERAR CONEXION
// 	liberar_conexion(socket_cliente);

// }




//................................. CORTO PLAZO.........................................................................................


// void enviarPcbACpu(pcb pcb){
// 	//CREAR CONEXION
// 		int socket_cliente;
// 		socket_cliente= crear_conexion(config_valores.ip_cpu,config_valores.puerto_cpu_dispatch);
// 	//CREAR PAQUETE
// 		t_paquete* nuevoPaquete = crear_paquete();
// 	//AGREGAR A PAQUETE
// 		agregar_entero_a_paquete(nuevoPaquete, id);
// 		agregar_a_paquete(nuevoPaquete,estado,(strlen(estado) + 1));

// 	//ENVIAR PAQUETE

// 		enviar_paquete(nuevoPaquete, socket_cliente);

// 	// BORRAR PAQUETE
// 	eliminar_paquete(nuevoPaquete);

// 	int codCpu = recibir_operacion(socket_cliente);

// 	if(codCpu == OK){
// 		log_info(logger, "Recibi Pcb OK");
// 	}

// 	// LIBERAR CONEXION
// 	liberar_conexion(socket_cliente);

// }
// //


// Inicializar colas

// void inicializarColas(){
// 	colaReady = list_create();
// 	listaExec= list_create();
// }


// Hilos


// void hiloReady_Exec(){
// 	while (1){
// 		pcb* procesoAEjecutar = obtenerSiguienteReady();

// 		if (procesoAEjecutar != NULL){
// 			list_add(listaExec,procesoAEjecutar);
// 			//enviarPCBaCPU(procesoAEjecutar);
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
// 	int ejecutando = list_size(listaExec);


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


// void agregarABlock(t_pcb proceso){		//ver semaforos

// 	sem_wait(&contadorExe);



//................................. MEDIANO PLAZO.........................................................................................


// TRANSICIONES_ ESTADOS


// void sacarDeBlocked(pcb* proceso){ 

// 	sem_wait(&contadorBlock);

// 	bool suspensionMayorAtiempoMaximo(void* elemento){

// 		if(carpincho->carpinchoPID == ((pcb_carpincho *) elemento)->carpinchoPID)
// 			return true;
// 		else
// 			return false;
// 	}

// 	pthread_mutex_lock(&mutexBlock);

// 	list_remove_by_condition(listaBlock, suspensionMayorAtiempoMaximo);
// 	log_info(logger, "[BLOCK] Sale el carpincho de PID: %d de la procesos.", carpincho->carpinchoPID);

// 	pthread_mutex_unlock(&mutexBlock);
// }

// void agregarASuspendedBlock(pcb* proceso){ // Para que entre en supension

// 	pthread_mutex_lock(&mutexBlockedSuspended);

// 	proceso->suspendido = true;

// 	list_add(listaBlockSuspended, carpincho);

// void avisarAMemoriaProcesoSuspendido(pcb proceso){

// 	int socket_cliente;
// 	socket_cliente= crear_conexion(config_valores.ip_ram,config_valores.puerto_ram);
//     enviar_mensaje("Proceso paso a estado SUPENDE_BLOCKED",socket_cliente);

// 	liberar_conexion(socket_cliente);

// }


// SWITCH ENTRE ESTADOS

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
// void suspender(){

// }
//}
