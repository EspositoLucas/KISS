#ifndef PLANIFICADOR_A_LARGO_PLAZO_H_
#define PLANIFICADOR_A_LARGO_PLAZO_H_

#include "kernel.h"
#include <commons/collections/queue.h>
#include <semaphore.h>

// LISTAS/COLAS

t_queue* colaNew;
t_list* colaReady;
t_list* listaExe;
t_list* listaBlock;
t_list* listaExit;
t_list* listaBlockSuspended;
t_queue* colaReadySuspended;

// SEMAFOROS 
pthread_mutex_t mutexNew;
pthread_mutex_t mutexReady;
pthread_mutex_t mutexBlock;
pthread_mutex_t mutexExe;
pthread_mutex_t mutexExit;
pthread_mutex_t mutexBlockSuspended;
pthread_mutex_t mutexReadySuspended;

sem_t contadorNew;
sem_t contadorReady;
sem_t contadorExe;
sem_t contadorProcesosEnMemoria;
sem_t multiprogramacion;
sem_t multiprocesamiento;
sem_t contadorBlock;
sem_t analizarSuspension;
sem_t suspensionFinalizada;
sem_t largoPlazo;
sem_t contadorReadySuspended;
sem_t medianoPlazo;

// FUNCIONES

void hiloNew_Ready() ;
t_pcb sacarDeNew(); 
void agregarANewProceso(t_pcb proceso);
void avisarAMemoria(t_pcb proceso)

#endif