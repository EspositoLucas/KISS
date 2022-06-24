#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <semaphore.h>
#include "comunicacion.h"
#include "log_config.h"
#include "sockets.h"
#include "tlb.h"

#define IP_CPU "0.0.0.0"
#define PUERTO_CPU "8001"

t_config *config;
int socket_memoria;
t_handshake* configuracion_tabla;

typedef struct  // archivo de configuracion cpu
{
   char* ip_cpu;
   int entradas_tlb;
   char* reemplazo_tlb;
   int retardo_NOOP;
   char* ip_memoria;
   char* puerto_memoria;
   char* puerto_escucha_dispatch;
   char* puerto_escucha_interrupt;
} arch_config;

arch_config config_valores_cpu;

void iterator(char *value);
void* ciclo_de_instruccion(pcb*,int);
void decode(instruccion*,pcb*);
void ejecutarNO_OP();
void ejecutarIO(int,pcb*);
void ejecutarREAD(uint32_t,pcb*);
void ejecutarWRITE(uint32_t,uint32_t,pcb*);
void ejecutarCOPY(uint32_t,uint32_t,pcb*);
void ejecutarEXIT(pcb*);
void* interrupt(void* interrupt);
int checkInterrupt();
void enviarPcb(pcb*,int);
void cargar_config();
void*conexion_inicial_memoria(void*);

#endif
