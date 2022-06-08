#ifndef COMUNICACION_H_
#define COMUNICACION_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<commons/string.h>
#include<commons/config.h>
#include<string.h>
#include<assert.h>
#include<signal.h>
#include<stdbool.h>
#include "pthread.h"
#include "semaphore.h"

// STRUCTS/ENUMS

typedef enum
{
    PAQUETE,
    MENSAJE,
	PCB
    // TABLA_PAGINAS
} op_code;

typedef enum
{
    NO_OP,
    IO,
    READ,
    WRITE,
    COPY,
    EXIT
} codigo_instrucciones;

typedef enum{
	NUEVO,
	LISTO,
	EXEC,
	BLOQUEADO,
	BLOQUEADO_SUSPENDIDO,
	LISTO_SUSPENDIDO,
	FINALIZADO
} estado;


typedef struct {
    codigo_instrucciones codigo ;
    uint32_t parametro1;
    uint32_t parametro2;
}instruccion ;


typedef struct {
    uint32_t id_proceso;
    uint32_t tamanio_proceso;
    uint32_t valor_tabla_paginas;
    uint32_t program_counter;
    float estimacion_rafaga;
    double tiempo_de_bloqueo; // tiempo de IO  para estar bloqueado
    uint8_t suspendido;
    uint8_t rafaga_anterior;
    estado estado_proceso;
    t_list* instrucciones;
}pcb;


t_log *logger;

typedef struct
{
//	int tamanio_proceso;
    int stream_size;
    void *stream;
} t_buffer;

typedef struct
{
    op_code codigo_operacion;
    t_buffer *buffer;
}t_paquete;


// FUNCIONES

void *recibir_stream(int *, int);
t_config* leer_config(void);
void recibir_mensaje(int,t_log*);
int recibir_operacion(int);
void enviar_mensaje(char *, int );
void *serializar_paquete(t_paquete *, int );
void eliminar_paquete(t_paquete *);
pcb *recibir_paquete_instrucciones(int );
//pcb* deserializar_pcb(t_buffer*);
void deserializar_pcb(void* stream,pcb* pcb);
void *serializar_pcb(pcb*);
pcb *armar_pcb(t_buffer* buffer);
pcb* recibir_pcb(int socket_cliente);
void enviar_mensaje(char *mensaje, int socket_cliente);
t_paquete *crear_paquete(void);
void agregar_a_paquete(t_paquete*, void *, int );
void agregar_entero_a_paquete(t_paquete*, int );
void agregar_datos_consola(t_paquete*,void *, int ,int );
void enviar_paquete(t_paquete*, int );
void* recibir_buffer(int*, int );
t_buffer *recibir_buffer_proceso(int);


#endif
