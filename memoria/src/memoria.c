#include "memoria.h"

int main(void) {

    logger = log_create("log.log", "Servidor Memoria", 1, LOG_LEVEL_DEBUG);

    int server_fd = iniciar_servidor(IP_MEMORIA,PUERTO_MEMORIA);
    log_info(logger, "Memoria lista para recibir al modulo cliente");
    int cliente_fd = esperar_cliente(logger,"memoria",server_fd);

    while (1)
    {
        int cod_op = recibir_operacion(cliente_fd);
        switch (cod_op)
        {
        case MENSAJE:
            recibir_mensaje(cliente_fd,logger);
            break;

        case PAQUETE:
            log_info(logger, "Me llego el paquete\n");
            break;

        case -1:
            log_error(logger, "Fallo la comunicacion. Abortando\n");
            return EXIT_FAILURE;
        default:
            log_warning(logger, "Operacion desconocida\n");
            break;
        }
    }
	return EXIT_SUCCESS;
}
