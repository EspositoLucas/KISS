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

// list_add(colaNew, proceso);
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

// 	pcb proceso  proceso = list_remove(colaNew);
// 	log_info(logger, "[NEW] Se saca el  proceso de ID: %d de la cola de NEW",  proceso-> id_proceso);

// 	pthread_mutex_unlock(&mutexNew);

// 	return  proceso;
// }

// void agregarAReady(pcb proceso){

//    int grado_multiprogramacion  = config_valores.grado_multiprogramacion ;
//    int tamanio_ready = list_size(colaReady);
// 	proceso->tiempoEspera = 0;
// //	sem_wait(&multiprogramacion); Lo sacamos de aca para usarlo en el contexto en el que se llama a la funcion, porque no siempre que se agrega a ready, se toca la multiprogramacion
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

// 		if(list_size(colaReadySuspended) != 0){

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



// void avisarAMemoriaInicializar(pcb proceso){
// 	//CREAR CONEXION
//     int socket_cliente;
//     socket_cliente= crear_conexion(config_valores.ip_ram,config_valores.puerto_ram);
//     enviar_mensaje("Inicializar estructuras Memoria",socket_cliente);
//     proceso.valor_tabla_paginas = recibir_paquete(socket_cliente)  ; // Ver como se recibe el paquete si el valor va a ser un int o una lista y en que posicion
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

//---------------------------------------- Inicializar colas---------------------------------------

// void inicializarColas(){
// 	colaReady = list_create();
// 	listaExec= list_create();
// }


//----------------------------------------Hilos-----------------------------------------------------


// void hiloReady_Exec(){
// 	while (1){
// 		pcb* procesoAEjecutar = obtenerSiguienteReady();

// 		if (procesoAEjecutar != NULL){
// 			list_add(listaExec,procesoAEjecutar);
// 			//enviarPCBaCPU(procesoAEjecutar);
// 		}
// 	}
// }


// ---------------------------------------------------------------------------------------------

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


// -----------------------------------------------------------------------------------------------------------------


