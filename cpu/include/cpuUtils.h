#ifndef CPUUTILS_H_
#define CPUUTILS_H_

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

#define IP_CPU "0.0.0.0"
#define PUERTO_CPU "8001"


typedef enum
{
    MENSAJE,
    PCB,
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

typedef struct  // archivo de configuracion cpu
{
   int entradas_tlb;
   char* reemplazo_tlb;
   int retardo_NOOP;
   char* ip_memoria;
   int puerto_memoria;
   int puerto_escucha_dispatch;
   int puerto_escucha_interrupt;

} arch_config;

arch_config config_valores_cpu ;

typedef struct
{
    int socket;
    int socket_anterior;
} t_conexiones;

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
