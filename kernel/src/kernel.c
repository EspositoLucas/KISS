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
    t_buffer * buffer ;

//    t_list *lista;

    //pcb* pcb = malloc(sizeof(pcb));

    while (1)
    {
        int cod_op = recibir_operacion(cliente_fd);
        switch (cod_op)
        {
        case MENSAJE:
            recibir_mensaje(cliente_fd,logger);
            break;
        case PAQUETE:
            log_info(logger, "Me llego el tamanio del proceso y las instrucciones:\n");
            buffer = recibir_buffer_proceso(cliente_fd);
            pcb* pcb = armar_pcb(buffer);
            // inicializar_planificacion(); // Una vez que se arma el pcb, se incicia la planificacion
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





//.................................. PARSEO_CONSOLA.............................................................................................


//VERSION CON IF

pcb *agregar_instrucciones_pcb(t_buffer* buffer) // Para deserializar las instrucciones de consola

{
    pcb* proceso_pcb = malloc(sizeof(pcb)) ;
    int offset = 0;
    int indice_split = 0 ;
    char* mensaje_consola = malloc(buffer->stream_size - sizeof(int)) ; // leido de consola que se envia en el paquete

    
     // Deserializar los campos del buffer

    memcpy(&(proceso_pcb->tamanio_proceso),buffer->stream ,sizeof(int));
    offset += sizeof(int);
    memcpy(mensaje_consola,buffer->stream ,buffer->stream_size - sizeof(int));
    char** split_buffer = string_split(mensaje_consola, "\n");
    proceso_pcb->instrucciones = list_create(); // aca como seria ahora que instrucciones es un instruccion* y no una t_list?
    while (split_buffer[indice_split] != NULL) {

        if(string_contains(split_buffer[indice_split], "NO_OP") ) {
            char** palabras = string_split(split_buffer[indice_split], " ") ;
            int parametro_NO_OP;
            if(palabras[1]==NULL){parametro_NO_OP=1;}
            else{
            parametro_NO_OP= atoi(palabras[1]);}
            for(int i=0; i< parametro_NO_OP  ; i++){
                instruccion* instruccion_No_op = malloc(sizeof(instruccion));
                instruccion_No_op->codigo = NO_OP ;
                list_add(proceso_pcb->instrucciones,instruccion_No_op) ; // Para agregar a lista a medida quese vaya parseando
            }
        string_array_destroy(palabras);

        } else if (string_contains(split_buffer[indice_split], "I/O")){
            char** palabras = string_split(split_buffer[indice_split], " ") ;
            int parametro_IO = atoi(palabras[1]);
            instruccion* instruccion_IO = malloc(sizeof(instruccion));
            instruccion_IO->codigo = IO ;
            instruccion_IO->parametro1= parametro_IO ;
            list_add(proceso_pcb->instrucciones,instruccion_IO) ;
            string_array_destroy(palabras);

        } else if (string_contains(split_buffer[indice_split], "READ")){
            char** palabras = string_split(split_buffer[indice_split], " ") ;
            int parametro_READ = atoi(palabras[1]);
            instruccion* instruccion_READ = malloc(sizeof(instruccion));
            instruccion_READ->codigo = READ ;
            instruccion_READ->parametro1= parametro_READ ;
            list_add(proceso_pcb->instrucciones,instruccion_READ) ;
            string_array_destroy(palabras);

        } else if (string_contains(split_buffer[indice_split], "WRITE")) {
            char** palabras = string_split(split_buffer[indice_split], " ") ;
            int parametro1_WRITE = atoi(palabras[1]);
            int parametro2_WRITE = atoi(palabras[2]);
            instruccion* instruccion_WRITE = malloc(sizeof(instruccion));
            instruccion_WRITE->codigo = WRITE ;
            instruccion_WRITE->parametro1= parametro1_WRITE ;
            instruccion_WRITE->parametro2= parametro2_WRITE ;
            list_add(proceso_pcb->instrucciones,instruccion_WRITE) ;
            string_array_destroy(palabras);

        } else if (string_contains(split_buffer[indice_split], "COPY")){
            char** palabras = string_split(split_buffer[indice_split], " ") ;
            int parametro1_COPY = atoi(palabras[1]);
            int parametro2_COPY = atoi(palabras[2]);
            instruccion* instruccion_COPY = malloc(sizeof(instruccion));
            instruccion_COPY->codigo = COPY ;
            instruccion_COPY->parametro1= parametro1_COPY ;
            instruccion_COPY->parametro2= parametro2_COPY ;
            printf("COPY %d %d ", parametro1_COPY,parametro2_COPY);
            list_add(proceso_pcb->instrucciones,instruccion_COPY) ;
            string_array_destroy(palabras);

        } else if (string_contains(split_buffer[indice_split], "EXIT")){
            instruccion* instruccion_EXIT = malloc(sizeof(instruccion));
            instruccion_EXIT->codigo = EXIT ;
            list_add(proceso_pcb->instrucciones,instruccion_EXIT) ;
            
        }

    indice_split++;
    }


    string_array_destroy(split_buffer);
    free(mensaje_consola);
    free(buffer);

    return proceso_pcb;
}

//.................................. ARMAR_PCB.............................................................................................

pcb *armar_pcb(t_buffer* buffer) // Para deserializar las instrucciones de consola

{
    //pcb* proceso_pcb = agregar_instrucciones_pcb(buffer) ;
    pcb* proceso_pcb = agregar_instrucciones_pcb(buffer);

// Inicializar campos del pcb

    proceso_pcb->estado_proceso= NUEVO ;
    proceso_pcb->estimacion_rafaga = config_valores_kernel.estimacion_inicial;
    proceso_pcb->rafaga_anterior = 0;
    proceso_pcb->suspendido = 0 ;
    proceso_pcb->tiempo_de_bloqueo = 0;

    return proceso_pcb;
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