// //void planificacionFIFO(){
// //	log_info(logger,"Se inicio la planificacion FIFO");
// //
// //	int ciclosTotales=0;
// //	int grado_multiprogramacion = config_valores.grado_multiprogramacion;
// //	int ejecutando;
// //	int blockIO;
// //	int tamanioReady;
// //	while(1){
// //		//log_info(logger,"Paso un ciclo de cpu");
// //
// //
// //		sem_wait(&planificacionPausada);
// //		sem_post(&planificacionPausada);
// //
// //		sem_wait(&operacionesProcesos);
// //		  ejecutando= list_size(procesosExecute);
// //		  blockIO= list_size(procesosBlockIO);
// //
// //		tamanioReady=list_size(procesosReady);
// //		sem_post(&operacionesProcesos);
// //		if (tamanioReady > 0){
// //			if (ejecutando<grado_multiprogramacion){
// //				int i=0;
// //				while(ejecutando<grado_multiprogramacion && (tamanioReady-i)>0){
// //					sem_wait(&operacionesProcesos);
// //					t_proceso* primerProceso= list_get(procesosReady,0);
// //					primerProceso->estado= "Exec";
// //					list_remove(procesosReady,0);
// //					sem_post(&primerProceso->semaforoProceso);
// //					enviarPcbACpu(primerProceso->id,"E");
// //					list_add(procesosExecute,primerProceso);//POR AHORA EL PANIF ES EL UNICO QUE MANEJA ESTA LISTA
// //					sem_post(&operacionesProcesos);
// //					i++;
// //					ejecutando++;
// //					log_info(logger,"Se pasaron algunos procesos de ready a Exec");
// //				}
// //			}
// //
// //		}
// //		if(ejecutando>0){
// //			for(int i=0;i<ejecutando;i++){
// //				sem_wait(&operacionesProcesos);
// //				proceso* procesoEjecutando= list_get(procesosExecute,i);
// //				int pasos;
// //				int taListo= procesoEjecutando->noTaListo;
// //
// //				sem_post(&operacionesProcesos);
// //				if(taListo==1){
// //
// //					sem_post(&procesoEjecutando->semaforoproceso);
// //					sem_wait(&sincroprocesoPlanif);
// //					 pasos= procesoEjecutando->pasosAdar;
// //				}
// //				else {
// //					sem_wait(&operacionesProcesos);
// //					 pasos= procesoEjecutando->pasosAdar;
// //					sem_post(&operacionesProcesos);
// //				}
// //				log_info(logger,"proceso: %d   x: %d  y: %d  aEjecut: %d ejecut: %d PASOS: %d \n",procesoEjecutando->id,procesoEjecutando->pos_x,procesoEjecutando->pos_y,procesoEjecutando->aEjectuar,procesoEjecutando->ejecutado,pasos);
// //
// //				if(pasos >0){
// //					sem_post(&procesoEjecutando->semaforoproceso);
// //					sem_wait(&sincroprocesoPlanif2);
// //
// //				} else {
// //					sem_wait(&operacionesProcesos);
// //					int tocaIO= procesoEjecutando->tocaIO;
// //					int ejecutado= procesoEjecutando->ejecutado;
// //					int aEjecutar= procesoEjecutando->aEjectuar;
// //					int ultimaTarea= procesoEjecutando->ultimaTarea;
// //					sem_post(&operacionesProcesos);
// //					if(tocaIO==1){ //NECESITO UN SEMAFORO O ALGO PARA ESTAR 100% SEGURO DE QUE VA A ESTAR SINCRONIZ CON EL proceso
// //					sem_wait(&operacionesProcesos);
// //					list_remove(procesosExecute,i);
// //					ejecutando--;
// //					i--;
// //					procesoEjecutando->estado='B';
// //					enviarPcbACpu(procesoEjecutando->id,procesoEjecutando->id_patota,"B");
// //					procesoEjecutando->tocaIO=0;     // TODO ESTO SERIA SUPONIENDO QUE EL IO ES 1 SOLO CICLO
// //					procesoEjecutando->ejecutado++;
// //					list_add(procesosBlockIO,procesoEjecutando);
// //					log_info(logger,"Se agrego un proceso a la lista de bloqueados");
// //					sem_post(&procesoEjecutando->semaforoproceso);
// //					sem_post(&operacionesProcesos);
// //					 // VER SI DSPS DE 1 DE IO SE DEBE DESBLOQUEAR
// //				}else if(ejecutado < aEjecutar){
// //					sem_wait(&operacionesProcesos);
// //
// //					procesoEjecutando->ejecutado++;
// //					if(procesoEjecutando->ejecutado== aEjecutar &&  ultimaTarea==0){
// //
// //
// //					sem_post(&procesoEjecutando->semaforoproceso);
// //					procesoEjecutando->noTaListo=1;
// //
// //				}
// //
// //				else if( procesoEjecutando->ejecutado ==  aEjecutar &&  ultimaTarea==1){
// //
// //					procesoEjecutando->estado='F';
// //					enviarPcbACpu(procesoEjecutando->id,procesoEjecutando->id_patota,"F"); //que lo saque de memoria
// //					list_remove(procesosExecute,i);
// //					ejecutando--;
// //					i--;
// //					list_add(procesosFinalizados,procesoEjecutando);
// //					sem_post(&procesoEjecutando->semaforoproceso);
// //				}
// //					sem_post(&operacionesProcesos);
// //				}
// //				}
// //			}
// //
// //		}
// //		if(blockIO>0){
// //				sem_wait(&operacionesProcesos);
// //				proceso* procesoBlock= list_get(procesosBlockIO,0);
// //				log_info(logger, "procesoBLOCK ID: %d A_EJEC: %d EJEC: %d",procesoBlock->id, procesoBlock->aEjectuar, procesoBlock->ejecutado);
// //				procesoBlock->ejecutado ++;
// //				int ultimaTarea= procesoBlock->ultimaTarea;
// //				if(procesoBlock->ejecutado == procesoBlock->aEjectuar && ultimaTarea==0){
// //					procesoBlock->estado='R';
// //					enviarPcbACpu(procesoBlock->id,"Ready");
// //					procesoBlock->tocaIO=0;
// //					list_remove(procesosBlockIO,0);
// //					list_add(procesosReady,procesoBlock);
// //					sem_post(&procesoBlock->semaforoproceso);
// //					log_info(logger,"Se paso un proceso a Ready");
// //					blockIO--;
// //					procesoBlock->noTaListo=1;
// //				}else if(procesoBlock->ejecutado == procesoBlock->aEjectuar && ultimaTarea==1){
// //					procesoBlock->estado='F';
// //					enviarPcbACpu(procesoBlock->id,"Finalizar");
// //					list_remove(procesosBlockIO,0);
// //					list_add(procesosFinalizados,procesoBlock);
// //					sem_post(&procesoBlock->semaforoproceso);
// //					blockIO--;
// //				}
// //
// //				sem_post(&operacionesProcesos);
// //
// //
// //		}
// //		}else if(var_sabotaje==1){
// //			  faltaPorArreglar= config_valores.dur_sabotaje;
// //			sem_wait(&operacionesProcesos);
// //			globalExcec= list_size(procesosExecute);
// //			globalExcecLoco = globalExcec;
// //			  blockIO= list_size(procesosBlockIO);
// //			  blockIOLoco = blockIO;
// //			  block_sabotaje=list_size(procesosBlock);
// //			  block_sabotajeLoco = block_sabotaje;
// //			  tamanioReady=list_size(procesosReady);
// //			  tamanioReadyLoco = tamanioReady;
// //			  sumaLoca = tamanioReady+globalExcec;
// //			sem_post(&operacionesProcesos);
// //			for(int i=0;i<globalExcec;i++){
// //				proceso* procesoEjecutando=list_get(procesosExecute,i);
// //				procesoEjecutando->estado='B';
// //				enviarPcbACpu(procesoEjecutando->id,procesoEjecutando->id_patota,"B");
// //				list_remove(procesosExecute,i);
// //				globalExcec--;
// //				i--;
// //				list_add(procesosBlock,procesoEjecutando);
// //			}
// //			for(int i=0;i<tamanioReady;i++){
// //				proceso* procesoEjecutando=list_get(procesosReady,i);
// //				procesoEjecutando->estado='B';
// //				enviarPcbACpu(procesoEjecutando->id,procesoEjecutando->id_patota,"B");
// //				list_remove(procesosReady,i);
// //				tamanioReady--;
// //				i--;
// //				list_add(procesosBlock,procesoEjecutando);
// //			}
// //			for(int i=0;i<sumaLoca;i++){
// //				proceso* candidato= list_get(procesosBlock,i);
// //				if(i==0 && candidato->tocaIO!=1){
// //					posicionElegido=i;
// //				}
// //					proceso * aComparar= list_get(procesosBlock,posicionElegido);
// //					int distanciaComparar= pasosAdar(aComparar->pos_x,aComparar->pos_y,sabotaje_x,sabotaje_y);
// //					int distCandidato= pasosAdar(candidato->pos_x,candidato->pos_y,sabotaje_x,sabotaje_y);
// //					if(distanciaComparar>distCandidato){
// //						posicionElegido=i;
// //
// //				}
// //			}
// //			//while(1){}
// //			proceso* elegido= list_get(procesosBlock,posicionElegido);
// //			elegido->elegidoPorElDestino=1;
// //			sem_post(&elegido->semaforoproceso);
// //			list_remove(procesosBlock,posicionElegido);
// //			list_add(bloqueadosEmergencia,elegido); // TODO SI LA POSICION ES 0 O 1 DSPS DEVOLVERLO A EXCEC
// //			var_sabotaje=2;
// //
// //		}else{
// //			proceso* elegido= list_get(bloqueadosEmergencia,0);
// //			sem_wait(&operacionesProcesos);
// //			int pasosRestantes=elegido->pasosAdar;
// //			sem_post(&operacionesProcesos);
// //			if(pasosRestantes>0){
// //				sem_post(&elegido->semaforoproceso);
// //				sem_wait(&sincroprocesoPlanif);
// //			}
// //			else if(pasosRestantes==0 && faltaPorArreglar>0)
// //			faltaPorArreglar--;
// //
// //			//sem_post(&elegido->semaforoproceso);
// //			if(faltaPorArreglar==0){
// //				var_sabotaje=0;
// //				int noEra=0;
// //					proceso* elegido= list_get(bloqueadosEmergencia,0);
// //					sem_post(&elegido->semaforoproceso);
// //					elegido->elegidoPorElDestino=0;
// //					if(posicionElegido<config_valores.multiprogramacion){
// //						noEra=1;
// //						list_remove(bloqueadosEmergencia,0);
// //						elegido->estado='E';
// //						enviarPcbACpu(elegido->id,elegido->id_patota,"E");
// //						list_add(procesosExecute,elegido);
// //						globalExcecLoco--;
// //					}
// //					int procesosBlock=list_size(procesosBlock);
// //					int entro= 0;
// //					int procesosEmergencia = list_size(bloqueadosEmergencia);
// //					int iTotales=0;
// //					int procesos = procesosBlock + procesosEmergencia;
// //					for(int i=0;i<procesos;i++){
// //						if(i<globalExcecLoco){
// //							proceso* proceso=list_get(procesosBlock,i);
// //							list_remove(procesosBlock,i);
// //							procesos--;
// //							i--;
// //							globalExcecLoco--;
// //							proceso->estado='E';
// //							enviarPcbACpu(proceso->id,proceso->id_patota,"E");
// //							list_add(procesosExecute,proceso);
// //						}else{
// //							if(posicionElegido == (iTotales-grado_multiprogramacion+1) && entro==0 && noEra==0){
// //								proceso* proceso=list_get(bloqueadosEmergencia,0);
// //								list_remove(bloqueadosEmergencia,0);
// //								procesos--;
// //							    i--;
// //							    proceso->estado='R';
// //							    entro=1;
// //								enviarPcbACpu(proceso->id,proceso->id_patota,"R");
// //								list_add(procesosReady,proceso);
// //							}else{
// //								proceso* proceso=list_get(procesosBlock,i);
// //								 list_remove(procesosBlock,i);
// //								 procesos--;
// //								 i--;
// //								 proceso->estado='R';
// //								 enviarPcbACpu(proceso->id,proceso->id_patota,"R");
// //								 list_add(procesosReady,proceso);
// //
// //							}
// //						}
// //						iTotales++;
// //					}
// //				}
// //
// //			}
// //
// //		ciclosTotales++;
// //		sleep(config_valores.tiempo_ciclo_cpu);
// //	}
// //
// //}
// //
// //// void planificacionFIFO(){
// //// 	log_info(logger,"Se inicio la planificacion FIFO");
// //
// //// 	int ciclosTotales=0;
// //// 	int grado_multiprogramacion = config_valores.grado_multiprogramacion;
// //// 	int ejecutando;
// //// 	int blockIO;
// //// 	int tamanioReady;
// //// 	int block_sabotaje;
// //// 	int block_sabotajeLoco;
// //// 	int faltaPorArreglar=0;
// //// 	int posicionElegido=0;
// //// 	int globalExcec=0;
// //// 	int globalExcecLoco=0;
// //// 	while(1){
// //// 		//log_info(logger,"Paso un ciclo de cpu");
// //
// //
// //// 		sem_wait(&planificacionPausada);
// //// 		sem_post(&planificacionPausada);
// //// 		if(var_sabotaje==0){
// //
// //// 		sem_wait(&operacionesProcesos);
// //// 		  ejecutando= list_size(procesosExecute);
// //// 		  blockIO= list_size(procesosBlockIO);
// //
// //// 		tamanioReady=list_size(procesosReady);
// //// 		sem_post(&operacionesProcesos);
// //// 		if (tamanioReady > 0){
// //// 			if (ejecutando<grado_multiprogramacion){
// //// 				int i=0;
// //// 				while(ejecutando<grado_multiprogramacion && (tamanioReady-i)>0){
// //// 					sem_wait(&operacionesProcesos);
// //// 					t_proceso* primerProceso= list_get(procesosReady,0);
// //// 					primerProceso->estado= "Exec";
// //// 					list_remove(procesosReady,0);
// //// 					sem_post(&primerProceso->semaforoProceso);
// //// 					enviarPcbACpu(primerProceso->id,"E");
// //// 					list_add(procesosExecute,primerProceso);//POR AHORA EL PANIF ES EL UNICO QUE MANEJA ESTA LISTA
// //// 					sem_post(&operacionesProcesos);
// //// 					i++;
// //// 					ejecutando++;
// //// 					log_info(logger,"Se pasaron algunos procesos de ready a Exec");
// //// 				}
// //// 			}
// //
// //// 		}
// //// 		if(ejecutando>0){
// //// 			for(int i=0;i<ejecutando;i++){
// //// 				sem_wait(&operacionesProcesos);
// //// 				proceso* procesoEjecutando= list_get(procesosExecute,i);
// //// 				int pasos;
// //// 				int taListo= procesoEjecutando->noTaListo;
// //
// //// 				sem_post(&operacionesProcesos);
// //// 				if(taListo==1){
// //
// //// 					sem_post(&procesoEjecutando->semaforoproceso);
// //// 					sem_wait(&sincroprocesoPlanif);
// //// 					 pasos= procesoEjecutando->pasosAdar;
// //// 				}
// //// 				else {
// //// 					sem_wait(&operacionesProcesos);
// //// 					 pasos= procesoEjecutando->pasosAdar;
// //// 					sem_post(&operacionesProcesos);
// //// 				}
// //// 				log_info(logger,"proceso: %d   x: %d  y: %d  aEjecut: %d ejecut: %d PASOS: %d \n",procesoEjecutando->id,procesoEjecutando->pos_x,procesoEjecutando->pos_y,procesoEjecutando->aEjectuar,procesoEjecutando->ejecutado,pasos);
// //
// //// 				if(pasos >0){
// //// 					sem_post(&procesoEjecutando->semaforoproceso);
// //// 					sem_wait(&sincroprocesoPlanif2);
// //
// //// 				} else {
// //// 					sem_wait(&operacionesProcesos);
// //// 					int tocaIO= procesoEjecutando->tocaIO;
// //// 					int ejecutado= procesoEjecutando->ejecutado;
// //// 					int aEjecutar= procesoEjecutando->aEjectuar;
// //// 					int ultimaTarea= procesoEjecutando->ultimaTarea;
// //// 					sem_post(&operacionesProcesos);
// //// 					if(tocaIO==1){ //NECESITO UN SEMAFORO O ALGO PARA ESTAR 100% SEGURO DE QUE VA A ESTAR SINCRONIZ CON EL proceso
// //// 					sem_wait(&operacionesProcesos);
// //// 					list_remove(procesosExecute,i);
// //// 					ejecutando--;
// //// 					i--;
// //// 					procesoEjecutando->estado='B';
// //// 					enviarPcbACpu(procesoEjecutando->id,procesoEjecutando->id_patota,"B");
// //// 					procesoEjecutando->tocaIO=0;     // TODO ESTO SERIA SUPONIENDO QUE EL IO ES 1 SOLO CICLO
// //// 					procesoEjecutando->ejecutado++;
// //// 					list_add(procesosBlockIO,procesoEjecutando);
// //// 					log_info(logger,"Se agrego un proceso a la lista de bloqueados");
// //// 					sem_post(&procesoEjecutando->semaforoproceso);
// //// 					sem_post(&operacionesProcesos);
// //// 					 // VER SI DSPS DE 1 DE IO SE DEBE DESBLOQUEAR
// //// 				}else if(ejecutado < aEjecutar){
// //// 					sem_wait(&operacionesProcesos);
// //
// //// 					procesoEjecutando->ejecutado++;
// //// 					if(procesoEjecutando->ejecutado== aEjecutar &&  ultimaTarea==0){
// //
// //
// //// 					sem_post(&procesoEjecutando->semaforoproceso);
// //// 					procesoEjecutando->noTaListo=1;
// //
// //// 				}
// //
// //// 				else if( procesoEjecutando->ejecutado ==  aEjecutar &&  ultimaTarea==1){
// //
// //// 					procesoEjecutando->estado='F';
// //// 					enviarPcbACpu(procesoEjecutando->id,procesoEjecutando->id_patota,"F"); //que lo saque de memoria
// //// 					list_remove(procesosExecute,i);
// //// 					ejecutando--;
// //// 					i--;
// //// 					list_add(procesosFinalizados,procesoEjecutando);
// //// 					sem_post(&procesoEjecutando->semaforoproceso);
// //// 				}
// //// 					sem_post(&operacionesProcesos);
// //// 				}
// //// 				}
// //// 			}
// //
// //// 		}
// //// 		if(blockIO>0){
// //// 				sem_wait(&operacionesProcesos);
// //// 				proceso* procesoBlock= list_get(procesosBlockIO,0);
// //// 				log_info(logger, "procesoBLOCK ID: %d A_EJEC: %d EJEC: %d",procesoBlock->id, procesoBlock->aEjectuar, procesoBlock->ejecutado);
// //// 				procesoBlock->ejecutado ++;
// //// 				int ultimaTarea= procesoBlock->ultimaTarea;
// //// 				if(procesoBlock->ejecutado == procesoBlock->aEjectuar && ultimaTarea==0){
// //// 					procesoBlock->estado='R';
// //// 					enviarPcbACpu(procesoBlock->id,"Ready");
// //// 					procesoBlock->tocaIO=0;
// //// 					list_remove(procesosBlockIO,0);
// //// 					list_add(procesosReady,procesoBlock);
// //// 					sem_post(&procesoBlock->semaforoproceso);
// //// 					log_info(logger,"Se paso un proceso a Ready");
// //// 					blockIO--;
// //// 					procesoBlock->noTaListo=1;
// //// 				}else if(procesoBlock->ejecutado == procesoBlock->aEjectuar && ultimaTarea==1){
// //// 					procesoBlock->estado='F';
// //// 					enviarPcbACpu(procesoBlock->id,"Finalizar");
// //// 					list_remove(procesosBlockIO,0);
// //// 					list_add(procesosFinalizados,procesoBlock);
// //// 					sem_post(&procesoBlock->semaforoproceso);
// //// 					blockIO--;
// //// 				}
// //
// //// 				sem_post(&operacionesProcesos);
// //
// //
// //// 		}
// //// 		}else if(var_sabotaje==1){
// //// 			  faltaPorArreglar= config_valores.dur_sabotaje;
// //// 			sem_wait(&operacionesProcesos);
// //// 			globalExcec= list_size(procesosExecute);
// //// 			globalExcecLoco = globalExcec;
// //// 			  blockIO= list_size(procesosBlockIO);
// //// 			  blockIOLoco = blockIO;
// //// 			  block_sabotaje=list_size(procesosBlock);
// //// 			  block_sabotajeLoco = block_sabotaje;
// //// 			  tamanioReady=list_size(procesosReady);
// //// 			  tamanioReadyLoco = tamanioReady;
// //// 			  sumaLoca = tamanioReady+globalExcec;
// //// 			sem_post(&operacionesProcesos);
// //// 			for(int i=0;i<globalExcec;i++){
// //// 				proceso* procesoEjecutando=list_get(procesosExecute,i);
// //// 				procesoEjecutando->estado='B';
// //// 				enviarPcbACpu(procesoEjecutando->id,procesoEjecutando->id_patota,"B");
// //// 				list_remove(procesosExecute,i);
// //// 				globalExcec--;
// //// 				i--;
// //// 				list_add(procesosBlock,procesoEjecutando);
// //// 			}
// //// 			for(int i=0;i<tamanioReady;i++){
// //// 				proceso* procesoEjecutando=list_get(procesosReady,i);
// //// 				procesoEjecutando->estado='B';
// //// 				enviarPcbACpu(procesoEjecutando->id,procesoEjecutando->id_patota,"B");
// //// 				list_remove(procesosReady,i);
// //// 				tamanioReady--;
// //// 				i--;
// //// 				list_add(procesosBlock,procesoEjecutando);
// //// 			}
// //// 			for(int i=0;i<sumaLoca;i++){
// //// 				proceso* candidato= list_get(procesosBlock,i);
// //// 				if(i==0 && candidato->tocaIO!=1){
// //// 					posicionElegido=i;
// //// 				}
// //// 					proceso * aComparar= list_get(procesosBlock,posicionElegido);
// //// 					int distanciaComparar= pasosAdar(aComparar->pos_x,aComparar->pos_y,sabotaje_x,sabotaje_y);
// //// 					int distCandidato= pasosAdar(candidato->pos_x,candidato->pos_y,sabotaje_x,sabotaje_y);
// //// 					if(distanciaComparar>distCandidato){
// //// 						posicionElegido=i;
// //
// //// 				}
// //// 			}
// //// 			//while(1){}
// //// 			proceso* elegido= list_get(procesosBlock,posicionElegido);
// //// 			elegido->elegidoPorElDestino=1;
// //// 			sem_post(&elegido->semaforoproceso);
// //// 			list_remove(procesosBlock,posicionElegido);
// //// 			list_add(bloqueadosEmergencia,elegido); // TODO SI LA POSICION ES 0 O 1 DSPS DEVOLVERLO A EXCEC
// //// 			var_sabotaje=2;
// //
// //// 		}else{
// //// 			proceso* elegido= list_get(bloqueadosEmergencia,0);
// //// 			sem_wait(&operacionesProcesos);
// //// 			int pasosRestantes=elegido->pasosAdar;
// //// 			sem_post(&operacionesProcesos);
// //// 			if(pasosRestantes>0){
// //// 				sem_post(&elegido->semaforoproceso);
// //// 				sem_wait(&sincroprocesoPlanif);
// //// 			}
// //// 			else if(pasosRestantes==0 && faltaPorArreglar>0)
// //// 			faltaPorArreglar--;
// //
// //// 			//sem_post(&elegido->semaforoproceso);
// //// 			if(faltaPorArreglar==0){
// //// 				var_sabotaje=0;
// //// 				int noEra=0;
// //// 					proceso* elegido= list_get(bloqueadosEmergencia,0);
// //// 					sem_post(&elegido->semaforoproceso);
// //// 					elegido->elegidoPorElDestino=0;
// //// 					if(posicionElegido<config_valores.multiprogramacion){
// //// 						noEra=1;
// //// 						list_remove(bloqueadosEmergencia,0);
// //// 						elegido->estado='E';
// //// 						enviarPcbACpu(elegido->id,elegido->id_patota,"E");
// //// 						list_add(procesosExecute,elegido);
// //// 						globalExcecLoco--;
// //// 					}
// //// 					int procesosBlock=list_size(procesosBlock);
// //// 					int entro= 0;
// //// 					int procesosEmergencia = list_size(bloqueadosEmergencia);
// //// 					int iTotales=0;
// //// 					int procesos = procesosBlock + procesosEmergencia;
// //// 					for(int i=0;i<procesos;i++){
// //// 						if(i<globalExcecLoco){
// //// 							proceso* proceso=list_get(procesosBlock,i);
// //// 							list_remove(procesosBlock,i);
// //// 							procesos--;
// //// 							i--;
// //// 							globalExcecLoco--;
// //// 							proceso->estado='E';
// //// 							enviarPcbACpu(proceso->id,proceso->id_patota,"E");
// //// 							list_add(procesosExecute,proceso);
// //// 						}else{
// //// 							if(posicionElegido == (iTotales-grado_multiprogramacion+1) && entro==0 && noEra==0){
// //// 								proceso* proceso=list_get(bloqueadosEmergencia,0);
// //// 								list_remove(bloqueadosEmergencia,0);
// //// 								procesos--;
// //// 							    i--;
// //// 							    proceso->estado='R';
// //// 							    entro=1;
// //// 								enviarPcbACpu(proceso->id,proceso->id_patota,"R");
// //// 								list_add(procesosReady,proceso);
// //// 							}else{
// //// 								proceso* proceso=list_get(procesosBlock,i);
// //// 								 list_remove(procesosBlock,i);
// //// 								 procesos--;
// //// 								 i--;
// //// 								 proceso->estado='R';
// //// 								 enviarPcbACpu(proceso->id,proceso->id_patota,"R");
// //// 								 list_add(procesosReady,proceso);
// //
// //// 							}
// //// 						}
// //// 						iTotales++;
// //// 					}
// //// 				}
// //
// //// 			}
// //
// //// 		ciclosTotales++;
// //// 		sleep(config_valores.tiempo_ciclo_cpu);
// //// 	}
// //
// //// }
// //
// //


