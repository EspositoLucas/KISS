#include "cpu.h"


int main(int argc, char *argv[])
{

	logger = log_create("log.log", "Servidor CPU", 1, LOG_LEVEL_DEBUG);
	config= iniciar_config("cfg/cpu.config");
	t_buffer * buffer=malloc(sizeof(t_buffer));
	pcb* pcb_recibido=malloc(sizeof(pcb));

	config_valores_cpu.entradas_tlb=config_get_int_value(config,"ENTRADAS");
	config_valores_cpu.reemplazo_tlb=config_get_string_value(config,"REEMPLAZO");
	config_valores_cpu.retardo_NOOP=config_get_int_value(config,"RETARDO_NOOP");
	config_valores_cpu.ip_memoria=config_get_string_value(config,"IP_MEMORIA");
	config_valores_cpu.puerto_memoria=config_get_int_value(config,"PUERTO_MEMORIA");
	config_valores_cpu.puerto_escucha_dispatch=config_get_int_value(config,"PUERTO_ESCUCHA_DISPATCH");
	config_valores_cpu.puerto_escucha_interrupt=config_get_int_value(config,"PUERTO_ESCUCHA_INTERRUPT");



	    int server_fd = iniciar_servidor(IP_CPU,PUERTO_CPU);
	    log_info(logger, "CPU listo para recibir al modulo cliente");
	    int cliente_fd = esperar_cliente(logger,"cpu",server_fd);

	    while (1)
	    {
	        int cod_op = recibir_operacion(cliente_fd);
	        switch (cod_op)
	        {
	        case MENSAJE:
	            recibir_mensaje(cliente_fd,logger);
	            break;
	        case PCB:
	            /*buffer = recibir_paquete(cliente_fd);
	            log_info(logger, "Me llegaron los mensajes:\n");
	            list_iterate(lista, (void *)iterator);*/
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
    // printf("Valor: %s\n", value);
    log_info(logger, "%s", value);
}


