#ifndef PLANIFICACION_H
#define PLANIFICACION_H


#include "kernel.h"
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "comunicacion.h"
#include "log_config.h"
#include "sockets.h"
#include "planificacionUtils.h"



// FUNCIONES


//Largo Plazo

pcb *crear_estructura_pcb(t_consola*);
// void iniciar_planificador_largo_plazo(void);
// void agregarANewPcb(pcb* );
// void transicion_admitir_por_prioridad(void) ;
// void finalizarPcb(pcb*);

//Mediano Plazo

// void iniciar_planificador_mediano_plazo(void);
// void transicion_suspender(pcb *);
// void estado_suspended_ready(void );




#endif
