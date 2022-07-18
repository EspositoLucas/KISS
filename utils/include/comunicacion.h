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
	SUSPENDER_PROCESO,
	ESPACIO_PCB_LIBERADO,
	INICIALIZAR_ESTRUCTURAS,
	LIBERAR_ESTRUCTURAS,
	ESTRUCTURAS_LIBERADAS,
	FINALIZAR_CONSOLA,
	MARCO,
	TABLA,
	HANDSHAKE,
	INSTRUCCION_MEMORIA,
	INTERRUPCION,
	SUSPENDER_PCB,
	ESCRITURA_OK,
	ESCRITURA_ERROR
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


typedef struct{
	int socket;
	pcb *pcb;
	uint32_t tiempo_inicio_bloqueo;
 }proceso;

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
	SRT,
	CLOCK,
	CLOCK_M
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


typedef struct{
	uint32_t indice;
	uint32_t numero_de_tabla2;
}t_p_1; // entrada primer nivel

typedef struct{
	uint32_t indice;
	uint32_t marco;
	bool p;
	bool u;
	bool m;
	bool puntero_indice;
}t_p_2; // entrada segundo nivel

 t_list* lista_tablas_segundo_nivel ;


typedef struct{
	int id_tabla;
	t_list* lista_paginas;
	uint32_t p_id ;
}tabla_de_segundo_nivel; // tabla de pagina

typedef struct{
	char* puerto;
	char* ip;
}conexion_t;

typedef struct{
	int pid;
	int numero_de_marco;
}marquito;

// FUNCIONES


//operacion

int recibir_operacion_nuevo(int);
int recibir_operacion(int);


//mensaje

void *recibir_stream(int *, int);
void enviar_mensaje(char *, int );
void recibir_mensaje(int,t_log*);


//pcb

pcb* deserializar_pcb(void* );
void *serializar_pcb(pcb*);
void eliminar_pcb(pcb* proceso);
void enviarPcb(int ,pcb* );
pcb* recibirPcb(int );

//paquete

t_paquete *crear_paquete(void);
t_paquete *crear_paquete_con_codigo_de_operacion(uint8_t );
void agregar_a_paquete(t_paquete *, void *, int);
void agregar_entero_a_paquete(t_paquete*, int );
void agregaAPaquete(t_paquete*,void* ,int );
void armarPaquete(t_paquete* ,pcb* );
void enviar_paquete(t_paquete*, int);
void* serializar_paquete_con_bytes(t_paquete* , int );
t_list *recibir_paquete(int );
t_list *recibirPaquete(int );
t_paquete* recibe_paquete(int );
t_buffer *serializar_paquete(t_paquete *);
void eliminar_paquete(t_paquete *);


//buffer

void* recibir_buffer(int*, int );
t_buffer *inicializar_buffer_con_parametros(uint32_t, void *);
void agregar_a_buffer(t_buffer *, void *, int);
void agregaABuffer(t_buffer *, void *, int );

//hanshake

t_handshake* recibir_handshake(int);
void pedir_tabla_pagina(int,uint32_t,uint32_t);
void pedir_marco(int,uint32_t,uint32_t);
void pedir_handshake(int);

//conexion

int atender_clientes(int , void (*)(t_paquete *,int));
int enviar_datos(int , void *, uint32_t );
int recibir_datos(int socket_fd, void *dest, uint32_t size);

#endif
