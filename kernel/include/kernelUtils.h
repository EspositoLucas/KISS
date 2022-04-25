#ifndef KERNEL_UTILS_H_
#define KERNEL_UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<string.h>
#include<assert.h>
#include <pthread.h>

// #define IP_KERNEL "127.0.0.1"
// #define PUERTO_KERNEL 8000

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
} op_code;

typedef struct {
    int id ;
    const int tamanio_proceso ;
    char* instrucciones [100] ;
    int pc;
    double estimacion_rafaga ;

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