// void agregarAReady(t_pcb proceso){

// 	time_t a = time(NULL);
// 	proceso->tiempoEspera = 0;
// 	//sem_wait(&multiprogramacion); Lo sacamos de aca para usarlo en el contexto en el que se llama a la funcion, porque no siempre que se agrega a ready, se toca la multiprogramacion
// 	pthread_mutex_lock(&mutexReady);

// 	proceso->suspendido = false;
// 	list_add(colaReady, proceso);
// 	log_info(logger, "[READY] Entra el proceso de PID: %d a la cola.", proceso->procesoPID);

// 	pthread_mutex_unlock(&mutexReady);
// 	sem_post(&contadorReady);
// 	//sem_post(&contadorProcesosEnMemoria); Lo sacamos de aca para usarlo en el contexto en el que se llama a la funcion, porque no siempre que se agrega a ready, se toca la multiprogramacion
// }

// void agregarABlock(t_pcb proceso){		//ver semaforos

// 	sem_wait(&contadorExe);

// usleep(tiempo_maximo)
// if(sigueen en la cola de bloqueo){
//         supedner()
// }

// 	bool tienenMismoPID(void* elemento){

// 		if(proceso->procesoPID == ((t_pcb *) elemento)->procesoPID)
// 			return true;
// 		else
// 			return false;
// 	}

