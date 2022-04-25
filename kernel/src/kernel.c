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
            log_info(logger, "Me llegaron las instrucciones:\n");
            list_iterate(lista, (void *)iterator);
            break;
        case -1:
            log_error(logger, "el cliente se desconecto. Terminando servidor");
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

//void cargar_configuracion(void) // configuraciones kernel
//{
//	t_config* config = config_create("/cfg/kernel.config"); //Leo el archivo de configuracion
//
//	if (config == NULL) {
//		perror("Archivo de configuracion de kernel no encontrado");
//		return;
//	}
//
//	config_valores.ip_memoria = 			config_get_string_value(config, "IP_MEMORIA");
//	config_valores.ip_cpu = 		    config_get_string_value(config, "IP_CPU");
//	config_valores.algoritmo_planificacion = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
//
//	config_valores.puerto_memoria =	config_get_int_value(config, 	"PUERTO_MEMORIA");
//	config_valores.puerto_cpu_dispatch = config_get_int_value(config, 	"PUERTO_CPU_DISPATCH");
//	config_valores.puerto_cpu_interrupt = config_get_int_value(config, "PUERTO_CPU_INTERRUPT");
//	config_valores.puerto_escucha = config_get_int_value(config, 	"PUERTO_ESCUCHA");
//	config_valores.estimacion_inicial = config_get_int_value(config, 	"GRADO_MULTIPROGRAMACION");
//    config_valores.grado_multiprogramacion = config_get_int_value(config, 	"ESTIMACION_INICIAL");
//    config_valores.tiempo_maximo_bloqueado = config_get_int_value(config, 	"TIEMPO_MAXIMO_BLOQUEADO");
//
//    config_valores.alfa = config_get_double_value(config, 	"ALFA");
//	//config_destroy(config);
//}


//void iterar_lista(t_list* lista, t_log* logger) {
//    void iterator(char* value) {
//	printf("Valor: %s\n", value);
//	log_info(logger,"%s\n", value);
//    }
//    list_iterate(lista, (void*) iterator);
//}
