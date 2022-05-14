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
pcb *recibir_paquete_instrucciones(int socket_cliente)
{
	t_buffer* buffer = malloc(sizeof(t_buffer)) ;
    int size;
    void *stream ;
    pcb* pcb ;

    stream = recibir_buffer(&size, socket_cliente);
    buffer->stream = stream ;

    pcb = deserializar_paquete_instrucciones_consola(buffer);
    free(buffer);
    return pcb ;
}

 pcb *deserializar_paquete_instrucciones_consola(t_buffer* buffer) // Para deserializar las instrucciones de consola
 {

     pcb* proceso_pcb = malloc(sizeof(pcb)) ;
     int indice_split = 0 ;
     void* stream = buffer->stream ; 
     char* mensaje_consola ; // leido de consola que se envia en el paquete

     
     // Deserializar los campos del buffer

    // memcpy(&(proceso->mensaje_length), stream, sizeof(uint32_t));
    // stream += sizeof(uint32_t);
    mensaje_consola = malloc(sizeof(strlen(mensaje_consola) + 1));
    memcpy(mensaje_consola, stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

//    char** split_buffer = malloc(sizeof(mensaje_consola));
    char** split_buffer = string_split(mensaje_consola, "\n");
//    char** palabras = malloc(sizeof(split_buffer[indice_split])) ;
    char** palabras ;
    proceso_pcb->instrucciones = list_create();

    while (split_buffer[indice_split] != NULL) {
      

        if(string_contains(split_buffer[indice_split], "NO_OP") ) {
            palabras = string_split(split_buffer[indice_split], " ") ;
     		int parametro_NO_OP = atoi(palabras[1]);
            for(int i=0; i< parametro_NO_OP  ; i++){
                printf("NO_OP %d ", parametro_NO_OP);
                list_add(proceso_pcb->instrucciones,palabras[0]) ; // Para agregar a lista a medida quese vaya parseando
            }
        // string_array_destroy(palabras);

        } else { // si no es no op directamente entra aca y se agrega a la lista
            printf("%s",split_buffer[indice_split]);
            list_add(proceso_pcb->instrucciones,split_buffer[indice_split]); // consultar si esto es lo mejor o volver a la anterior de if para cada uno. 
        }

     	// } else if (string_contains(split_buffer[indice_split], "I/O")){
        //     //  int parametro_IO = atoi(palabras[1]);
        //      printf("I/O %d ", parametro_IO);
        //      list_add(proceso->instrucciones,split_buffer[indice_split]) ; 
     	// }
     	//  else if (string_contains(split_buffer[indice_split], "READ")){
        //     //  int parametro_READ = atoi(palabras[1]);
        //     printf("READ %d ", parametro_READ);
        //     list_add(proceso->instrucciones,split_buffer[indice_split]) ; 

     	// } else if (string_contains(split_buffer[indice_split], "WRITE")) {
        //     //  int parametro1_WRITE = atoi(palabras[1]);
        //     //  int parametro2_WRITE = atoi(palabras[2]);
        //      printf("WRITE %d %d ", parametro1_WRITE,parametro2_WRITE);
        //      list_add(proceso->instrucciones,split_buffer[indice_split]) ;

     	// } else if (string_contains(split_buffer[indice_split], "COPY")){
        //     //  int parametro1_COPY = atoi(palabras[1]);
        //     //  int parametro2_COPY = atoi(palabras[2]);
        //      printf("COPY %d %d ", parametro1_COPY,parametro2_COPY);
        //      list_add(proceso->instrucciones,split_buffer[indice_split]) ;

     	// } else if (string_contains(split_buffer[indice_split], "EXIT")){
     	// 	printf("EXIT");
        //     list_add(proceso->instrucciones,split_buffer[indice_split]) ;
     	// }
        indice_split++;
    }    

    string_array_destroy(split_buffer);
    string_array_destroy(palabras);
    list_destroy(proceso_pcb->instrucciones);
    free(mensaje_consola);
//    free(proceso_pcb);

    return proceso_pcb;
}

pcb* deserializar_pcb(t_buffer* buffer) {
    
    pcb* pcb = malloc(sizeof(pcb));
    
    void* stream = buffer->stream;
    int offset = 0;

    //Deserializar los campos int y float
    memcpy(&(pcb->id_proceso), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(pcb->tamanio_proceso), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(pcb->valor_tabla_paginas), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(pcb->program_counter), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(pcb->estimacion_rafaga), stream, sizeof(1));
    stream += sizeof(1);
    memcpy(&(pcb->tiempo_bloqueado), stream, sizeof(1)*2);
    stream += sizeof(1);
    memcpy(&(pcb->suspendido), stream, sizeof(uint8_t));
    stream += sizeof(1);

    //Deserializar los campos char*

    pcb->estado = malloc(sizeof(uint32_t));
    memcpy(&(pcb->estado), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    //Aca en el medio faltaria deserializar los campos de la lista

    for (int i = 0; i < list_size(pcb->instrucciones); i++){ // hacemos un char* a la vez
        memcpy(stream + offset, sizeof(pcb->instrucciones[i]), sizeof(uint32_t));
        offset += sizeof(uint32_t);
        memcpy(stream + offset, list_get(pcb->instrucciones,i), strlen(sizeof(pcb->instrucciones[i])+1));
    }

    // memcpy(&(sizeof(pcb->estado_length)), stream,sizeof(uint32_t));
    // stream += sizeof(uint32_t);
    // pcb->estado = malloc(pcb->estado_length);
    // memcpy( &(pcb->estado),stream,pcb->estado_length);
    return pcb;

}


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

void *serializar_pcb(pcb* pcb)
{
    t_buffer* buffer = malloc(sizeof(t_buffer));

    buffer->size = sizeof(uint32_t) * 5 // Para los unint32
             + sizeof(uint8_t) *  1 // Para los unint8
             + sizeof(1) * 1 // Para los float
             + strlen(pcb->estado) + 1; // Para los char*

    void* stream = malloc(buffer->size);
    int offset = 0; // Desplazamiento

    // Serializar los campos int , float y double

    memcpy(stream + offset, &pcb->id_proceso, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &pcb->tamanio_proceso, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &pcb->valor_tabla_paginas, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &pcb->program_counter, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &pcb->estimacion_rafaga, sizeof(1));
    offset += sizeof(1);
    memcpy(stream + offset, &pcb->tiempo_bloqueado,sizeof(1)*2);
    offset += sizeof(1);
    memcpy(stream + offset, &pcb->suspendido, sizeof(uint8_t));
    offset += sizeof(uint32_t);

    //Serializar los campos char*
    memcpy(stream + offset, sizeof(pcb->estado), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &pcb->estado, strlen(sizeof(pcb->estado)) + 1);

     //Aca en el medio faltaria serializar los campos de la lista
    for (int i = 0; i < list_size(pcb->instrucciones); i++){ // hacemos un char* a la vez
        memcpy(stream + offset,/*list_get(pcb->instrucciones,i)*/ sizeof(pcb->instrucciones[i]), sizeof(uint32_t));
        offset += sizeof(uint32_t);
        memcpy(stream + offset, list_get(pcb->instrucciones,i), strlen(sizeof(pcb->instrucciones[i])+1));

    }

    buffer->stream = stream;


    free(pcb->estado);
    list_destroy(pcb->instrucciones);
    free(stream);

    return buffer ; 

}


void eliminar_paquete(t_paquete *paquete)
{
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);
}
