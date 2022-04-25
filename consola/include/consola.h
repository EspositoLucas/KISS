#ifndef CONSOLA_H
#define CONSOLA_H

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>

// #define NO_OP 5
// #define IO 3000
// #define READ 0
// #define WRITE 4 42
// #define COPY 0 4
// #define EXIT



#include "consolaUtils.h"

t_log* iniciar_logger(void);
t_config* iniciar_config(void);
char *leer_archivo();
void paquete_proceso(int);
void terminar_programa(int, t_log*, t_config*);


#endif
