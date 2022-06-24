#include "kernel.h"

//.................................. INICIO_KERNEL.............................................................................................


int main(void)
{
    

     cargar_configuracion("/home/utnso/tp-2022-1c-Ubunteam/kernel/cfg/kernel.config");
     char* ip_kernel = config_valores_kernel.ip_kernel;
     char* puerto_kernel = config_valores_kernel.puerto_escucha;


    logger = log_create("log.log", "Servidor Kernel", 1, LOG_LEVEL_DEBUG);

    int server_fd = iniciar_servidor(ip_kernel,puerto_kernel);
    log_info(logger, "Kernel listo para recibir al modulo cliente");

    //PROBANDO MONOHILO FUNCIONA FENOMENO
//    int socket_cliente = esperar_cliente(server_fd);
//    manejar_conexion(socket_cliente);

    while(atender_clientes1(server_fd));

    //     iniciar_planificacion(void); // Se inician los hilos para la planificacion una vez que se levanto el kernel

//    if(atender_clientes(server_fd, manejo_conexiones) == -1) {
//    		log_error(logger, "Error al escuchar clientes... Finalizando servidor");
//    }

    return EXIT_SUCCESS;
}






//..................................CONFIGURACIONES.......................................................................


void cargar_configuracion(char* path) {

      t_config* config = config_create(path); //Leo el archivo de configuracion

      if (config == NULL) {
          perror("Archivo de configuracion de kernel no encontrado");
          abort();
      }

      config_valores_kernel.ip_memoria = config_get_string_value(config, "IP_MEMORIA");
      config_valores_kernel.ip_cpu = config_get_string_value(config, "IP_CPU");
      config_valores_kernel.algoritmo_planificacion = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
      config_valores_kernel.puerto_memoria =    config_get_int_value(config, "PUERTO_MEMORIA");
      config_valores_kernel.puerto_cpu_dispatch = config_get_int_value(config, "PUERTO_CPU_DISPATCH");
      config_valores_kernel.puerto_cpu_interrupt = config_get_int_value(config, "PUERTO_CPU_INTERRUPT");
      config_valores_kernel.ip_kernel = config_get_string_value(config, "IP_KERNEL");
      config_valores_kernel.puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
      config_valores_kernel.estimacion_inicial = config_get_int_value(config, "GRADO_MULTIPROGRAMACION");
      config_valores_kernel.grado_multiprogramacion = config_get_int_value(config, "ESTIMACION_INICIAL");
      config_valores_kernel.tiempo_maximo_bloqueado = config_get_int_value(config, "TIEMPO_MAXIMO_BLOQUEADO");
      config_valores_kernel.alfa = config_get_double_value(config, "ALFA");

      //config_destroy(config);

  }



//..................................INICIALIZACIONES.......................................................................


//PLANIFICACION


// void iniciar_planificacion(void){
//	iniciar_planificador_largo_plazo();
//	iniciar_planificador_corto_plazo();
//	iniciar_planificador_mediano_plazo();



// }


//.................................. CONEXIONES.............................................................................................


  void manejo_conexiones(t_paquete* paquete ,int socket_kernel)
  {

		while (1)
    {
    	int cod_op = recibir_operacion(socket_kernel);
    	printf("%d ", cod_op);
    	switch (cod_op) {
    	case MENSAJE:
    		recibir_mensaje(socket_kernel,logger);
    		break;
    	case PAQUETE_CONSOLA:
    		log_info(logger, "Me llego el tamanio y las instrucciones\n");
            consola = deserializar_consola(socket_kernel);
            log_info(logger, "PCB listo para armar\n");
            pcb* pcb = crear_estructura_pcb(consola);
            //meter pcb a new
            log_info(logger, "PCB creado\n");
    		break;
    	case PAQUETE:
    		log_info(logger, "Me llego el paquete:\n");
    		break;

        case -1:
            log_error(logger, "Fallo la comunicacion. Abortando");
            break;
        default:
            log_warning(logger, "Operacion desconocida");
            break;
        }
		
 }

  }

  //----------------------------------DESERIALIZAR INSTRUCCIONES CONSOLA ----------------------------------

