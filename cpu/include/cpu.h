#ifndef CPU_H_
#define CPU_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include "comunicacion.h"
#include "log_config.h"
#include "sockets.h"

#define IP_CPU "0.0.0.0"
#define PUERTO_CPU "8001"

extern t_config *config;
extern t_log* logger;
typedef struct  // archivo de configuracion cpu
{
   int entradas_tlb;
   char* reemplazo_tlb;
   int retardo_NOOP;
   char* ip_memoria;
   int puerto_memoria;
   int puerto_escucha_dispatch;
   int puerto_escucha_interrupt;

} arch_config;

arch_config config_valores_cpu ;

void iterator(char *value);
void* fetch(pcb*,int);
void decode(instruccion*,pcb*);
void ejecutarNO_OP();
void ejecutarIO(int,pcb*);
//void ejecutarREAD(dirLogica);
//void ejecutarWRITE(dirLogica,valor);
//void ejecutarCOPY(dirLogicaDestino,dirLogicaOrigen);
void ejecutarEXIT(pcb*);
int checkInterrupt();
void enviarPcb(pcb*,int);

#endif
