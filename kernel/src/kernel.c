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


//----------------------------------DESERIALIZAR INSTRUCCIONES CONSOLA ----------------------------------

t_list *deserializar_instrucciones(t_list *datos, uint32_t longitud_datos) {
	t_list *instrucciones = list_create();

	for(int i = 0; i < longitud_datos; i += 5) {
		instruccion *instruccion = malloc(sizeof(instruccion));
		instruccion->codigo = *(codigo_instrucciones *)list_get(datos, i);
		instruccion->parametro1 = *(uint32_t *)list_get(datos, i + 1);
		instruccion->parametro2 = *(uint32_t *)list_get(datos, i + 3);
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

//void eliminar_configuracion(t_config* config) {

//	free(config.ip_memoria);
//	free(config.puerto_memoria);
//	free(config.ip_cpu);
//	free(config.puerto_cpu_dispatch);
//	free(config.puerto_cpu_interrupt);
//	free(config.ip_kernel);
//	free(config.puerto_escucha);
//	free(config.algoritmo_planificacion);
//	free(config);
//}

//..................................INICIALIZACIONES.......................................................................


//PLANIFICACION


// void inicializar_planificacion(void){
//	iniciar_planificador_largo_plazo();
//	iniciar_planificador_corto_plazo();
//	iniciar_planificador_mediano_plazo();



// }


//.................................. CONEXIONES.............................................................................................


//  void manejar_conexion(t_buffer * buffer ,pcb* pcb)
//  {


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
// server_fd = iniciar_servidor(IP_KERNEL,PUERTO_KERNEL);
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





