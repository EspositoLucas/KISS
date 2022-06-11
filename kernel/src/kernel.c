#include "kernel.h"

//.................................. INICIO_KERNEL.............................................................................................

int main(void)
{
    
	// CARGAR CONFIG/SEM/LISTAS

    // cargar_configuracion();
    //manejar_clientes();
    // inicializar_listas();
	// inicializar_semaforos();

    logger = log_create("log.log", "Servidor Kernel", 1, LOG_LEVEL_DEBUG);

    int server_fd = iniciar_servidor(IP_KERNEL,PUERTO_KERNEL);
    log_info(logger, "Kernel listo para recibir al modulo cliente");
    int cliente_fd = esperar_cliente(logger,"kernel",server_fd);
    t_consola * consola ;

//    t_list *lista;

    //pcb* pcb = malloc(sizeof(pcb));

    while (1)
    {
    	int cod_op = recibir_operacion(cliente_fd);
    	printf("%d ", cod_op);
    	switch (cod_op) {
    	case MENSAJE:
    		recibir_mensaje(cliente_fd,logger);
    		break;
    	case PAQUETE_CONSOLA:
    		log_info(logger, "Me llego el tamanio y las instrucciones\n");
            consola = deserializar_consola(cliente_fd);
            log_info(logger, "PCB listo para armar\n");
            pcb* pcb = crear_estructura_pcb(consola);
            log_info(logger, "PCB creado\n");
            // inicializar_planificacion(); // Una vez que se arma el pcb, se incicia la planificacion
    		break;
    	case PAQUETE:
    		log_info(logger, "Me llego el paquete:\n");
    		break;

        // case PCB: // PARA CUANDO SE RECIBA PCB DE CPU O MEMORIA
        // 	pcb* pcb_recibido = recibir_pcb(cliente_fd);
        // 	log_info(logger,"Recibi PCB de Id: %d",pcb_recibido->id_proceso);
        // 	if(pcb_recibido->estado == BLOQUEADO) {
            //time_t tiempo_actual_bloqueo ;
        //    agregarABlocked(pcb* proceso);
        // time(tiempo_actual_bloqueo);
        //   usleep(pcb_recibido->tiempo_bloqueado);
        //      manejo_suspensiones(pcb_recibido) ;
        //     }
        //     break;
        
        // case TABLA_PAGINAS:   // PARA MEMORIA
        //     lista = recibir_paquete(cliente_fd);
        //     log_info(logger, "Me llego el valor de la tabla de paginas:\n");
        //     list_iterate(lista, (void *)iterator);
        //     break;

        case -1:
            log_error(logger, "Fallo la comunicacion. Abortando");
            return EXIT_FAILURE;
        default:
            log_warning(logger, "Operacion desconocida");
            break;
        }
    }
    return EXIT_SUCCESS;
}

// Funcion TP0

//void iterator(char *value)
//{
//    // printf("Valor: %s\n", value);
//    log_info(logger, "%s", value);
//}

//----------------------------------DESERIALIZAR INSTRUCCIONES----------------------------------

t_list *deserializar_instrucciones(t_list *datos, uint32_t longitud_datos) {
	t_list *instrucciones = list_create();

	for(int i = 0; i < longitud_datos; i += 3) {
		instruccion *instruccion = malloc(sizeof(instruccion));
		instruccion->codigo = *(codigo_instrucciones *)list_get(datos, i);
		instruccion->parametro1 = *(uint32_t *)list_get(datos, i + 1);
		instruccion->parametro2 = *(uint32_t *)list_get(datos, i + 2);
		list_add(instrucciones, instruccion);
	}

	return instrucciones;
}

t_consola *deserializar_consola(int  socket_cliente) {
	t_list *datos = recibir_paquete(socket_cliente);
	t_consola *consola = malloc(sizeof(t_consola));

	consola->instrucciones = deserializar_instrucciones(datos, list_size(datos) - 1);
	consola->tamanio_proceso = *(uint32_t *)list_get(datos, list_size(datos) - 1);

	list_destroy_and_destroy_elements(datos, free);
	return consola;
}



//..................................CONFIGURACIONES.......................................................................


//  void cargar_configuracion(void) {

//  	t_config* config = config_create("/cfg/kernel.config"); //Leo el archivo de configuracion

//  	if (config == NULL) {
//  		perror("Archivo de configuracion de kernel no encontrado");
//  		return;
//  	}

//  	config_valores_kernel.ip_memoria = 			config_get_string_value(config, "IP_MEMORIA");
//  	config_valores_kernel.ip_cpu = 		    config_get_string_value(config, "IP_CPU");
//  	config_valores_kernel.algoritmo_planificacion = config_get_string_value(config, "ALGORITMO_PLANIFICACION");

