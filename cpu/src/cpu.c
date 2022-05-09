#include <stdlib.h>
#include <stdio.h>
#include <commons/collections/list.h>

int main(int argc, char *argv[])
{
	logger = log_create("log.log", "Servidor CPU", 1, LOG_LEVEL_DEBUG);

	    int server_fd = iniciar_servidor();
	    log_info(logger, "CPU listo para recibir al modulo cliente");
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
	        case PCB:
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
    // printf("Valor: %s\n", value);
    log_info(logger, "%s", value);
}
