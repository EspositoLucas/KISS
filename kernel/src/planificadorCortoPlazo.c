//#include "planificadorCortoPlazo.h"
//
//
//
//void enviarPcbACpu(int id,char* estado){
//	//CREAR CONEXION
//		int socket_cliente;
//		socket_cliente= crear_conexion(config_valores.ip_cpu,config_valores.puerto_cpu_dispatch);
//	//CREAR PAQUETE
//		t_paquete* nuevoPaquete = crear_paquete_con_codigo_op(CAMBIAR_ESTADO);
//	//AGREGAR A PAQUETE
//		agregar_entero_a_paquete(nuevoPaquete, id);
//		agregar_a_paquete(nuevoPaquete,estado,(strlen(estado) + 1));
//
//	//ENVIAR PAQUETE
//
//		enviar_paquete(nuevoPaquete, socket_cliente);
//
//	// BORRAR PAQUETE
//	eliminar_paquete(nuevoPaquete);
//
//	int codCpu = recibir_operacion(socket_cliente);
//
//	if(codCpu == OK){
//		log_info(logger, "Recibi Pcb OK");
//	}
//
//	// LIBERAR CONEXION
//	liberar_conexion(socket_cliente);
//
//}
//
//void planificacionFIFO(){
//	log_info(logger,"Se inicio la planificacion FIFO");
//
//	int ciclosTotales=0;
//	int grado_multiprogramacion = config_valores.grado_multiprogramacion;
//	int ejecutando;
//	int blockIO;
//	int tamanioReady;
//	while(1){
//		//log_info(logger,"Paso un ciclo de cpu");
//
//
//		sem_wait(&planificacionPausada);
//		sem_post(&planificacionPausada);
//
//		sem_wait(&operacionesProcesos);
//		  ejecutando= list_size(procesosExecute);
//		  blockIO= list_size(procesosBlockIO);
//
//		tamanioReady=list_size(procesosReady);
//		sem_post(&operacionesProcesos);
//		if (tamanioReady > 0){
//			if (ejecutando<grado_multiprogramacion){
//				int i=0;
//				while(ejecutando<grado_multiprogramacion && (tamanioReady-i)>0){
//					sem_wait(&operacionesProcesos);
//					t_proceso* primerProceso= list_get(procesosReady,0);
//					primerProceso->estado= "Exec";
//					list_remove(procesosReady,0);
//					sem_post(&primerProceso->semaforoProceso);
//					enviarPcbACpu(primerProceso->id,"E");
//					list_add(procesosExecute,primerProceso);//POR AHORA EL PANIF ES EL UNICO QUE MANEJA ESTA LISTA
//					sem_post(&operacionesProcesos);
//					i++;
//					ejecutando++;
//					log_info(logger,"Se pasaron algunos procesos de ready a Exec");
//				}
//			}
//
//		}
//		if(ejecutando>0){
//			for(int i=0;i<ejecutando;i++){
//				sem_wait(&operacionesProcesos);
//				proceso* procesoEjecutando= list_get(procesosExecute,i);
//				int pasos;
//				int taListo= procesoEjecutando->noTaListo;
//
//				sem_post(&operacionesProcesos);
//				if(taListo==1){
//
//					sem_post(&procesoEjecutando->semaforoproceso);
//					sem_wait(&sincroprocesoPlanif);
//					 pasos= procesoEjecutando->pasosAdar;
//				}
//				else {
//					sem_wait(&operacionesProcesos);
//					 pasos= procesoEjecutando->pasosAdar;
//					sem_post(&operacionesProcesos);
//				}
//				log_info(logger,"proceso: %d   x: %d  y: %d  aEjecut: %d ejecut: %d PASOS: %d \n",procesoEjecutando->id,procesoEjecutando->pos_x,procesoEjecutando->pos_y,procesoEjecutando->aEjectuar,procesoEjecutando->ejecutado,pasos);
//
//				if(pasos >0){
//					sem_post(&procesoEjecutando->semaforoproceso);
//					sem_wait(&sincroprocesoPlanif2);
//
//				} else {
//					sem_wait(&operacionesProcesos);
//					int tocaIO= procesoEjecutando->tocaIO;
//					int ejecutado= procesoEjecutando->ejecutado;
//					int aEjecutar= procesoEjecutando->aEjectuar;
//					int ultimaTarea= procesoEjecutando->ultimaTarea;
//					sem_post(&operacionesProcesos);
//					if(tocaIO==1){ //NECESITO UN SEMAFORO O ALGO PARA ESTAR 100% SEGURO DE QUE VA A ESTAR SINCRONIZ CON EL proceso
//					sem_wait(&operacionesProcesos);
//					list_remove(procesosExecute,i);
//					ejecutando--;
//					i--;
//					procesoEjecutando->estado='B';
//					enviarPcbACpu(procesoEjecutando->id,procesoEjecutando->id_patota,"B");
//					procesoEjecutando->tocaIO=0;     // TODO ESTO SERIA SUPONIENDO QUE EL IO ES 1 SOLO CICLO
//					procesoEjecutando->ejecutado++;
//					list_add(procesosBlockIO,procesoEjecutando);
//					log_info(logger,"Se agrego un proceso a la lista de bloqueados");
//					sem_post(&procesoEjecutando->semaforoproceso);
//					sem_post(&operacionesProcesos);
//					 // VER SI DSPS DE 1 DE IO SE DEBE DESBLOQUEAR
//				}else if(ejecutado < aEjecutar){
//					sem_wait(&operacionesProcesos);
//
//					procesoEjecutando->ejecutado++;
//					if(procesoEjecutando->ejecutado== aEjecutar &&  ultimaTarea==0){
//
//
//					sem_post(&procesoEjecutando->semaforoproceso);
//					procesoEjecutando->noTaListo=1;
//
//				}
//
//				else if( procesoEjecutando->ejecutado ==  aEjecutar &&  ultimaTarea==1){
//
//					procesoEjecutando->estado='F';
//					enviarPcbACpu(procesoEjecutando->id,procesoEjecutando->id_patota,"F"); //que lo saque de memoria
//					list_remove(procesosExecute,i);
//					ejecutando--;
//					i--;
//					list_add(procesosFinalizados,procesoEjecutando);
//					sem_post(&procesoEjecutando->semaforoproceso);
//				}
//					sem_post(&operacionesProcesos);
//				}
//				}
//			}
//
//		}
//		if(blockIO>0){
//				sem_wait(&operacionesProcesos);
//				proceso* procesoBlock= list_get(procesosBlockIO,0);
//				log_info(logger, "procesoBLOCK ID: %d A_EJEC: %d EJEC: %d",procesoBlock->id, procesoBlock->aEjectuar, procesoBlock->ejecutado);
//				procesoBlock->ejecutado ++;
//				int ultimaTarea= procesoBlock->ultimaTarea;
//				if(procesoBlock->ejecutado == procesoBlock->aEjectuar && ultimaTarea==0){
//					procesoBlock->estado='R';
//					enviarPcbACpu(procesoBlock->id,"Ready");
//					procesoBlock->tocaIO=0;
//					list_remove(procesosBlockIO,0);
//					list_add(procesosReady,procesoBlock);
//					sem_post(&procesoBlock->semaforoproceso);
//					log_info(logger,"Se paso un proceso a Ready");
//					blockIO--;
//					procesoBlock->noTaListo=1;
//				}else if(procesoBlock->ejecutado == procesoBlock->aEjectuar && ultimaTarea==1){
//					procesoBlock->estado='F';
//					enviarPcbACpu(procesoBlock->id,"Finalizar");
//					list_remove(procesosBlockIO,0);
//					list_add(procesosFinalizados,procesoBlock);
//					sem_post(&procesoBlock->semaforoproceso);
//					blockIO--;
//				}
//
//				sem_post(&operacionesProcesos);
//
//
//		}
//		}else if(var_sabotaje==1){
//			  faltaPorArreglar= config_valores.dur_sabotaje;
//			sem_wait(&operacionesProcesos);
//			globalExcec= list_size(procesosExecute);
//			globalExcecLoco = globalExcec;
//			  blockIO= list_size(procesosBlockIO);
//			  blockIOLoco = blockIO;
//			  block_sabotaje=list_size(procesosBlock);
//			  block_sabotajeLoco = block_sabotaje;
//			  tamanioReady=list_size(procesosReady);
//			  tamanioReadyLoco = tamanioReady;
//			  sumaLoca = tamanioReady+globalExcec;
//			sem_post(&operacionesProcesos);
//			for(int i=0;i<globalExcec;i++){
//				proceso* procesoEjecutando=list_get(procesosExecute,i);
//				procesoEjecutando->estado='B';
//				enviarPcbACpu(procesoEjecutando->id,procesoEjecutando->id_patota,"B");
//				list_remove(procesosExecute,i);
//				globalExcec--;
//				i--;
//				list_add(procesosBlock,procesoEjecutando);
//			}
//			for(int i=0;i<tamanioReady;i++){
//				proceso* procesoEjecutando=list_get(procesosReady,i);
//				procesoEjecutando->estado='B';
//				enviarPcbACpu(procesoEjecutando->id,procesoEjecutando->id_patota,"B");
//				list_remove(procesosReady,i);
//				tamanioReady--;
//				i--;
//				list_add(procesosBlock,procesoEjecutando);
//			}
//			for(int i=0;i<sumaLoca;i++){
//				proceso* candidato= list_get(procesosBlock,i);
//				if(i==0 && candidato->tocaIO!=1){
//					posicionElegido=i;
//				}
//					proceso * aComparar= list_get(procesosBlock,posicionElegido);
//					int distanciaComparar= pasosAdar(aComparar->pos_x,aComparar->pos_y,sabotaje_x,sabotaje_y);
//					int distCandidato= pasosAdar(candidato->pos_x,candidato->pos_y,sabotaje_x,sabotaje_y);
//					if(distanciaComparar>distCandidato){
//						posicionElegido=i;
//
//				}
//			}
//			//while(1){}
//			proceso* elegido= list_get(procesosBlock,posicionElegido);
//			elegido->elegidoPorElDestino=1;
//			sem_post(&elegido->semaforoproceso);
//			list_remove(procesosBlock,posicionElegido);
//			list_add(bloqueadosEmergencia,elegido); // TODO SI LA POSICION ES 0 O 1 DSPS DEVOLVERLO A EXCEC
//			var_sabotaje=2;
//
//		}else{
//			proceso* elegido= list_get(bloqueadosEmergencia,0);
//			sem_wait(&operacionesProcesos);
//			int pasosRestantes=elegido->pasosAdar;
//			sem_post(&operacionesProcesos);
//			if(pasosRestantes>0){
//				sem_post(&elegido->semaforoproceso);
//				sem_wait(&sincroprocesoPlanif);
//			}
//			else if(pasosRestantes==0 && faltaPorArreglar>0)
//			faltaPorArreglar--;
//
//			//sem_post(&elegido->semaforoproceso);
//			if(faltaPorArreglar==0){
//				var_sabotaje=0;
//				int noEra=0;
//					proceso* elegido= list_get(bloqueadosEmergencia,0);
//					sem_post(&elegido->semaforoproceso);
//					elegido->elegidoPorElDestino=0;
//					if(posicionElegido<config_valores.multiprogramacion){
//						noEra=1;
//						list_remove(bloqueadosEmergencia,0);
//						elegido->estado='E';
//						enviarPcbACpu(elegido->id,elegido->id_patota,"E");
//						list_add(procesosExecute,elegido);
//						globalExcecLoco--;
//					}
//					int procesosBlock=list_size(procesosBlock);
//					int entro= 0;
//					int procesosEmergencia = list_size(bloqueadosEmergencia);
//					int iTotales=0;
//					int procesos = procesosBlock + procesosEmergencia;
//					for(int i=0;i<procesos;i++){
//						if(i<globalExcecLoco){
//							proceso* proceso=list_get(procesosBlock,i);
//							list_remove(procesosBlock,i);
//							procesos--;
//							i--;
//							globalExcecLoco--;
//							proceso->estado='E';
//							enviarPcbACpu(proceso->id,proceso->id_patota,"E");
//							list_add(procesosExecute,proceso);
//						}else{
//							if(posicionElegido == (iTotales-grado_multiprogramacion+1) && entro==0 && noEra==0){
//								proceso* proceso=list_get(bloqueadosEmergencia,0);
//								list_remove(bloqueadosEmergencia,0);
//								procesos--;
//							    i--;
//							    proceso->estado='R';
//							    entro=1;
//								enviarPcbACpu(proceso->id,proceso->id_patota,"R");
//								list_add(procesosReady,proceso);
//							}else{
//								proceso* proceso=list_get(procesosBlock,i);
//								 list_remove(procesosBlock,i);
//								 procesos--;
//								 i--;
//								 proceso->estado='R';
//								 enviarPcbACpu(proceso->id,proceso->id_patota,"R");
//								 list_add(procesosReady,proceso);
//
//							}
//						}
//						iTotales++;
//					}
//				}
//
//			}
//
//		ciclosTotales++;
//		sleep(config_valores.tiempo_ciclo_cpu);
//	}
//
//}
//
//// void planificacionFIFO(){
//// 	log_info(logger,"Se inicio la planificacion FIFO");
//
//// 	int ciclosTotales=0;
//// 	int grado_multiprogramacion = config_valores.grado_multiprogramacion;
//// 	int ejecutando;
//// 	int blockIO;
//// 	int tamanioReady;
//// 	int block_sabotaje;
//// 	int block_sabotajeLoco;
//// 	int faltaPorArreglar=0;
//// 	int posicionElegido=0;
//// 	int globalExcec=0;
//// 	int globalExcecLoco=0;
//// 	while(1){
//// 		//log_info(logger,"Paso un ciclo de cpu");
//
//
//// 		sem_wait(&planificacionPausada);
//// 		sem_post(&planificacionPausada);
//// 		if(var_sabotaje==0){
//
//// 		sem_wait(&operacionesProcesos);
//// 		  ejecutando= list_size(procesosExecute);
//// 		  blockIO= list_size(procesosBlockIO);
//
//// 		tamanioReady=list_size(procesosReady);
//// 		sem_post(&operacionesProcesos);
//// 		if (tamanioReady > 0){
//// 			if (ejecutando<grado_multiprogramacion){
//// 				int i=0;
//// 				while(ejecutando<grado_multiprogramacion && (tamanioReady-i)>0){
//// 					sem_wait(&operacionesProcesos);
//// 					t_proceso* primerProceso= list_get(procesosReady,0);
//// 					primerProceso->estado= "Exec";
//// 					list_remove(procesosReady,0);
//// 					sem_post(&primerProceso->semaforoProceso);
//// 					enviarPcbACpu(primerProceso->id,"E");
//// 					list_add(procesosExecute,primerProceso);//POR AHORA EL PANIF ES EL UNICO QUE MANEJA ESTA LISTA
//// 					sem_post(&operacionesProcesos);
//// 					i++;
//// 					ejecutando++;
//// 					log_info(logger,"Se pasaron algunos procesos de ready a Exec");
//// 				}
//// 			}
//
//// 		}
//// 		if(ejecutando>0){
//// 			for(int i=0;i<ejecutando;i++){
//// 				sem_wait(&operacionesProcesos);
//// 				proceso* procesoEjecutando= list_get(procesosExecute,i);
//// 				int pasos;
//// 				int taListo= procesoEjecutando->noTaListo;
//
//// 				sem_post(&operacionesProcesos);
//// 				if(taListo==1){
//
//// 					sem_post(&procesoEjecutando->semaforoproceso);
//// 					sem_wait(&sincroprocesoPlanif);
//// 					 pasos= procesoEjecutando->pasosAdar;
//// 				}
//// 				else {
//// 					sem_wait(&operacionesProcesos);
//// 					 pasos= procesoEjecutando->pasosAdar;
//// 					sem_post(&operacionesProcesos);
//// 				}
//// 				log_info(logger,"proceso: %d   x: %d  y: %d  aEjecut: %d ejecut: %d PASOS: %d \n",procesoEjecutando->id,procesoEjecutando->pos_x,procesoEjecutando->pos_y,procesoEjecutando->aEjectuar,procesoEjecutando->ejecutado,pasos);
//
//// 				if(pasos >0){
//// 					sem_post(&procesoEjecutando->semaforoproceso);
//// 					sem_wait(&sincroprocesoPlanif2);
//
//// 				} else {
//// 					sem_wait(&operacionesProcesos);
//// 					int tocaIO= procesoEjecutando->tocaIO;
//// 					int ejecutado= procesoEjecutando->ejecutado;
//// 					int aEjecutar= procesoEjecutando->aEjectuar;
//// 					int ultimaTarea= procesoEjecutando->ultimaTarea;
//// 					sem_post(&operacionesProcesos);
//// 					if(tocaIO==1){ //NECESITO UN SEMAFORO O ALGO PARA ESTAR 100% SEGURO DE QUE VA A ESTAR SINCRONIZ CON EL proceso
//// 					sem_wait(&operacionesProcesos);
//// 					list_remove(procesosExecute,i);
//// 					ejecutando--;
//// 					i--;
//// 					procesoEjecutando->estado='B';
//// 					enviarPcbACpu(procesoEjecutando->id,procesoEjecutando->id_patota,"B");
//// 					procesoEjecutando->tocaIO=0;     // TODO ESTO SERIA SUPONIENDO QUE EL IO ES 1 SOLO CICLO
//// 					procesoEjecutando->ejecutado++;
//// 					list_add(procesosBlockIO,procesoEjecutando);
//// 					log_info(logger,"Se agrego un proceso a la lista de bloqueados");
//// 					sem_post(&procesoEjecutando->semaforoproceso);
//// 					sem_post(&operacionesProcesos);
//// 					 // VER SI DSPS DE 1 DE IO SE DEBE DESBLOQUEAR
//// 				}else if(ejecutado < aEjecutar){
//// 					sem_wait(&operacionesProcesos);
//
//// 					procesoEjecutando->ejecutado++;
//// 					if(procesoEjecutando->ejecutado== aEjecutar &&  ultimaTarea==0){
//
//
//// 					sem_post(&procesoEjecutando->semaforoproceso);
//// 					procesoEjecutando->noTaListo=1;
//
//// 				}
//
//// 				else if( procesoEjecutando->ejecutado ==  aEjecutar &&  ultimaTarea==1){
//
//// 					procesoEjecutando->estado='F';
//// 					enviarPcbACpu(procesoEjecutando->id,procesoEjecutando->id_patota,"F"); //que lo saque de memoria
//// 					list_remove(procesosExecute,i);
//// 					ejecutando--;
//// 					i--;
//// 					list_add(procesosFinalizados,procesoEjecutando);
//// 					sem_post(&procesoEjecutando->semaforoproceso);
//// 				}
//// 					sem_post(&operacionesProcesos);
//// 				}
//// 				}
//// 			}
//
//// 		}
//// 		if(blockIO>0){
//// 				sem_wait(&operacionesProcesos);
//// 				proceso* procesoBlock= list_get(procesosBlockIO,0);
//// 				log_info(logger, "procesoBLOCK ID: %d A_EJEC: %d EJEC: %d",procesoBlock->id, procesoBlock->aEjectuar, procesoBlock->ejecutado);
//// 				procesoBlock->ejecutado ++;
//// 				int ultimaTarea= procesoBlock->ultimaTarea;
//// 				if(procesoBlock->ejecutado == procesoBlock->aEjectuar && ultimaTarea==0){
//// 					procesoBlock->estado='R';
//// 					enviarPcbACpu(procesoBlock->id,"Ready");
//// 					procesoBlock->tocaIO=0;
//// 					list_remove(procesosBlockIO,0);
//// 					list_add(procesosReady,procesoBlock);
//// 					sem_post(&procesoBlock->semaforoproceso);
//// 					log_info(logger,"Se paso un proceso a Ready");
//// 					blockIO--;
//// 					procesoBlock->noTaListo=1;
//// 				}else if(procesoBlock->ejecutado == procesoBlock->aEjectuar && ultimaTarea==1){
//// 					procesoBlock->estado='F';
//// 					enviarPcbACpu(procesoBlock->id,"Finalizar");
//// 					list_remove(procesosBlockIO,0);
//// 					list_add(procesosFinalizados,procesoBlock);
//// 					sem_post(&procesoBlock->semaforoproceso);
//// 					blockIO--;
//// 				}
//
//// 				sem_post(&operacionesProcesos);
//
//
//// 		}
//// 		}else if(var_sabotaje==1){
//// 			  faltaPorArreglar= config_valores.dur_sabotaje;
//// 			sem_wait(&operacionesProcesos);
//// 			globalExcec= list_size(procesosExecute);
//// 			globalExcecLoco = globalExcec;
//// 			  blockIO= list_size(procesosBlockIO);
//// 			  blockIOLoco = blockIO;
//// 			  block_sabotaje=list_size(procesosBlock);
//// 			  block_sabotajeLoco = block_sabotaje;
//// 			  tamanioReady=list_size(procesosReady);
//// 			  tamanioReadyLoco = tamanioReady;
//// 			  sumaLoca = tamanioReady+globalExcec;
//// 			sem_post(&operacionesProcesos);
//// 			for(int i=0;i<globalExcec;i++){
//// 				proceso* procesoEjecutando=list_get(procesosExecute,i);
//// 				procesoEjecutando->estado='B';
//// 				enviarPcbACpu(procesoEjecutando->id,procesoEjecutando->id_patota,"B");
//// 				list_remove(procesosExecute,i);
//// 				globalExcec--;
//// 				i--;
//// 				list_add(procesosBlock,procesoEjecutando);
//// 			}
//// 			for(int i=0;i<tamanioReady;i++){
//// 				proceso* procesoEjecutando=list_get(procesosReady,i);
//// 				procesoEjecutando->estado='B';
//// 				enviarPcbACpu(procesoEjecutando->id,procesoEjecutando->id_patota,"B");
//// 				list_remove(procesosReady,i);
//// 				tamanioReady--;
//// 				i--;
//// 				list_add(procesosBlock,procesoEjecutando);
//// 			}
//// 			for(int i=0;i<sumaLoca;i++){
//// 				proceso* candidato= list_get(procesosBlock,i);
//// 				if(i==0 && candidato->tocaIO!=1){
//// 					posicionElegido=i;
//// 				}
//// 					proceso * aComparar= list_get(procesosBlock,posicionElegido);
//// 					int distanciaComparar= pasosAdar(aComparar->pos_x,aComparar->pos_y,sabotaje_x,sabotaje_y);
//// 					int distCandidato= pasosAdar(candidato->pos_x,candidato->pos_y,sabotaje_x,sabotaje_y);
//// 					if(distanciaComparar>distCandidato){
//// 						posicionElegido=i;
//
//// 				}
//// 			}
//// 			//while(1){}
//// 			proceso* elegido= list_get(procesosBlock,posicionElegido);
//// 			elegido->elegidoPorElDestino=1;
//// 			sem_post(&elegido->semaforoproceso);
//// 			list_remove(procesosBlock,posicionElegido);
//// 			list_add(bloqueadosEmergencia,elegido); // TODO SI LA POSICION ES 0 O 1 DSPS DEVOLVERLO A EXCEC
//// 			var_sabotaje=2;
//
//// 		}else{
//// 			proceso* elegido= list_get(bloqueadosEmergencia,0);
//// 			sem_wait(&operacionesProcesos);
//// 			int pasosRestantes=elegido->pasosAdar;
//// 			sem_post(&operacionesProcesos);
//// 			if(pasosRestantes>0){
//// 				sem_post(&elegido->semaforoproceso);
//// 				sem_wait(&sincroprocesoPlanif);
//// 			}
//// 			else if(pasosRestantes==0 && faltaPorArreglar>0)
//// 			faltaPorArreglar--;
//
//// 			//sem_post(&elegido->semaforoproceso);
//// 			if(faltaPorArreglar==0){
//// 				var_sabotaje=0;
//// 				int noEra=0;
//// 					proceso* elegido= list_get(bloqueadosEmergencia,0);
//// 					sem_post(&elegido->semaforoproceso);
//// 					elegido->elegidoPorElDestino=0;
//// 					if(posicionElegido<config_valores.multiprogramacion){
//// 						noEra=1;
//// 						list_remove(bloqueadosEmergencia,0);
//// 						elegido->estado='E';
//// 						enviarPcbACpu(elegido->id,elegido->id_patota,"E");
//// 						list_add(procesosExecute,elegido);
//// 						globalExcecLoco--;
//// 					}
//// 					int procesosBlock=list_size(procesosBlock);
//// 					int entro= 0;
//// 					int procesosEmergencia = list_size(bloqueadosEmergencia);
//// 					int iTotales=0;
//// 					int procesos = procesosBlock + procesosEmergencia;
//// 					for(int i=0;i<procesos;i++){
//// 						if(i<globalExcecLoco){
//// 							proceso* proceso=list_get(procesosBlock,i);
//// 							list_remove(procesosBlock,i);
//// 							procesos--;
//// 							i--;
//// 							globalExcecLoco--;
//// 							proceso->estado='E';
//// 							enviarPcbACpu(proceso->id,proceso->id_patota,"E");
//// 							list_add(procesosExecute,proceso);
//// 						}else{
//// 							if(posicionElegido == (iTotales-grado_multiprogramacion+1) && entro==0 && noEra==0){
//// 								proceso* proceso=list_get(bloqueadosEmergencia,0);
//// 								list_remove(bloqueadosEmergencia,0);
//// 								procesos--;
//// 							    i--;
//// 							    proceso->estado='R';
//// 							    entro=1;
//// 								enviarPcbACpu(proceso->id,proceso->id_patota,"R");
//// 								list_add(procesosReady,proceso);
//// 							}else{
//// 								proceso* proceso=list_get(procesosBlock,i);
//// 								 list_remove(procesosBlock,i);
//// 								 procesos--;
//// 								 i--;
//// 								 proceso->estado='R';
//// 								 enviarPcbACpu(proceso->id,proceso->id_patota,"R");
//// 								 list_add(procesosReady,proceso);
//
//// 							}
//// 						}
//// 						iTotales++;
//// 					}
//// 				}
//
//// 			}
//
//// 		ciclosTotales++;
//// 		sleep(config_valores.tiempo_ciclo_cpu);
//// 	}
//
//// }
//
//
