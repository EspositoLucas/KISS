#include "kernelUtils.h"

int iniciar_servidor(void)
{

    struct addrinfo hints, *servinfo;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(IP_KERNEL, PUERTO_KERNEL, &hints, &servinfo);

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

int esperar_cliente(int socket_servidor)
{

    struct sockaddr_in direccion_Cliente;
    socklen_t tam_Direccion = sizeof(struct sockaddr_in);

    int socket_cliente = accept(socket_servidor, (void *)&direccion_Cliente, &tam_Direccion);
    log_info(logger, "Se conecto un cliente!");

    return socket_cliente;
}

int recibir_operacion(int socket_cliente)
{
    int cod_op;
    if (recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
        return cod_op;
    else
    {
        close(socket_cliente);
        return -1;
    }
}

void *recibir_buffer(int *size, int socket_cliente)
{
    void *buffer;

    recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
    buffer = malloc(*size);
    recv(socket_cliente, buffer, *size, MSG_WAITALL);

    return buffer;
}

void recibir_mensaje(int socket_cliente)
{
    int size;
    char *buffer = recibir_buffer(&size, socket_cliente);
    log_info(logger, "Me llego el mensaje : %s", buffer);
    free(buffer);
}

t_list *recibir_paquete(int socket_cliente)
{
    int size;
    int desplazamiento = 0;
    void *buffer;
    t_list *valores = list_create();
    int tamanio;

    buffer = recibir_buffer(&size, socket_cliente);

    while (desplazamiento < size)
    {
        memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
        desplazamiento += sizeof(int);
        char *valor = malloc(tamanio);
        memcpy(valor, buffer + desplazamiento, tamanio);
        desplazamiento += tamanio;
        list_add(valores, valor);
    }
    free(buffer);
    return valores;
}

// t_list *recibir_paquete_instrucciones(int socket_cliente) // Para deserializar las instrucciones de consola
// {
//     int size;
//     int desplazamiento = 0;
//     void *buffer;
//     t_list *valores = list_create();
//     int tamanio;
//     int indice_split = 0 ;

//     buffer = recibir_buffer(&size, socket_cliente);
//     char** split_buffer = string_split(buffer, "\n");

//     while (desplazamiento < size && split_buffer[indice_split] != NULL)
//     {
//         memcpy(&tamanio, split_buffer + desplazamiento, sizeof(int));
//         desplazamiento += sizeof(int);
        
//         char** palabras = string_split(split_buffer[indice_split], " ") ;

//         if(string_contains(palabras[0], "NO_OP") ) {
//     		int parametro_NO_OP = atoi(palabras[1]);
//             for(int i=0; i< parametro_NO_OP  ; i++){
//             	printf("NO_OP %d ", parametro_NO_OP);
//                 list_add(proceso.instrucciones) ; // Para agregar a lista a medida quese vaya parseando

//             }

//     	} else if (string_contains(palabras[0], "I/O")){
//             int parametro_IO = atoi(palabras[1]);
//             printf("I/O %d ", parametro_IO);
//     	}
//     	 else if (string_contains(palabras[0], "READ")){
//             int parametro_READ = atoi(palabras[1]);
//             printf("READ %d ", parametro_READ);

//     	} else if (string_contains(palabras[0], "WRITE")) {
//             int parametro1_WRITE = atoi(palabras[1]);
//             int parametro2_WRITE = atoi(palabras[2]);
//             printf("WRITE %d %d ", parametro1_WRITE,parametro2_WRITE);

//     	} else if (string_contains(palabras[0], "COPY")){
//             int parametro1_COPY = atoi(palabras[1]);
//             int parametro2_COPY = atoi(palabras[2]);
//             printf("COPY %d %d ", parametro1_COPY,parametro2_COPY);

//     	} else if (string_contains(palabras[0], "EXIT")){

//     		printf("EXIT");
//     	}
//         memcpy(palabras, split_buffer + desplazamiento, tamanio);
//         desplazamiento += tamanio;
//         list_add(valores, palabras);
//         free(palabras) ;
//     }
        	
//     free(split_buffer);
//     return valores;
// }

void enviar_mensaje(char *mensaje, int socket_cliente)
{
    t_paquete *paquete = malloc(sizeof(t_paquete));

    paquete->codigo_operacion = MENSAJE;
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->size = strlen(mensaje) + 1;
    paquete->buffer->stream = malloc(paquete->buffer->size);
    memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

    int bytes = paquete->buffer->size + 2 * sizeof(int);

    void *a_enviar = serializar_paquete(paquete, bytes);

    send(socket_cliente, a_enviar, bytes, 0);

    free(a_enviar);
    eliminar_paquete(paquete);
}

void *serializar_paquete(t_paquete *paquete, int bytes)
{
    void *magic = malloc(bytes);
    int desplazamiento = 0;

    memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
    desplazamiento += sizeof(int);
    memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
    desplazamiento += sizeof(int);
    memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
    desplazamiento += paquete->buffer->size;

    return magic;
}

void eliminar_paquete(t_paquete *paquete)
{
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);
}
