#include "kernel.h"

int main(void)
{
    logger = log_create("log.log", "Servidor Kernel", 1, LOG_LEVEL_DEBUG);

    int server_fd = iniciar_servidor();
    log_info(logger, "Kernel listo para recibir al modulo cliente");
    int cliente_fd = esperar_cliente(server_fd);

    t_list *lista;
    while (1)
    {
        int cod_op = recibir_operacion(cliente_fd);
        switch (cod_op)
        {
        case MENSAJE:
            recibir_mensaje(cliente_fd);
            break;
        case PAQUETE:
            lista = recibir_paquete(cliente_fd);
            log_info(logger, "Me llegaron los mensajes:\n");
            list_iterate(lista, (void *)iterator);
            break;
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

void iterator(char *value)
{
    printf("Valor: %s\n", value);
    log_info(logger, "%s", value);
}

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
// 	//enviar CLAVE al servirdor
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
