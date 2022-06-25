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


//STRUCTS/ENUMS

 typedef struct  // archivo de configuracion kernel
 {
    char* ip_memoria;
    int puerto_memoria;
    char* ip_cpu;
    char* puerto_cpu_dispatch;
    char* puerto_cpu_interrupt;
    char* ip_kernel;
    char* puerto_escucha;
    char* algoritmo_planificacion;
    float estimacion_inicial;
    float alfa;
    int grado_multiprogramacion;
    int tiempo_maximo_bloqueado;

} arch_config;

arch_config config_valores_kernel ;
int server_fd;
int socket_dispatch;
int socket_interrupt;

int socket_kernel ;
t_consola * consola ;

// FUNCIONES


int atender_clientes_kernel(int);
void cargar_configuracion(char*);
void manejar_conexion(int);
//void iniciar_planificacion(void);
t_list *deserializar_instrucciones(t_list*, uint32_t );
t_consola *deserializar_consola(int );
void manejo_conexiones(t_paquete* ,int);
void terminar_kernel();

#endif 
