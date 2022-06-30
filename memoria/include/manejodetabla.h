#ifndef INCLUDE_MANEJODETABLA_H_
#define INCLUDE_MANEJODETABLA_H_
#include "memoria.h"

t_list* lista_de_tablas_de_pagina_2_nivel;
int indice_de_tabla=0;
bool posicion_vacia(t_p_1*);
t_p_1* buscar_posicion_libre();
uint32_t buscar_marco_disponible();
void inicializar_tabla_primer_nivel();
t_list* inicializar_tabla_segundo_nivel();
uint32_t devolver_entrada_a_segunda_tabla(uint32_t,uint32_t);

#endif /* INCLUDE_MANEJODETABLA_H_ */