// 	list_remove_by_condition(listaExe, tienenMismoPID);

// 	pthread_mutex_lock(&mutexBlock

// 	list_add(listaBlock, proceso);
// 	log_info(logger, "[BLOCK] Entra el proceso de PID: %d a la cola.", proceso->procesoPID);

// 	pthread_mutex_unlock(&mutexBlock);
// 	sem_post(&multiprocesamiento);
// 	sem_post(&contadorBlock);

// 	sem_post(&analizarSuspension);
// 	sem_wait(&suspensionFinalizada);
// }

// void sacarDeBlock(t_pcb proceso){

// 	sem_wait(&contadorBlock);

// 	bool tienenMismoPID(void* elemento){

// 		if(proceso->procesoPID == ((t_pcb *) elemento)->procesoPID)
// 			return true;
// 		else
// 			return false;
// 	}

// 	pthread_mutex_lock(&mutexBlock);

// 	list_remove_by_condition(listaBlock, tienenMismoPID);
// 	log_info(logger, "[BLOCK] Sale el proceso de PID: %d de la cola.", proceso->procesoPID);

// 	pthread_mutex_unlock(&mutexBlock);
// }








//................................. MEDIANO PLAZO.........................................................................................

// void sacarDeBlocked(pcb* proceso){ 

// 	sem_wait(&contadorBlock);

// 	bool tienenMismoPID(void* elemento){

// 		if(carpincho->carpinchoPID == ((pcb_carpincho *) elemento)->carpinchoPID)
// 			return true;
// 		else
// 			return false;
// 	}

// 	pthread_mutex_lock(&mutexBlock);

// 	list_remove_by_condition(listaBlock, tienenMismoPID);
// 	log_info(logger, "[BLOCK] Sale el carpincho de PID: %d de la cola.", carpincho->carpinchoPID);

// 	pthread_mutex_unlock(&mutexBlock);
// }

// void agregarABlockSuspended(pcb* proceso){ // Para que entre en supension

// 	pthread_mutex_lock(&mutexBlockedSuspended);

// 	if(proceso->tiempo_bloqueado ==)
// 	proceso->suspendido = true;
// 	list_add(listaBlockSuspended, carpincho);

// 	log_info(logger, "[BLOCKED-SUSPENDED] Ingresa el proceso de PID: %d a la cola.", carpincho->carpinchoPID);

// 	pthread_mutex_unlock(&mutexBlockSuspended);


// }



