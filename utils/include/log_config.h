#ifndef LOG_CONFIG_H_
#define LOG_CONFIG_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>
#include<commons/config.h>
#include<commons/string.h>

#include "sockets.h"

// FUNCIONES

t_log* iniciar_logger(char* archivoLog, char* nombrePrograma, int flagConsola, t_log_level nivelLoggeo);
t_config* iniciar_config(char* path);
char* obtener_de_config(t_config* config, char* key);
int obtener_int_de_config(t_config* config, char* key);
float obtener_float_de_config(t_config* config, char* key);
bool config_tiene_todas_las_propiedades(t_config* cfg, char** propiedades);
void terminar_programa(int conexion, t_log* logger, t_config* config);

#endif