t_list *deserializar_instrucciones(t_list *datos, uint32_t longitud_datos) {
	t_list *instrucciones = list_create();

  	for(int i = 0; i < longitud_datos; i += 3) {
  		instruccion *instruccion = malloc(sizeof(instruccion));
  		//printf("Op code: %d, Primer param: %d , Segundo param: %d\n",*(codigo_instrucciones *)list_get(datos,i), *(uint32_t *)list_get(datos,i+1), *(uint32_t *)list_get(datos,i+2));
  		instruccion->codigo = *(codigo_instrucciones *)list_get(datos, i);
  		instruccion->parametro1 = *(uint32_t *)list_get(datos, i + 1);
  		instruccion->parametro2 = *(uint32_t *)list_get(datos, i + 2);
  		list_add(instrucciones, instruccion);
  		printf("Instruccion numero [%d]: Cod operacion es: %d, 1erParam: %d, 2doParam: %d\n", i/3, *(codigo_instrucciones *)list_get(datos,i), *(uint32_t *)list_get(datos,i+1), *(uint32_t *)list_get(datos,i+2));
  	}
  	return instrucciones;
}


t_consola *deserializar_consola(int  socket_cliente) {
	printf("Entre a deseralizar consola con el socket de cliente: %d\n", socket_cliente);

//	int tamanio = sizeof(int);
//	void* stream = malloc(tamanio);
//	recv(socket_cliente, stream, tamanio,0);
//
//	int tamanio_proceso;
//
//	memcpy(tamanio_proceso, stream, sizeof(int));

	t_list *datos = recibir_paquete(socket_cliente);

	printf("Tamanio de lista recibida: %d\n", list_size(datos));

//    for(int i=0 ; i<datos->elements_count;i++){
//    	printf("Dato numero '%d': %p \n",i,list_get(datos,i));
//    }


//	void* stream = malloc(tamanio_stream);
//	recv(socket_cliente, stream, tamanio_stream,0);

//  	printf("Cantidad de datos deberia ser igual a 6, es: %d", datos->elements_count);

	printf("Recibi el stream con los datos\n");
  	t_consola *consola = malloc(sizeof(t_consola));
  	consola->tamanio_proceso = *(uint32_t *)list_remove(datos, 0);
//  	printf("Tamanio de proceso en cosola: %d", *(int *) consola->tamanio_proceso);
  	consola->instrucciones = deserializar_instrucciones(datos, list_size(datos));
//  	printf("Datos de consola, cantidad de instr en la lista: %d", consola->instrucciones->elements_count);
//  	list_destroy_and_destroy_elements(datos, free);
  	return consola;
}


  //---------------------------------------VIDEO LA CAJA -----------------------



  void manejar_conexion(int socket_cliente){
	printf("\nAdentro de manejar conexion con el socket cliente: %d\n", socket_cliente);

	op_code codigo_operacion = recibir_operacion(socket_cliente);

	printf("Codigo de operacion: %d\n", codigo_operacion);
	  	switch (codigo_operacion) {
	  	case MENSAJE:
	  		recibir_mensaje(socket_kernel,logger);
	  		break;
	  	case PAQUETE_CONSOLA:
	  		log_info(logger, "Me llego el tamanio y las instrucciones\n");
	  		consola = deserializar_consola(socket_cliente);
	  		pcb* proceso = crear_estructura_pcb(consola);
	  		printf("PCB armada -> Lo meto en new y arrancamos con la planificacion");
	  		//agregarANewPcb(proceso);
	  //        log_info(logger, "PCB listo para armar\n");
	  //        pcb* pcb = crear_estructura_pcb(consola);
	  //        log_info(logger, "PCB creado\n");
	  		break;
	  	case PAQUETE:
			log_info(logger, "Me llego el paquete:\n");
	  		break;

	//      case -1:
	//          log_error(logger, "Fallo la comunicacion. Abortando");
	//          break;
	      default:
	          log_warning(logger, "Operacion desconocida");
	          break;
	  	}
  }



int atender_clientes1(int socket_servidor){

	int socket_cliente = esperar_cliente(socket_servidor); // se conecta el cliente
//	printf("Socket cliente: %d\n", socket_cliente);


	while(true){
		pthread_t hilo_cliente;
//		printf("Creo el hilo\n");
		pthread_create(&hilo_cliente, NULL, (void*) manejar_conexion, (void *)socket_cliente); // creo el hilo con la funcion manejar conexion a la que le paso el socket del cliente y sigo en la otra funcion
		pthread_detach(hilo_cliente);
		return 1;
	}
	return 0;
}
