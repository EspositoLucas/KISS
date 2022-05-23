#ifndef CONSOLA_H
#define CONSOLA_H

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include "consolaUtils.h"

// INCLUDE UTILS

// #include "utils/comunicacion.h"
// #include "utils/log_config.h"


// FUNCIONES

t_log* iniciar_logger(void);
t_config* iniciar_config(void);
char *leer_archivo();
void paquete_proceso(int);
void terminar_programa(int, t_log*, t_config*);


#endif
