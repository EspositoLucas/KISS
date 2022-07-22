#ifndef INCLUDE_MANEJODETABLA_H_
#define INCLUDE_MANEJODETABLA_H_
#include "memoria.h"

//VARIABLES

t_list* lista_de_tablas_de_pagina_2_nivel;
int indice_de_tabla2;
uint32_t numero_tabla_2p;
int pid_comparador;
int indice_comparador;

//FUNCIONES

bool posicion_vacia(t_p_1*);
t_p_1* buscar_posicion_libre();
uint32_t buscar_marco_disponible();
void inicializar_tabla_primer_nivel();
t_list* inicializar_tabla_segundo_nivel();
uint32_t devolver_entrada_a_segunda_tabla(uint32_t,uint32_t);
uint32_t devolver_marco(uint32_t,uint32_t );

algoritmo obtener_algoritmo();
uint32_t obtenerPaginaClock(t_list*,uint32_t);
uint32_t  obtenerPaginaClockM(t_list*, uint32_t);
uint32_t  obtenerPaginaAReemplazar(uint32_t);

bool condicion_misma_numero_tabla(void* );
bool condicion_misma_numero_p_id( tabla_de_segundo_nivel* );
bool pagina_con_presencia(void*);
bool pagina_con_modificado(t_p_2 *pagina);
bool pagConIgualPid(tabla_de_segundo_nivel* );
bool tienePunteroEnCero(t_p_2*);
bool punteroEnUno(t_p_2*);


t_list* paginasEnMemoria(uint32_t );
t_list *paginas_por_proceso(int );
t_list* sortear_segun_marco(t_list*);
bool marcosMin(void* ,void * );
void eliminar_entrada_tp1(pcb*);
uint32_t* indice_tabla_tp2(tabla_de_segundo_nivel* );
uint32_t devolverNroTablaEnSwap(uint32_t,uint32_t );



#endif /* INCLUDE_MANEJODETABLA_H_ */
