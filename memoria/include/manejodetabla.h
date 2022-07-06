#ifndef INCLUDE_MANEJODETABLA_H_
#define INCLUDE_MANEJODETABLA_H_
#include "memoria.h"

t_list* lista_de_tablas_de_pagina_2_nivel;
int indice_de_tabla2;
bool posicion_vacia(t_p_1*);
t_p_1* buscar_posicion_libre();
uint32_t buscar_marco_disponible();
void inicializar_tabla_primer_nivel();
t_list* inicializar_tabla_segundo_nivel();
uint32_t devolver_entrada_a_segunda_tabla(uint32_t,uint32_t);
uint32_t devolver_marco(uint32_t,uint32_t );
uint32_t  obtenerPaginaAReemplazar(uint32_t );
algoritmo obtener_algoritmo();
uint32_t obtenerPaginaClock(t_list*);
uint32_t  obtenerPaginaClockM(t_list* );
bool condicion_misma_numero_tabla(void* );
bool pagina_con_presencia(void*);
t_list* paginas_en_memoria(t_list* );
t_list* sortear_segun_marco(t_list*);
bool marcosMin(void* ,void * );
uint32_t uso_en_cero(t_list*);
uint32_t uso_y_mod_en_cero_uno(t_list* );
uint32_t uso_y_mod_en_cero(t_list* lista);


#endif /* INCLUDE_MANEJODETABLA_H_ */
