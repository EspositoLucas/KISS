#include "kernel.h"

int main(void)
{
    logger = log_create("log.log", "Servidor Kernel", 1, LOG_LEVEL_DEBUG);

    int server_fd = iniciar_servidor();
    log_info(logger, "Kernel listo para recibir al cliente");
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
            log_warning(logger, "Operacion desconocida. No quieras meter la pata");
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

//void iterar_lista(t_list* lista, t_log* logger) {
//    void iterator(char* value) {
//	printf("Valor: %s\n", value);
//	log_info(logger,"%s\n", value);
//    }
//    list_iterate(lista, (void*) iterator);
//}
