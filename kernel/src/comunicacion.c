// #include "comunicacion.h"




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


// // HILOS 

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

