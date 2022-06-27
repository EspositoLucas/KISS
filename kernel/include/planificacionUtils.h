#ifndef PLANIFICACION_UTILS_H
#define PLANIFICACION_UTILS_H


#include "planificacion.h"
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

// FUNCIONES
void inciar_planificacion();
algoritmo obtener_algoritmo(void);
void inicializar_listas(void);
void inicializar_semaforos(void);
void avisarAModulo(int ,op_code );
void enviar_pcb_a_memoria(pcb *, int,op_code  );
pcb* obtener_entrada_tabla_de_pagina(int,pcb*);
void destruir_semaforos(void);
void destruir_listas(void);
void destruirListaYElementos(t_list* );
void interrumpir_cpu();
op_code esperar_respuesta_memoria(int );
void calculoEstimacionProceso(pcb *);
void asignarEstimacionesAProcesos();
pcb* elegirElDeMenorEstimacion();
pcb* obtenerSiguienteSRT();
pcb* obtenerSiguienteFIFO();
pcb* obtenerSiguienteReady();
void ejecutarIO(uint32_t);


// VARIABLES

uint32_t generador_de_id;

// Kernel

 t_log *kernel_logger;
 int socket_cpu_interrupt;
 int socket_cpu_dispatch;
 int socket_consola ;
 int socket_memoria;

typedef struct{
	int socket;
	pcb *pcb;
	uint32_t tiempo_inicio_bloqueo;
 }proceso;

// Planificador Largo Plazo

 t_list* colaNew;
 t_list *colaExit;
 sem_t sem_admitir;
 sem_t sem_exit;
 sem_t sem_grado_multiprogramacion;
 pthread_mutex_t mutex_generador_id;
 pthread_mutex_t mutex_new;
 pthread_mutex_t mutex_exit;
 pthread_t thread_exit;
 pthread_t thread_admitir;

 // TIMER

 pthread_mutex_t mutex_timer;
 pthread_t th_timer;
 uint32_t tiempo;
 void inciar_timer();
 void timer();
 uint32_t get_time();


// Planificador Corto Plazo

 t_list *colaReady;
 t_list *colaExec;
 t_list *colaBlocked;
 sem_t sem_ready;
 sem_t sem_exec;
 sem_t sem_blocked;
 sem_t sem_desalojo;
 pthread_mutex_t mutex_ready;
 pthread_mutex_t mutex_blocked;
 pthread_mutex_t mutex_exec;
 pthread_t thread_ready;
 pthread_t thread_exec;
 pthread_t thread_blocked;


// Planificador Mediano Plazo

 t_list *colaSuspendedBlocked;
 t_list *colaSuspendedReady;
 sem_t sem_suspended_ready;
 pthread_mutex_t mutex_suspended_blocked;
 pthread_mutex_t mutex_suspended_ready;
 pthread_t thread_suspended_ready;


#endif
