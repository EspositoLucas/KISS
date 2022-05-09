// #include "planificadorLargoPlazo.h"

// // MANEJO DE ESTADOS PROCESOS

// void agregarANewProceso(pcb proceso) {

// 	pthread_mutex_lock(&mutexNew);

// 	queue_push(colaNew, proceso);
//     proceso.estado = "NEW";
// 	log_info(logger, "[NEW] Entra el proceso de ID: %d a la cola.", proceso->id_proceso);

// 	pthread_mutex_unlock(&mutexNew);

// 	sem_post(&analizarSuspension); // Despierta al planificador de mediano plazo
// 	sem_wait(&suspensionFinalizada); // Espera a que ya se haya hecho, o no, la suspension

// 	sem_post(&contadorNew); // Despierta al planificador de largo plazo
// 	sem_post(&largoPlazo);
// }

// pcb sacarDeNew(){

// 	sem_wait(&contadorNew);
// 	pthread_mutex_lock(&mutexNew);

// 	pcb proceso  proceso = queue_pop(colaNew);
// 	log_info(logger, "[NEW] Se saca el  proceso de ID: %d de la cola de NEW",  proceso-> id_proceso);

// 	pthread_mutex_unlock(&mutexNew);

// 	return  proceso;
// }

// void agregarAReady(pcb proceso){

//    int grado_multiprogramacion  = config_valores.grado_multiprogramacion ;
//    int tamanio_ready = queue_size(colaReady);
// 	proceso->tiempoEspera = 0;
// 	//sem_wait(&multiprogramacion); Lo sacamos de aca para usarlo en el contexto en el que se llama a la funcion, porque no siempre que se agrega a ready, se toca la multiprogramacion
// 	pthread_mutex_lock(&mutexReady);
//    if(tamanio_ready < grado_multiprogramacion ) {
//            proceso->suspendido = false;
//            proceso.estado = "READY";
//            list_add(colaReady,  proceso);
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

// 	pthread_mutex_unlock(&mutexExit);



// }


// // Hilo que maneja pasar los procesos de new a ready	-	CASO FIFO

// void hiloNew_Ready(){

// 	while(1){

// 		sem_wait(&largoPlazo);

// 		if(queue_size(colaReadySuspended) != 0){

// 			sem_post(&medianoPlazo);
// 		}else{

// 			pcb proceso = sacarDeNew();

// 			// proceso->rafagaAnterior = 0;
// 			 proceso->estimacion_rafaga = estimacion_anterior;
// 			//  proceso->estimacion_rafaga= estimacion_actual;	//"estimacio_inicial" va a ser una variable que vamos a obtener del cfg
// 			 proceso->tiempoEspera = 0;

// 			sem_wait(&multiprogramacion);
// 			agregarAPotencialesRetensores( proceso);
// 			agregarAReady( proceso);
// 			sem_post(&contadorProcesosEnMemoria);
// 		}
// 	}
// }



// void avisarAMemoriaInicializar(pcb proceso){
// 	//CREAR CONEXION
//     int socket_cliente;
//     socket_cliente= crear_conexion(config_valores.ip_ram,config_valores.puerto_ram);
//     enviar_mensaje("Inicializar estructuras Memoria",socket_cliente);
//     proceso.valor_tabla_paginas = recibir_paquete(socket_cliente)  ; // Ver comos e recibe el paquete si el valor va a ser un int o una lista y en que posicion
//     log_info(logger, "Obtuve el valor de la tabla de paginas ");

//     // LIBERAR CONEXION
//     liberar_conexion(socket_cliente);

// }
// void avisarAMemoriaLiberar(pcb proceso){
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
//     enviar_mensaje("Finalizar Proceso",socket_cliente);

// 	// LIBERAR CONEXION
// 	liberar_conexion(socket_cliente);

// }

