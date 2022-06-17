#include "sockets.h"

//CLIENTE 

int crear_conexion(char *ip, char *puerto)
{
    struct addrinfo hints;
    struct addrinfo *server_info;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ip, puerto, &hints, &server_info);

    int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

    if (connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
    {
        printf("error");
    }

    //freeaddrinfo(server_info);

    return socket_cliente;
}


void liberar_conexion(int socket_cliente) {
    close(socket_cliente);
}
//SERVIDOR

int iniciar_servidor(char *ip, char *puerto)
{

    struct addrinfo hints, *servinfo;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ip, puerto, &hints, &servinfo);

    // Creamos el socket de escucha del servidor

    int socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

    // Asociamos el socket a un puerto

    if (bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen) != 0)
    {
        perror("Fallo el bind");
        exit(1);
    }

    // Escuchamos las conexiones entrantes

    listen(socket_servidor, SOMAXCONN);

    freeaddrinfo(servinfo);
    log_trace(logger, "Listo para escuchar a cliente");

    return socket_servidor;
}

int esperar_cliente(int socket_servidor) {
	struct sockaddr_in dir_cliente;
	socklen_t tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (struct sockaddr *) &dir_cliente, &tam_direccion);
	if(socket_cliente == -1)
		return -1;

	return socket_cliente;
}
