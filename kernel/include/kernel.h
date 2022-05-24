#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include "comunicacion.h"
#include "log_config.h"
#include "sockets.h"

// PUERTOS

#define IP_KERNEL "0.0.0.0"
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
    float estimacion_inicial ;
    float alfa;
    int grado_multiprogramacion ;
    int tiempo_maximo_bloqueado;

 } arch_config;

arch_config config_valores_kernel ;

// FUNCIONES

// void cargar_configuracion();
// pcb *recibir_paquete_instrucciones(int );
// pcb *deserializar_paquete_instrucciones_consola(t_buffer* );
// t_buffer *recibir_buffer_instrucciones(int );
pcb *armar_pcb(t_buffer* );
t_buffer *recibir_buffer_instrucciones(int );
void iterator(char *value);

#endif 
