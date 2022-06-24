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
#include "sockets.h"

// STRUCTS/ENUMS

typedef enum
{
	PAQUETE,
    PAQUETE_CONSOLA,
    MENSAJE,
	PCB,
	LIBERAR_ESPACIO_PCB,
	ESPACIO_PCB_LIBERADO,
	INICIALIZAR_ESTRUCTURAS,
	LIBERAR_ESTRUCTURAS,
	FINALIZAR_CONSOLA,
	MARCO,
	TABLA,
	HANDSHAKE,
	INSTRUCCION_MEMORIA,
	INTERRUPCION
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
}instruccion;

typedef struct
{
    uint32_t tamanio_proceso;
    t_list* instrucciones;
} t_consola;

typedef struct {
    uint32_t id_proceso;
    uint32_t tamanio_proceso;
    uint32_t valor_tabla_paginas;
    uint32_t program_counter;
    float estimacion_rafaga;
    double tiempo_de_bloqueo; // tiempo de IO  para estar bloqueado
    uint8_t rafaga_anterior;
    estado estado_proceso;
    t_list* instrucciones;
}pcb;


t_log *logger;

typedef struct
{
    uint32_t stream_size;
    void *stream;
} t_buffer;

typedef struct
{
    op_code codigo_operacion;
    t_buffer *buffer;
}t_paquete;

typedef struct{
	uint32_t pagina;
	uint32_t marco;
	int turno_reemplazo;
}traduccion_t;

typedef enum{
	FIFO,
	LRU,
	SRT
}algoritmo;

typedef struct{
	t_list* lista;
	algoritmo algoritmo;
	int posicion_a_reemplazar;
}tlb_t;

tlb_t* tlb;

typedef struct{
	uint32_t tam_pagina;
	uint32_t entradas;
} t_handshake;

typedef struct {
	int socket;
	void (*manejo_conexiones)(t_paquete *,int);
} t_socket;

typedef struct{
	uint32_t indice;
	uint32_t numero_de_tabla2;
}t_p_1;

typedef struct{
	uint32_t indice;
	uint32_t marco;
	bool p;
	bool u;
	bool m;
}t_p_2;

typedef struct{
	int id_tabla;
	t_list* lista_marcos;
}tabla_de_segundo_nivel;

// FUNCIONES

int recibir_operacion_nuevo(int);

void *recibir_stream(int *, int);
t_config* leer_config(void);
void recibir_mensaje(int,t_log*);
int recibir_operacion(int);
void enviar_mensaje(char *, int );
t_buffer *serializar_paquete(t_paquete *);
void eliminar_paquete(t_paquete *);
t_list *recibir_paquete(int );
t_paquete* recibe_paquete(int );
pcb* deserializar_pcb(void* stream);
void *serializar_pcb(pcb*);
pcb *armar_pcb(t_buffer* buffer);
void eliminar_pcb(pcb* proceso);
pcb* recibir_pcb(int socket_cliente);
void enviar_mensaje(char *mensaje, int socket_cliente);
t_paquete *crear_paquete(void);
t_paquete *crear_paquete_con_codigo_de_operacion(uint8_t );
void agregar_entero_a_paquete(t_paquete*, int );
void agregar_datos_consola(t_paquete*,void *, int ,int );
void enviar_paquete(t_paquete*, int);
void* recibir_buffer(int*, int );
void* serializar_paquete_con_bytes(t_paquete* , int );

t_buffer *inicializar_buffer_con_parametros(uint32_t, void *);
void agregar_a_paquete(t_paquete *, void *, int);
void agregar_a_buffer(t_buffer *, void *, int);

t_handshake* recibir_handshake(int);
void pedir_tabla_pagina(int,uint32_t,uint32_t);
void pedir_marco(int,uint32_t,uint32_t);
void pedir_handshake(int);

int atender_clientes(int , void (*)(t_paquete *,int));
int enviar_datos(int , void *, uint32_t );
int recibir_datos(int socket_fd, void *dest, uint32_t size);
void ejecutar_instruccion(t_socket *);
t_socket *crear_socket_conexion(int , void (*)());

#endif
