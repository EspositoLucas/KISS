#ifndef PLANIFICADOR_A_CORTO_PLAZO_H_
#define PLANIFICADOR_A_CORTO_PLAZO_H_


#include "kernel.h"


#include<commons/collections/list.h>

//sem_t planificacion;
//sem_t operacionesProcesos;
//sem_t planificacionPausada;
//sem_t sincroProcesPlanif;
//sem_t sincroProcesPlanif2;

// typedef struct {
//     t_pcb pcb ;
//     char* estado ;
// }t_proceso 

void planificador_crearTripulantes(int  ,char*,int  );
void planificacionFIFO();
void planificacionSRT();
void agregarAReady(t_pcb* proceso);
void agregarABlock(t_pcb* proceso);
void sacarDeBlock(t_pcb* proceso);



#endif
