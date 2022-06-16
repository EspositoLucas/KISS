#include "memoria.h"

int main(void) {

    logger = log_create("log.log", "Servidor Memoria", 1, LOG_LEVEL_DEBUG);

    int server_fd = iniciar_servidor(IP_MEMORIA,PUERTO_MEMORIA);
    log_info(logger, "Memoria lista para recibir al modulo cliente");


	return EXIT_SUCCESS;
}

// Manejo conexiones - Procesar conexiones con los op code




