#include "kernel.h"

//.................................. INICIO_KERNEL.............................................................................................


int main(void)
{
    

     cargar_configuracion("cfg/kernel.config");
     char* ip_kernel = config_valores_kernel.ip_kernel;
     char* puerto_kernel = config_valores_kernel.puerto_escucha;

//     iniciar_planificacion(void);


    logger = log_create("log.log", "Servidor Kernel", 1, LOG_LEVEL_DEBUG);

    int server_fd = iniciar_servidor(ip_kernel,puerto_kernel);
    log_info(logger, "Kernel listo para recibir al modulo cliente");

    if(atender_clientes(server_fd, manejo_conexiones) == -1) {
    		log_error(logger, "Error al escuchar clientes... Finalizando servidor");
    	}




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

  }

//void eliminar_configuracion(t_config* config) {
//
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








