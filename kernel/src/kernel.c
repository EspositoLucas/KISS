#include "kernel.h"

//.................................. INICIO_KERNEL.............................................................................................


int main(void)
{
    

    cargar_configuracion("/home/utnso/shared/TP/tp-2022-1c-Ubunteam/kernel/Default/kernel.config");

    kernel_logger_info = log_create("kernel.log", "Servidor Kernel", 1, LOG_LEVEL_INFO);

    log_info(kernel_logger_info, "Archivo de configuracion cargada ");


    //conexion cpu

//    socket_dispatch = crear_conexion(config_valores_kernel.ip_cpu, config_valores_kernel.puerto_cpu_dispatch);
//    socket_interrupt = crear_conexion(config_valores_kernel.ip_cpu, config_valores_kernel.puerto_cpu_interrupt);

    printf("Conectado a cpu \n");

   // conexion memoria

 //   socket_memoria = crear_conexion(config_valores_kernel.ip_memoria, config_valores_kernel.puerto_memoria);

    server_fd = iniciar_servidor(config_valores_kernel.ip_kernel,config_valores_kernel.puerto_escucha);

    printf("Servidor creado \n");

    inciar_planificacion();

    printf("Planificacion iniciada\n");

    log_info(kernel_logger_info, "Kernel listo para recibir al modulo cliente");


    while(atender_clientes_kernel(server_fd));

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
      config_valores_kernel.puerto_cpu_dispatch = config_get_string_value(config, "PUERTO_CPU_DISPATCH");
      config_valores_kernel.puerto_cpu_interrupt = config_get_string_value(config, "PUERTO_CPU_INTERRUPT");
      config_valores_kernel.ip_kernel = config_get_string_value(config, "IP_KERNEL");
      config_valores_kernel.puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
      config_valores_kernel.estimacion_inicial = config_get_int_value(config, "GRADO_MULTIPROGRAMACION");
      config_valores_kernel.grado_multiprogramacion = config_get_int_value(config, "ESTIMACION_INICIAL");
      config_valores_kernel.tiempo_maximo_bloqueado = config_get_int_value(config, "TIEMPO_MAXIMO_BLOQUEADO");
      config_valores_kernel.alfa = config_get_double_value(config, "ALFA");

}





  //----------------------------------DESERIALIZAR INSTRUCCIONES CONSOLA ----------------------------------

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
  	consola->tamanio_proceso = *(uint32_t *)list_remove(datos, 0);
  	consola->instrucciones = deserializar_instrucciones(datos, list_size(datos));
  	return consola;
}



  //---------------------------------------MANEJO CLIENTES - CONEXIONES -----------------------


// manejar conexion con codigo de operacion de tipo int

  void manejar_conexion(int socket_cliente){

	int codigo_operacion = recibir_operacion_nuevo(socket_cliente);

	  	switch (codigo_operacion) {
	  	case MENSAJE:
	  		recibir_mensaje(socket_cliente,kernel_logger_info);
	  		break;
	  	case PAQUETE_CONSOLA:
	  		log_info(kernel_logger_info, "Me llego el tamanio y las instrucciones\n");
	  		consola = deserializar_consola(socket_cliente);
	  		printf("Consola deserializada, entro a armar pcb\n");
	  		proceso* proceso = malloc(sizeof(proceso)) ;
	  		proceso->pcb = malloc(sizeof(pcb));
	  		proceso->pcb = crear_estructura_pcb(consola);
	  		proceso->socket = socket_cliente;
	  		printf("PCB armada -> Lo meto en new y arrancamos con la planificacion\n");
	  		agregarANewPcb(proceso);
	  		break;
	  	case PAQUETE:
			log_info(kernel_logger_info, "Me llego el paquete:\n");
	  		break;

	      default:
	          log_warning(kernel_logger_info, "Operacion desconocida");
	          break;
	  	}
  }

  void chequear_lista_pcbs(t_list*lista){
      for (int i= 0 ; i < list_size(lista) ;i++){
          proceso* proceso = list_get(lista, i);
          printf("PCB ID: %d\n",proceso->pcb->id_proceso);
      }
  }

  //

int atender_clientes_kernel(int socket_servidor){

	int socket_cliente = esperar_cliente(socket_servidor); // se conecta el cliente

		if(socket_cliente != -1) {
			pthread_t hilo_cliente;
			pthread_create(&hilo_cliente, NULL, (void*) manejar_conexion, (void *)socket_cliente); // creo el hilo con la funcion manejar conexion a la que le paso el socket del cliente y sigo en la otra funcion
			pthread_detach(hilo_cliente);
			return 1;
		}


	return 0;
}

