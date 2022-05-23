#include "kernel.h"

int main(void)
{
    logger = log_create("log.log", "Servidor Kernel", 1, LOG_LEVEL_DEBUG);

    int server_fd = iniciar_servidor();
    log_info(logger, "Kernel listo para recibir al modulo cliente");
    int cliente_fd = esperar_cliente(server_fd);
    t_buffer * buffer ;

//    t_list *lista;
    pcb* pcb = malloc(sizeof(pcb));

    while (1)
    {
        int cod_op = recibir_operacion(cliente_fd);
        switch (cod_op)
        {
        case MENSAJE:
            recibir_mensaje(cliente_fd);
            break;
        // case PAQUETE:
        //     lista = recibir_paquete(cliente_fd);
        //     log_info(logger, "Me llegaron los mensajes:\n");
        //     list_iterate(lista, (void *)iterator);
        //     break;
        case PAQUETE:
            log_info(logger, "Me llegaron las instrucciones:\n");
            buffer =recibir_buffer_instrucciones(cliente_fd);
            pcb = armar_pcb(buffer);
            free(pcb);
            break;
        
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

t_buffer *recibir_buffer_instrucciones(int socket_cliente) // deserializar paquete instrucciones y tamanio proceso
{
	t_buffer* buffer = malloc(sizeof(t_buffer)) ;
    int size;
    void *stream ;

    recv(socket_cliente, &(buffer->tamanio_proceso), sizeof(int), MSG_WAITALL);
    recv(socket_cliente, &size, sizeof(int), MSG_WAITALL);
    stream = malloc(size);
    recv(socket_cliente, stream, size, MSG_WAITALL);

    buffer->stream = stream ;

    free(stream);
    return buffer ;
}

//VERSION CON IF

 pcb *armar_pcb(t_buffer* buffer) // Para deserializar las instrucciones de consola

{
    pcb* proceso_pcb = malloc(sizeof(pcb)) ;
    int indice_split = 0 ;
    char* mensaje_consola = malloc(buffer->stream_size) ; // leido de consola que se envia en el paquete
    buffer = malloc(sizeof(t_buffer)) ;

     // Deserializar los campos del buffer

//    mensaje_consola = malloc(buffer->stream_size);

    memcpy(mensaje_consola,buffer->stream ,buffer->stream_size);
    memcpy(&(proceso_pcb->tamanio_proceso),&(buffer->tamanio_proceso) ,sizeof(int));
    char** split_buffer = string_split(mensaje_consola, "\n");
    proceso_pcb->instrucciones = list_create();
    while (split_buffer[indice_split] != NULL) {

        if(string_contains(split_buffer[indice_split], "NO_OP") ) {
            char** palabras = string_split(split_buffer[indice_split], " ") ;
            int parametro_NO_OP = atoi(palabras[1]);
            for(int i=0; i< parametro_NO_OP  ; i++){
                // printf("NO_OP %d ", parametro_NO_OP);
                instruccion* instruccion_No_op = malloc(sizeof(instruccion));
                instruccion_No_op->codigo = NO_OP ;
                list_add(proceso_pcb->instrucciones,instruccion_No_op) ; // Para agregar a lista a medida quese vaya parseando
                free(instruccion_No_op);
            }
        string_array_destroy(palabras);

    } else if (string_contains(split_buffer[indice_split], "I/O")){
            char** palabras = string_split(split_buffer[indice_split], " ") ;
            int parametro_IO = atoi(palabras[1]);
            // printf("I/O %d ", parametro_IO);
            instruccion* instruccion_IO = malloc(sizeof(instruccion));
            instruccion_IO->codigo = IO ;
            instruccion_IO->parametro1= parametro_IO ;
            list_add(proceso_pcb->instrucciones,instruccion_IO) ;
            string_array_destroy(palabras);
            free(instruccion_IO) ;
    }

    else if (string_contains(split_buffer[indice_split], "READ")){
            char** palabras = string_split(split_buffer[indice_split], " ") ;
            int parametro_READ = atoi(palabras[1]);
            instruccion* instruccion_READ = malloc(sizeof(instruccion));
            instruccion_READ->codigo = READ ;
            instruccion_READ->parametro1= parametro_READ ;
            // printf("READ %d ", parametro_READ);
            list_add(proceso_pcb->instrucciones,instruccion_READ) ;
            string_array_destroy(palabras);
            free(instruccion_READ);

    } else if (string_contains(split_buffer[indice_split], "WRITE")) {
            char** palabras = string_split(split_buffer[indice_split], " ") ;
            int parametro1_WRITE = atoi(palabras[1]);
            int parametro2_WRITE = atoi(palabras[2]);
            instruccion* instruccion_WRITE = malloc(sizeof(instruccion));
            instruccion_WRITE->codigo = WRITE ;
            instruccion_WRITE->parametro1= parametro1_WRITE ;
            instruccion_WRITE->parametro2= parametro2_WRITE ;
            // printf("WRITE %d %d ", parametro1_WRITE,parametro2_WRITE);
            list_add(proceso_pcb->instrucciones,instruccion_WRITE) ;
            string_array_destroy(palabras);
            free(instruccion_WRITE);

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
            free(instruccion_COPY);

    } else if (string_contains(split_buffer[indice_split], "EXIT")){
            instruccion* instruccion_EXIT = malloc(sizeof(instruccion));
            instruccion_EXIT->codigo = EXIT ;
    		// printf("EXIT");
            list_add(proceso_pcb->instrucciones,instruccion_EXIT) ;
            free(instruccion_EXIT);
    }
        indice_split++;
    }
    string_array_destroy(split_buffer);
    list_destroy(proceso_pcb->instrucciones);
    free(mensaje_consola);
    free(buffer);

    return proceso_pcb;
}

// Funcion TP0

void iterator(char *value)
{
    // printf("Valor: %s\n", value);
    log_info(logger, "%s", value);
}

//.................................. PARSEO_CONSOLA.............................................................................................

// t_buffer *recibir_buffer_instrucciones(int socket_cliente) // deserializar paquete instrucciones y tamanio proceso
// {
// 	t_buffer* buffer = malloc(sizeof(t_buffer)) ;
//     int size;
//     void *stream ;

//     recv(socket_cliente, &(buffer->tamanio_proceso), sizeof(int), MSG_WAITALL);
//     recv(socket_cliente, &size, sizeof(int), MSG_WAITALL);
//     stream = malloc(size);
//     recv(socket_cliente, stream, size, MSG_WAITALL);

//     buffer->stream = stream ;

//     free(stream);
//     return buffer ;
// }

//VERSION CON IF

//  pcb *armar_pcb(t_buffer* buffer) // Para deserializar las instrucciones de consola
// {

//     pcb* proceso_pcb = malloc(sizeof(pcb)) ;
//     int indice_split = 0 ;
//     char* mensaje_consola = malloc(buffer->stream_size) ; // leido de consola que se envia en el paquete


//      // Deserializar los campos del buffer

// //    mensaje_consola = malloc(buffer->stream_size);
//     memcpy(mensaje_consola,buffer->stream ,buffer->stream_size);
//     memcpy(&(proceso_pcb->tamanio_proceso),&(buffer->tamanio_proceso) ,sizeof(int));

//     char** split_buffer = string_split(mensaje_consola, "\n");
//     proceso_pcb->instrucciones = list_create();

//     while (split_buffer[indice_split] != NULL) {


//         if(string_contains(split_buffer[indice_split], "NO_OP") ) {
//             char** palabras = string_split(split_buffer[indice_split], " ") ;
//             int parametro_NO_OP = atoi(palabras[1]);
//             for(int i=0; i< parametro_NO_OP  ; i++){
//                 printf("NO_OP %d ", parametro_NO_OP);
//                 list_add(proceso_pcb->instrucciones,string_duplicate(palabras[0])) ; // Para agregar a lista a medida quese vaya parseando
//             }
//         string_array_destroy(palabras);


//     } else if (string_contains(split_buffer[indice_split], "I/O")){
//             char** palabras = string_split(split_buffer[indice_split], " ") ;
//             int parametro_IO = atoi(palabras[1]);
//             printf("I/O %d ", parametro_IO);
//             list_add(proceso_pcb->instrucciones,string_duplicate(palabras[0])) ;
//             string_array_destroy(palabras);
//     }
//     else if (string_contains(split_buffer[indice_split], "READ")){
//             char** palabras = string_split(split_buffer[indice_split], " ") ;
//             int parametro_READ = atoi(palabras[1]);
//             printf("READ %d ", parametro_READ);
//             list_add(proceso_pcb->instrucciones,string_duplicate(palabras[0])) ;
//             string_array_destroy(palabras);

//     } else if (string_contains(split_buffer[indice_split], "WRITE")) {
//             char** palabras = string_split(split_buffer[indice_split], " ") ;
//             int parametro1_WRITE = atoi(palabras[1]);
//             int parametro2_WRITE = atoi(palabras[2]);
//             printf("WRITE %d %d ", parametro1_WRITE,parametro2_WRITE);
//             list_add(proceso_pcb->instrucciones,string_duplicate(palabras[0])) ;
//             string_array_destroy(palabras);


//     } else if (string_contains(split_buffer[indice_split], "COPY")){
//             char** palabras = string_split(split_buffer[indice_split], " ") ; 
//             int parametro1_COPY = atoi(palabras[1]);
//             int parametro2_COPY = atoi(palabras[2]);
//             printf("COPY %d %d ", parametro1_COPY,parametro2_COPY);
//             list_add(proceso_pcb->instrucciones,string_duplicate(palabras[0])) ;
//             string_array_destroy(palabras);


//     } else if (string_contains(split_buffer[indice_split], "EXIT")){
//     		char** palabras = string_split(split_buffer[indice_split], " ") ;
//     		printf("EXIT");
//             list_add(proceso_pcb->instrucciones,string_duplicate(palabras[0])) ;
//             string_array_destroy(palabras);
//     }
//         indice_split++;
//     }    

//     string_array_destroy(split_buffer);
//     list_destroy(proceso_pcb->instrucciones);
//     free(mensaje_consola);
//     free(buffer->stream);

//     return proceso_pcb;
// }


//VERSION CON SWITCH

// pcb *armar_pcb(t_buffer* buffer) // Para deserializar las instrucciones de consola
// {

//     pcb* proceso_pcb = malloc(sizeof(pcb)) ;
//     int indice_split = 0 ;
//     char* mensaje_consola ; // leido de consola que se envia en el paquete
//     int codigo_instrucciones

//     // Deserializar los campos del buffer

//    mensaje_consola = malloc(buffer->stream_size);
//    memcpy(mensaje_consola,buffer->stream ,buffer->stream_size);
//    memcpy(proceso_pcb->tamanio_proceso,&(buffer->tamanio_proceso) ,sizeof(int));
//    char** split_buffer = string_split(mensaje_consola, "\n");
//    char** palabras ;
//    proceso_pcb->instrucciones = list_create();

//    while (split_buffer[indice_split] != NULL) {

//        switch(codigo_instrucciones) {

//            case:NO_OP:
//                palabras = string_split(split_buffer[indice_split], " ") ;
//                int parametro_NO_OP = atoi(palabras[1]);
//                for(int i=0; i< parametro_NO_OP  ; i++){
//                    printf("NO_OP %d ", parametro_NO_OP);
//                    list_add(proceso_pcb->instrucciones,palabras[0]) ; // Para agregar a lista a medida quese vaya parseando
//                    break;
//            }

//            case:IO:
//                int parametro_IO = atoi(palabras[1]);
//                printf("I/O %d ", parametro_IO);
//                list_add(proceso->instrucciones,split_buffer[indice_split]) ;
//                break;

//            case:READ:
//                int parametro_READ = atoi(palabras[1]);
//                printf("READ %d ", parametro_READ);
//                list_add(proceso->instrucciones,split_buffer[indice_split]) ;
//                break;

//            case:WRITE:
//                int parametro1_WRITE = atoi(palabras[1]);
//                int parametro2_WRITE = atoi(palabras[2]);
//                printf("WRITE %d %d ", parametro1_WRITE,parametro2_WRITE);
//                list_add(proceso->instrucciones,split_buffer[indice_split]) ;
//                break;

//            case:COPY:
//                int parametro1_COPY = atoi(palabras[1]);
//                int parametro2_COPY = atoi(palabras[2]);
//                printf("COPY %d %d ", parametro1_COPY,parametro2_COPY);
//                list_add(proceso->instrucciones,split_buffer[indice_split]) ;
//                break;

//            case:EXIT:
//                printf("EXIT");
//                list_add(proceso->instrucciones,split_buffer[indice_split]) ;
//                break;


//        }

//        indice_split++;
//    }

//    string_array_destroy(split_buffer);
//    string_array_destroy(palabras);
//    list_destroy(proceso_pcb->instrucciones);
//    free(mensaje_consola);

//    return proceso_pcb;
// }

//..................................CONFIGURACIONES.......................................................................


// void cargar_configuracion(void) 
// {
// 	t_config* config = config_create("/cfg/kernel.config"); //Leo el archivo de configuracion

// 	if (config == NULL) {
// 		perror("Archivo de configuracion de kernel no encontrado");
// 		return;
// 	}

// 	config_valores.ip_memoria = 			config_get_string_value(config, "IP_MEMORIA");
// 	config_valores.ip_cpu = 		    config_get_string_value(config, "IP_CPU");
// 	config_valores.algoritmo_planificacion = config_get_string_value(config, "ALGORITMO_PLANIFICACION");

// 	config_valores.puerto_memoria =	config_get_int_value(config, 	"PUERTO_MEMORIA");
// 	config_valores.puerto_cpu_dispatch = config_get_int_value(config, 	"PUERTO_CPU_DISPATCH");
// 	config_valores.puerto_cpu_interrupt = config_get_int_value(config, "PUERTO_CPU_INTERRUPT");
// 	config_valores.puerto_escucha = config_get_int_value(config, 	"PUERTO_ESCUCHA");
// 	config_valores.estimacion_inicial = config_get_int_value(config, 	"GRADO_MULTIPROGRAMACION");
//    config_valores.grado_multiprogramacion = config_get_int_value(config, 	"ESTIMACION_INICIAL");
//    config_valores.tiempo_maximo_bloqueado = config_get_int_value(config, 	"TIEMPO_MAXIMO_BLOQUEADO");

//    config_valores.alfa = config_get_double_value(config, 	"ALFA");
// 	//config_destroy(config);
// }


// void iterar_lista(t_list* lista, t_log* logger) {
//    void iterator(char* value) {
// 	printf("Valor: %s\n", value);
// 	log_info(logger,"%s\n", value);
//    }
//    list_iterate(lista, (void*) iterator);
// }


// MANEJO CONEXIONES



// void manejar_conexion(t_conexiones* conexiones)
// {
// 	int socket = conexiones->socket;
// 	int socket_anterior = conexiones->socket_anterior;

// 		int cod_op = recibir_operacion(socket);

//         switch (cod_op)
//         {
//         case MENSAJE:
//             recibir_mensaje(cliente_fd);
//             break;
//         case PAQUETE:
//             lista = recibir_paquete(cliente_fd);
//             log_info(logger, "Me llegaron las instrucciones:\n");
//             list_iterate(lista, (void *)iterator);
//             break;
//         case -1:
//             log_error(logger, "el cliente se desconecto. Terminando servidor");
//             return EXIT_FAILURE;
//         default:
//             log_warning(logger, "Operacion desconocida");
//             break;
//         }
//     }
		
// }

// void manejar_clientes(int server_fd) //Thread para esperar clientes
// {
// 	while(1)
// 	{
// 		t_conexiones conexiones;
// 		conexiones.socket =  esperar_cliente(server_fd);
// 		conexiones.socket_anterior = 0;
// 		//Threads para recepcion / envio de info a clientes
// 		pthread_t t;
// 		pthread_create(&t, NULL, (void*) manejar_conexion, (void*) &conexiones);
// 		pthread_detach(t);
// 		//printf("Thread: %lu\n", t);
// 	}
// }

// int main(void)
// {
// 	int conexion;
// 	procesosReady =      list_create();
// 	procesosNuevos=      list_create();
// 	procesosExecute=     list_create();
// 	procesosBlockIO=     list_create();
// 	procesosBlock=       list_create();
// 	procesosFinalizados= list_create();
// 	puerto_inicial=5800;


// // TODO IMPORTANTE: SI LA PLANIFICACION ES FIFO, PONER QUANTUM -1
// 	cargar_configuracion();
// 	sem_init(&planificacion,0,config_valores.multitarea);
// 	sem_init(&operacionesprocesos,0,1);
// 	sem_init(&planificacionPausada,0,0);
// 	sem_init(&sincroProcesPlanif,0,0);
// 	sem_init(&sincroProcesPlanif2,0,0);
// 	sem_init(&sabotaje,0,1);
// 	prog_andando= 1;
// 	logger = iniciar_logger();

// 	log_info(logger,"Soy un log");

// 	log_info(logger,"lei la ip %s y el puerto %s",config_valores.ip_ram,config_valores.puerto_ram);

// 	//crear conexion
// 	//conexion= crear_conexion(config_valores.ip_ram,config_valores.puerto_ram);
// 	//enviar CLAVE al servidor
// 	//enviar_mensaje("DISCORDIADOR" ,conexion);

// 	//liberar_conexion(conexion);
// 	int server_fd = iniciar_servidor(config_valores.ip_propia,config_valores.puerto_propio);
// 	pthread_t manejo_recepcion;
// 	pthread_create(&manejo_recepcion, NULL, (void*) manejar_clientes, (void*)server_fd);
// 	pthread_detach(manejo_recepcion); //Va a estar corriendo siempre a la espera de una solicitud de apertura de socket
// 	pthread_t manejo_consola;
// 		pthread_create(&manejo_consola, NULL, (void*) manejarConsola, NULL);
// 		pthread_detach(manejo_consola);

// 	while (prog_andando)
// 	{
// 		sleep(1);
// 		/*char* leido = readline(">");
// 		if (string_equals_ignore_case(leido, "exit"))
// 		{
// 			free(leido);
// 			break;
// 		}
// 		free(leido);*/
// 	}

// 	//finalizar_programa(conexion, logger);
// }


// //MANEJO CONEXIONES

// void manejar_conexion(t_conexiones* conexiones)
// {
// 	int socket = conexiones->socket;
// 	int socket_anterior = conexiones->socket_anterior;

// 		int cod_op = recibir_operacion(socket);

//         switch (cod_op)
//         {
//         case MENSAJE:
//             recibir_mensaje(cliente_fd);
//             break;
//         case PAQUETE:
//             lista = recibir_paquete(cliente_fd);
//             log_info(logger, "Me llegaron las instrucciones:\n");
//             list_iterate(lista, (void *)iterator);
//             break;
//         case -1:
//             log_error(logger, "el cliente se desconecto. Terminando servidor");
//             return EXIT_FAILURE;
//         default:
//             log_warning(logger, "Operacion desconocida");
//             break;
//         }
//     }
		
// }
// void iterator(char *value)
// {
//     // printf("Valor: %s\n", value);
//     log_info(logger, "%s", value);
// }

// void manejar_clientes(int server_fd) //Thread para esperar clientes
// {
// 	while(1)
// 	{
// 		t_conexiones conexiones;
// 		conexiones.socket =  esperar_cliente(server_fd);
// 		conexiones.socket_anterior = 0;
// 		//Threads para recepcion / envio de info a clientes
// 		pthread_t t;
// 		pthread_create(&t, NULL, (void*) manejar_conexion, (void*) &conexiones);
// 		pthread_detach(t);
// 		//printf("Thread: %lu\n", t);
// 	}
// }

// int main(void)
// {
// 	int conexion;
// 	procesosReady =      list_create();
// 	procesosNuevos=      list_create();
// 	procesosExecute=     list_create();
// 	procesosBlockIO=     list_create();
// 	procesosBlock=       list_create();
// 	procesosFinalizados= list_create();


// 	cargar_configuracion();
// 	// sem_init(&planificacion,0,config_valores.grado_multiprogramacion);
// 	// sem_init(&operacionesprocesos,0,1);
// 	// sem_init(&planificacionPausada,0,0);
// 	// sem_init(&sincroProcesPlanif,0,0);
// 	// sem_init(&sincroProcesPlanif2,0,0);
// 	prog_andando= 1;
// 	logger = iniciar_logger();

// 	log_info(logger,"Soy el modulo Kernel");

// 	log_info(logger,"lei la ip %s y el puerto %s",config_valores.ip_ram,config_valores.puerto_ram);
//     log_info(logger,"lei la ip %s y el puerto %s",config_valores.cpu,config_valores.cpu);

// 	//crear conexion
// 	//conexion= crear_conexion(config_valores.ip_ram,config_valores.puerto_ram);
// 	//enviar CLAVE al servidor
// 	//enviar_mensaje("DISCORDIADOR" ,conexion);

// 	//liberar_conexion(conexion);
// 	int server_fd = iniciar_servidor();
// 	pthread_t manejo_recepcion;
// 	pthread_create(&manejo_recepcion, NULL, (void*) manejar_clientes, (void*)server_fd);
// 	pthread_detach(manejo_recepcion); //Va a estar corriendo siempre a la espera de una solicitud de apertura de socket
//     iniciar_planificacion();
// 	while (prog_andando)
// 	{
// 		sleep(1);
// 	}

// }

// //CONFIGURACIONES


// void cargar_configuracion(void) 
// {
// 	t_config* config = config_create("/cfg/kernel.config"); //Leo el archivo de configuracion

// 	if (config == NULL) {
// 		perror("Archivo de configuracion de kernel no encontrado");
// 		return;
// 	}

// 	config_valores.ip_memoria = 			config_get_string_value(config, "IP_MEMORIA");
// 	config_valores.ip_cpu = 		    config_get_string_value(config, "IP_CPU");
// 	config_valores.algoritmo_planificacion = config_get_string_value(config, "ALGORITMO_PLANIFICACION");

// 	config_valores.puerto_memoria =	config_get_int_value(config, 	"PUERTO_MEMORIA");
// 	config_valores.puerto_cpu_dispatch = config_get_int_value(config, 	"PUERTO_CPU_DISPATCH");
// 	config_valores.puerto_cpu_interrupt = config_get_int_value(config, "PUERTO_CPU_INTERRUPT");
// 	config_valores.puerto_escucha = config_get_int_value(config, 	"PUERTO_ESCUCHA");
// 	config_valores.estimacion_inicial = config_get_int_value(config, 	"GRADO_MULTIPROGRAMACION");
//    config_valores.grado_multiprogramacion = config_get_int_value(config, 	"ESTIMACION_INICIAL");
//    config_valores.tiempo_maximo_bloqueado = config_get_int_value(config, 	"TIEMPO_MAXIMO_BLOQUEADO");

//    config_valores.alfa = config_get_double_value(config, 	"ALFA");
// 	//config_destroy(config);
// }

// t_log* iniciar_logger(void)
// {
// 	t_log * logger;
// 	if((logger = log_create("kernel.log","KERNEL",1,LOG_LEVEL_INFO)) == NULL){//el segundo item va el nombre que quiera, el 1 representa que la consola esta activa, si no iria 0
// 		printf("no se pudo crear el logger");
// 		exit (1);
// 	}
// 	return logger;


// void finalizar_programa(int conexion, t_log* logger )
// {
// 	if (logger != NULL){
// 		log_destroy(logger);
// 	}

// 	liberar_conexion(conexion);
// }


//..................................... HILOS............................................................................ 

// void inicializar_planificacion(){

//  pthread_create(&hiloQueDesuspende, NULL, (void*)hiloSuspensionAReady,(void*)server_fd);
// 	pthread_create(&hiloMedianoPlazo, NULL, (void*)hiloBlockASuspension, (void*)server_fd);
// 	pthread_detach(hiloQueDesuspende);
// 	pthread_detach(hiloMedianoPlazo);

// 	pthread_create(&hiloNewReady, NULL, (void*)hiloNew_Ready, (void*)server_fd);
// 	pthread_create(&hiloReady_Exec, NULL, (void*)hiloReady_Exe, (void*)server_fd);
// 	pthread_detach(hiloNewReady);
// 	pthread_detach(hiloReady_Exec);

	
// }

// void manejo_recepcion() {

//  int server_fd = iniciar_servidor();
// 	pthread_t manejo_recepcion;
// 	pthread_create(&manejo_recepcion, NULL, (void*) manejar_clientes, (void*)server_fd);
// 	pthread_detach(manejo_recepcion);
// }


