#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include "comunicacion.h"
#include "log_config.h"
#include "sockets.h"
#include "planificacion.h"

// PUERTOS

#define IP_KERNEL "127.0.0.1"
#define PUERTO_KERNEL "8000"

//STRUCTS/ENUMS

 typedef struct  // archivo de configuracion kernel
 {
    char* ip_memoria;
    int puerto_memoria;
    char* ip_cpu;
    int puerto_cpu_dispatch;
    int puerto_cpu_interrupt;
    int puerto_escucha;
    char* algoritmo_planificacion;
    float estimacion_inicial;
    float alfa;
    int grado_multiprogramacion;
    int tiempo_maximo_bloqueado;

 } arch_config;

arch_config config_valores_kernel ;

typedef struct
{
    int socket;
    int socket_anterior; 
} t_conexiones; // para el manejo de las conexiones entre los clientes que se vayan conectando y tener referencia el ultimo y proximo que se conecto

typedef struct
{
    uint32_t tamanio_proceso;
    t_list* instrucciones; 
} t_consola;

// FUNCIONES

//void cargar_configuracion();
//void manejar_conexion(t_buffer *  ,pcb* );
//void manejo_recepcion(int );
//void manejar_clientes(int ,int );
//void destruir_semaforos(void);
//void destruir_listas(void);
//void inicializarListas(void);
//void inicializarPlanificacion(void);
//void inicializarSemaforos(void);
t_list *deserializar_instrucciones(t_list *, uint32_t );
t_consola *deserializar_consola(int );


void iterator(char *value);

#endif 
