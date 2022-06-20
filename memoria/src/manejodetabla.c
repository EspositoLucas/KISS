#include "manejodetabla.h"

//------------------------ACCESO A PRIMERA TABLA------------------------
uint32_t devolver_entrada_a_segunda_tabla(uint32_t tabla,uint32_t entrada){
	return (uint32_t) list_get(tabla_de_pagina_1_nivel,(int)entrada);
}



 //-------------INICILIZAR TABLA DE PRIMER NIVEL------------------------------------
void inicializar_tabla_primer_nivel(){
	tabla_de_pagina_1_nivel=list_create();
	for(int i=0;i<config_valores_memoria.entradas_por_tabla;i++){
		t_p_1* entrada_tp1=malloc(sizeof(t_p_1));
		entrada_tp1->indice=i;
		entrada_tp1->numero_de_tabla2=-1;
		list_add(tabla_de_pagina_1_nivel,entrada_tp1);
	}
}

//---------------------------CREAR TABLA DE SEGUNDO NIVEL----------------------------------

t_list* inicializar_tabla_segundo_nivel(){
	t_list* tabla_segundo_nivel=list_create();
	for(int i=0;i<config_valores_memoria.entradas_por_tabla;i++){
		t_p_2* entrada_tp2=malloc(sizeof(t_p_2));
		entrada_tp2->indice=(uint32_t)i;
		entrada_tp2->marco=buscar_marco_disponible();
		/*entrada_tp2->m;
		entrada_tp2->p;
		entrada_tp2->u;*/
		list_add(tabla_segundo_nivel,entrada_tp2);
	}

	return tabla_segundo_nivel;
}


//-------------------------BUSCAR POSICION LIBRE EN TABLA DE PRIMER NIVEL-----------------------------

t_p_1* buscar_posicion_libre(){
	t_p_1* posicion;
	posicion=list_find(tabla_de_pagina_1_nivel,posicion_vacia);
	return posicion;
}
uint32_t buscar_marco_disponible(){
	uint32_t marco=0;
	while((int)marco<cantidad_de_marcos){
		if(memoria_usuario+marco==NULL){
			return marco;
		}
		marco++;
	}
	return -1;
}

bool posicion_vacia(t_p_1* posicion){
	bool respuesta=posicion->numero_de_tabla2==-1;
	return respuesta;
}
