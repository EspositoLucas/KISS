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
    int id_proceso ;
    const int tamanio_proceso ;
    t_list* instrucciones ;
    int valor_tabla_paginas ;
    int program_counter;
    double estimacion_rafaga ;
    char* estado ;

}t_pcb ;
typedef struct  // archivo de configuracion kernel
{
   char* ip_memoria;
   int puerto_memoria;
   char* ip_cpu;
   int puerto_cpu_dispatch;
   int puerto_cpu_interrupt;
   int puerto_escucha;
   char* algoritmo_planificacion;
   int estimacion_inicial ;
   double alfa;
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

void *recibir_buffer(int *, int);
void cargar_configuracion();

t_config* leer_config(void);
int iniciar_servidor(void);
int esperar_cliente(int);
t_list *recibir_paquete(int);
void recibir_mensaje(int);
int recibir_operacion(int);

#endif
