#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include "comunicacion.h"
#include "log_config.h"
#include "sockets.h"

// PUERTOS

#define IP_MEMORIA "0.0.0.0"
#define PUERTO_MEMORIA "8002"

// STRUCTS/ENUMS

 typedef struct  // archivo de configuracion memoria
 {
 	int puerto_escucha;
 	int tam_memoria;
 	int tam_pagina;
 	int entradas_por_tabla;
 	int retardo_memoria;
 	char* algoritmo_reemplazo;
 	int marcos_por_proceso;
 	int retardo_swap;
 	char* path_swap;
 } arch_config;

arch_config config_valores_memoria;

#endif
