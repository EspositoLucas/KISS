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

// STRUCTS/ENUMS

//typedef enum{
//	FIFO,
//	SRT
//}t_algoritmo_planificacion;

// FUNCIONES

//t_algoritmo_planificacion obtener_algoritmo(void);
//void inicializar_listas(void);
//void inicializar_semaforos(void);
//void avisarAModulo(int socket_fd,op_code codigo);
//void enviar_pcb_a_memoria(pcb *, int );
//uint32_t obtener_entrada_tabla_de_pagina(int );
//void eliminar_pcb(pcb *);
//void destruir_semaforos(void);
//void destruir_listas(void);
//void destruirListaYElementos(t_list* );

// VARIABLES

uint32_t generador_de_id;

// Kernel

// t_log *kernel_logger;
// int socket_cpu_interrupt;
// int socket_cpu_dispatch;
// int socket_memoria;

// Planificador Largo Plazo

// uint32_t generador_de_id;
// t_list* colaNew;
// t_list *colaExit;
// sem_t sem_admitir;
// sem_t sem_exit;
// sem_t sem_grado_multiprogramacion;
 pthread_mutex_t mutex_generador_id;
// pthread_mutex_t mutex_new;
// pthread_mutex_t mutex_exit;
// pthread_t thread_exit;
// pthread_t thread_admitir;


// Planificador Corto Plazo

// t_list *colaReady;
// t_list *colaExec;
// t_list *colaBlocked;
// sem_t sem_ready;
// sem_t sem_exec;
// sem_t sem_blocked;
// sem_t sem_desalojo;
// pthread_mutex_t mutex_ready;
// pthread_mutex_t mutex_blocked;
// pthread_mutex_t mutex_exec;
// pthread_t thread_ready;
// pthread_t thread_exec;
// pthread_t thread_blocked;


// Planificador Mediano Plazo

// t_list *colaSuspendedBlocked;
// t_list *colaSuspendedReady;
// sem_t sem_suspended_ready;
// pthread_mutex_t mutex_suspended_blocked;
// pthread_mutex_t mutex_suspended_ready;
// pthread_t thread_suspended_ready;


#endif
