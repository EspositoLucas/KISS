#ifndef UTILS_H_
#define UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<string.h>
#include<assert.h>

#define IP_KERNEL "127.0.0.1"
#define PUERTO_KERNEL 8000

#define NEW 
#define READY 
#define EXEC 
#define BLOCK 
#define SUSPENDED_BLOCK
#define SUSPENDED_READY
#define EXIT

typedef enum
{
    MENSAJE,
    PAQUETE,
    // NO_OP,
    // IO,
    // READ,
    // WRITE,
    // COPY,
    // EXIT
} op_code;

typedef struct
{
    char* ip_kernel;
    char *puerto_kernel;

} arch_config;

arch_config config_valores ;

t_log *logger;

void *recibir_buffer(int *, int);

int iniciar_servidor(void);
int esperar_cliente(int);
t_list *recibir_paquete(int);
void recibir_mensaje(int);
int recibir_operacion(int);

#endif