//  	config_valores_kernel.puerto_memoria =	config_get_int_value(config, 	"PUERTO_MEMORIA");
//  	config_valores_kernel.puerto_cpu_dispatch = config_get_int_value(config, 	"PUERTO_CPU_DISPATCH");
//  	config_valores_kernel.puerto_cpu_interrupt = config_get_int_value(config, "PUERTO_CPU_INTERRUPT");
//  	config_valores_kernel.puerto_escucha = config_get_int_value(config, 	"PUERTO_ESCUCHA");
//  	config_valores_kernel.estimacion_inicial = config_get_int_value(config, 	"GRADO_MULTIPROGRAMACION");
//  	config_valores_kernel.grado_multiprogramacion = config_get_int_value(config, 	"ESTIMACION_INICIAL");
//  	config_valores_kernel.tiempo_maximo_bloqueado = config_get_int_value(config, 	"TIEMPO_MAXIMO_BLOQUEADO");
//      config_valores_kernel.alfa = config_get_double_value(config, 	"ALFA");
//  	config_destroy(config);



//  }

//..................................INICIALIZACIONES.......................................................................


// LISTAS

// void inicializar_listas(void){

// 	colaNew =lista_create();
// 	colaReady = list_create();
// 	colaExec = list_create();
// 	colaBlocked = list_create();
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



//PLANIFICACION


// void inicializar_planificacion(void){



// 	pthread_create(&hiloSuspendedAReady, NULL, (void*)hiloSuspensionAReady, NULL);
// 	pthread_create(&hiloMedianoPlazo, NULL, (void*)hiloBlockedASuspension, NULL);
// 	pthread_detach(hiloQueDesuspende);
// 	pthread_detach(hiloMedianoPlazo);


// 	pthread_create(&hiloNewAReady, NULL, (void*)hiloNew_Ready, NULL);
// 	pthread_create(&hiloReadyAExec, NULL, (void*)hiloReady_Exec, NULL);
// 	pthread_detach(hiloNewReady);
// 	pthread_detach(hiloReady_Exec);


// }


//.................................. CONEXIONES.............................................................................................


//  void manejar_conexion(t_buffer * buffer ,pcb* pcb)
//  {


// //    t_list *lista;

//     pcb = malloc(sizeof(pcb));

//     while (1)
//     {
//         int cod_op = recibir_operacion(cliente_fd);
//         switch (cod_op)
//         {
//         case MENSAJE:
//             recibir_mensaje(cliente_fd,logger);
//             break;
//         case PAQUETE:
//             log_info(logger, "Me llego el tamanio del proceso y las instrucciones:\n");
//             buffer = recibir_buffer_proceso(cliente_fd);
//             pcb = armar_pcb(buffer);
//             inicializar_planificacion(); // Una vez que se arma el pcb, se incicia la planificacion
//             break;
        
//         // case TABLA_PAGINAS:   // PARA MEMORIA
//         //     lista = recibir_paquete(cliente_fd);
//         //     log_info(logger, "Me llego el valor de la tabla de paginas:\n");
//         //     list_iterate(lista, (void *)iterator);
//         //     break;

//         case -1:
//             log_error(logger, "Fallo la comunicacion. Abortando");
//             return EXIT_FAILURE;
//         default:
//             log_warning(logger, "Operacion desconocida");
//             break;
//         }
//     }
		
// }




//..................................... HILOS............................................................................ 


// MANEJO CLIENTE-SERVIDOR


// void manejo_recepcion(int  server_fd) {
// logger = log_create("log.log", "Servidor Kernel", 1, LOG_LEVEL_DEBUG);
//server_fd = iniciar_servidor(IP_KERNEL,PUERTO_KERNEL);
// log_info(logger, "Kernel listo para recibir al modulo cliente");

// 	pthread_t manejo_recepcion;
// 	pthread_create(&manejo_recepcion, NULL, (void*) manejar_clientes, (void*)server_fd);
// 	pthread_detach(manejo_recepcion);
// }


// void manejar_clientes(int server_fd,int cliente_fd) //Thread para esperar clientes
// {
// 	while(1)
// 	{
    	// t_conexiones conexiones;
		// conexiones.socket =  esperar_cliente(server_fd);
		// conexiones.socket_anterior = 0;
		// //Threads para recepcion / envio de info a clientes
		// pthread_t t;
		// pthread_create(&t, NULL, (void*) manejar_conexion, (void*) &conexiones,);
		// pthread_detach(t);
// 	}
// }




//..................................... DESTRUCCIONES............................................................................ 



// SEMAFOROS

// void destruir_semaforos(){

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

// void destruir_listas(){

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
