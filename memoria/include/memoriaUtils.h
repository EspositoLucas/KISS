#ifndef MEMORIA_UTILS_H_
#define MEMORIA_UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/config.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>

#define IP_MEMORIA "0.0.0.0"
#define PUERTO_MEMORIA "8002"

typedef enum
{
    MENSAJE,
    PAQUETE,
} op_code;

typedef struct  // archivo de configuracion memoria
{
	int puerto_escucha;
	int tam_memoria;
	int tam_pagina;
	int entradas_por_tabla;
	int retardo_memoria;
	char* algoritmo_reemplazo;
	int marcos_por_proceso;
	int retardo_swap;
	char* path_swap;
} arch_config;

arch_config config_valores_memoria;

typedef struct
{
    int socket;
    int socket_anterior;
} t_conexiones;

t_log *logger;

typedef struct
{
	int tamanio_proceso;
    int stream_size;
    void *stream;
} t_buffer;

typedef struct
{
    op_code codigo_operacion;
    t_buffer *buffer;
}t_paquete;


void cargar_configuracion();

t_config* leer_config(void);


void *serializar_paquete(t_paquete *, int );
void eliminar_paquete(t_paquete *);

int iniciar_servidor(void);
int esperar_cliente(int);
int recibir_operacion(int);
void *recibir_stream(int *, int);
void recibir_mensaje(int);
t_list *recibir_paquete(int);

void enviar_mensaje(char *, int );

#endif
