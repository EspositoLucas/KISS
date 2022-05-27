#ifndef SOCKET_H_
#define SOCKET_H_

#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>


t_log* logger;

// FUNCIONES

int crear_conexion( char* , char* );
void liberar_conexion(int socket_cliente);
int iniciar_servidor(char* , char* );
int esperar_cliente(t_log*,const char*, int  );

#endif
