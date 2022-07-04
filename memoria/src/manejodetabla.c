#include "manejodetabla.h"


uint32_t numero_tabla_2p ;

//------------------------ACCESO A PRIMERA TABLA------------------------
uint32_t devolver_entrada_a_segunda_tabla(uint32_t tabla,uint32_t entrada){
	t_p_1* tabla_1 =  list_get(tabla_de_pagina_1_nivel,(int)entrada);
	return tabla_1->numero_de_tabla2;
}


 //-------------INICIALIZAR TABLA DE PRIMER NIVEL------------------------------------

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

bool posicion_vacia(t_p_1* posicion){
	bool respuesta = posicion->numero_de_tabla2 == -1;
	return respuesta;
}

t_p_1* buscar_posicion_libre(){
	t_p_1* posicion;
	posicion = list_find(tabla_de_pagina_1_nivel,posicion_vacia);
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


///------------ALGORITMO_REEMPLAZO---------------

uint32_t  obtenerPaginaAReemplazar(uint32_t entrada_primer_nivel){

	uint32_t pagina_reemplazo ;
	numero_tabla_2p = devolver_entrada_a_segunda_tabla(0,entrada_primer_nivel);

	t_list* tabla_auxiliar = list_filter(lista_tablas_segundo_nivel, condicion_misma_numero_tabla);
	t_list* tabla_marcos = list_create();

	for(int i = 0 ; i<list_size(tabla_auxiliar);i++){
		tabla_de_segundo_nivel* tabla_auxiliar2 = list_get(tabla_auxiliar,i);
		t_list* paginas_en_memoria = list_filter(tabla_auxiliar2->lista_paginas,pagina_con_presencia);
		list_add_all(tabla_marcos,paginas_en_memoria);
	}


	algoritmo algoritmo_reemplazo = obtener_algoritmo();

 		switch(algoritmo_reemplazo){
 		case CLOCK:
 			pagina_reemplazo = obtenerPaginaClock(tabla_marcos);
 			break;
 		case CLOCK_M:
 			pagina_reemplazo = obtenerPaginaClockM(tabla_marcos);
 			break;
 		}

 	return pagina_reemplazo ;
 }


algoritmo obtener_algoritmo(){

 	 algoritmo switcher;
 	 char* algoritmo_reemplazo = config_valores_memoria.algoritmo_reemplazo ;
 	    //CLOCK
 	 if (strcmp(algoritmo_reemplazo,"CLOCK") == 0)
 	 {
 		 switcher = CLOCK;
 	     log_info(logger, "El algoritmo de planificacion elegido es CLOCK.");
 	 }

 	    //CLOCK-M
 	 if (strcmp(algoritmo_reemplazo,"CLOCK-M") == 0)
 	 {
 		 switcher = CLOCK_M;
 	     log_info(logger, "El algoritmo de planificacion elegido es CLOCK-M.");
 	 }
 	 return switcher;
}

uint32_t  obtenerPaginaClock(t_list* tabla_marcos) {

	tabla_reemplazo_clock_clock_modificado tabla_reemplazo ;
	uint32_t pagina_reemplazada ;


	return pagina_reemplazada ;
}

uint32_t  obtenerPaginaClockM(t_list* tabla_marcos) {

	tabla_reemplazo_clock_clock_modificado tabla_reemplazo ;
	uint32_t pagina_reemplazada ;


	return pagina_reemplazada ;
}


bool condicion_misma_numero_tabla(tabla_de_segundo_nivel* tabla){
	return tabla->id_tabla == numero_tabla_2p;

}

bool pagina_con_presencia(t_p_2* tabla){
	return tabla->p ;

}

