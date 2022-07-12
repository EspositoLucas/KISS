
#ifndef CONSOLA_H
#define CONSOLA_H

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <readline/readline.h>
#include "comunicacion.h"
#include "log_config.h"
#include "sockets.h"

t_log *consola_logger;

// PUERTOS

typedef struct // archivo de configuracion consola
{
    char* ip_kernel;
    char* puerto_kernel;

} arch_config;

arch_config config_valores_consola;

// FUNCIONES

t_config* cargar_configuracion(char *);
t_list *parsear_instrucciones(char*);
instruccion *armar_estructura_instruccion(codigo_instrucciones, uint32_t, uint32_t);
char *leer_archivo(char *);
void serializar_instrucciones(t_list*, t_paquete*);

#endif
