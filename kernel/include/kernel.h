#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include "utils/planificacion.h"
#include "kernelUtils.h"

// INCLUDE UTILS

// #include "utils/comunicacion.h"
// #include "utils/log_config.h"



// PUERTOS

// #define IP_KERNEL "0.0.0.0"
// #define PUERTO_KERNEL "8000"


// FUNCIONES

// void cargar_configuracion();
// pcb *recibir_paquete_instrucciones(int );
// pcb *deserializar_paquete_instrucciones_consola(t_buffer* );
// t_buffer *recibir_buffer_instrucciones(int );
void iterator(char *value);

#endif 
