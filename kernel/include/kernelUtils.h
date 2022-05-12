#ifndef KERNEL_UTILS_H_
#define KERNEL_UTILS_H_

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
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>


#define IP_KERNEL "0.0.0.0"
#define PUERTO_KERNEL "8000"

//#define NEW
//#define READY
//#define EXEC
//#define BLOCK
//#define SUSPENDED_BLOCK
//#define SUSPENDED_READY
//#define EXIT

typedef enum
{
    MENSAJE,
    PAQUETE,
} op_code;

typedef struct {
    uint32_t id_proceso ;
    uint32_t tamanio_proceso ;
    uint32_t valor_tabla_paginas ;
    uint32_t program_counter;
    float estimacion_rafaga ;
    double tiempo_bloqueado ;
    uint8_t suspendido;
    char* estado ;
    t_list* instrucciones ; 
}pcb ;

typedef struct  // archivo de configuracion kernel
{
   char* ip_memoria;
   int puerto_memoria;
   char* ip_cpu;
   int puerto_cpu_dispatch;
   int puerto_cpu_interrupt;
   int puerto_escucha;
   char* algoritmo_planificacion;
   float estimacion_inicial ;
   float alfa;
   int grado_multiprogramacion ;
   int tiempo_maximo_bloqueado;

} arch_config;

arch_config config_valores ;

typedef struct
{
    int socket;
    int socket_anterior;
} t_conexiones;

//HILOS PARA PLANIFICACION
//pthread_t hiloNewReady;
//pthread_t hiloReady_Exec;
//pthread_t hiloMedianoPlazo;
//pthread_t hiloQueDesuspende;



t_log *logger;

typedef struct
{
    int size;
    void *stream;
} t_buffer;

typedef struct
{
    op_code codigo_operacion;
    t_buffer *buffer;
}t_paquete;

void *recibir_buffer(int *, int);
void cargar_configuracion();

t_config* leer_config(void);
int iniciar_servidor(void);
int esperar_cliente(int);
t_list *recibir_paquete(int);
void recibir_mensaje(int);
int recibir_operacion(int);
void enviar_mensaje(char *, int );
void *serializar_paquete(t_paquete *, int );
void eliminar_paquete(t_paquete *);
pcb *recibir_paquete_instrucciones(int );
pcb *deserializar_paquete_instrucciones_consola(t_buffer* );
pcb* deserializar_pcb(t_buffer*);
void *serializar_pcb(pcb*);


#endif
