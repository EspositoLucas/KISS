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

// PUERTOS

typedef struct // archivo de configuracion consola
{
    char* ip_kernel;
    char* puerto_kernel;

} arch_config;

arch_config config_valores_consola;

// FUNCIONES

char *leer_archivo(char*);
void paquete_proceso(int, t_paquete*, int);

#endif
