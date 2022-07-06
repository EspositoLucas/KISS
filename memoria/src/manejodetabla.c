#include "manejodetabla.h"


uint32_t numero_tabla_2p ;

//------------------------ACCESO A PRIMERA TABLA------------------------
uint32_t devolver_entrada_a_segunda_tabla(uint32_t tabla,uint32_t entrada){
	int entradafinal=(int)(tabla+entrada);
	t_p_1* tabla_1 =  list_get(tabla_de_pagina_1_nivel,entradafinal);
	return tabla_1->numero_de_tabla2;
}
//------------------------ACCESO A SEGUNDA TABLA----------------------------
uint32_t devolver_marco(uint32_t tabla,uint32_t entrada){
	tabla_de_segundo_nivel*tabla_elegida=list_get(lista_tablas_segundo_nivel,tabla);
	t_p_2* pagina=(t_p_2*)list_get(tabla_elegida,entrada);
	if(pagina->p){
		return pagina->marco;
	}
	else{
		//DEBO TRAER LA PAG DESDE MEMORIA (ALGORITMO DE REEMPLAZO)
		return pagina->marco;//pongo esto x ahora para q no rompa
	}
}

//---------------------------CREAR TABLA DE SEGUNDO NIVEL----------------------------------

t_list* inicializar_tabla_segundo_nivel(){
	t_list* tabla_segundo_nivel=list_create();
	for(int i=0;i<config_valores_memoria.entradas_por_tabla;i++){
		t_p_2* entrada_tp2=malloc(sizeof(t_p_2));
		entrada_tp2->indice=(uint32_t)i;
		entrada_tp2->m=0;
		entrada_tp2->p=0;
		entrada_tp2->u=0;
		list_add(tabla_segundo_nivel,entrada_tp2);
	}

	return tabla_segundo_nivel;
}


///------------ALGORITMO_REEMPLAZO---------------

uint32_t  obtenerPaginaAReemplazar(uint32_t entrada_primer_nivel){

	uint32_t pagina_reemplazo ;
	numero_tabla_2p = devolver_entrada_a_segunda_tabla(0,entrada_primer_nivel);

	t_list* tabla_auxiliar =(t_list*) list_filter(lista_tablas_segundo_nivel, condicion_misma_numero_tabla);
	t_list* tabla_marcos = list_create();

	for(int i = 0 ; i<list_size(tabla_auxiliar);i++){
		tabla_de_segundo_nivel* tabla_auxiliar2 = list_get(tabla_auxiliar,i);
		t_list* paginas_en_memoria =(t_list*) list_filter(tabla_auxiliar2->lista_paginas,pagina_con_presencia);
		list_add_all(tabla_marcos,paginas_en_memoria);
	}

 		switch(algoritmo_memoria){
 		case CLOCK:
 			pagina_reemplazo = obtenerPaginaClock(tabla_marcos);
 			break;
 		case CLOCK_M:
 			pagina_reemplazo = obtenerPaginaClockM(tabla_marcos);
 			break;
 		default:break;
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

t_list* paginas_en_memoria(t_list* lista_tablas_segundo_nivel){

	t_list *lista_pags_en_mem = list_create();

	for (int i = 0; i < lista_tablas_segundo_nivel->elements_count ; i++){
		t_p_2 *aux = list_get(lista_tablas_segundo_nivel, i);
		if (pagina_con_presencia(aux)){
			list_add(lista_pags_en_mem, aux);
		}
	}

	list_sort(lista_pags_en_mem, marcosMin);
	return lista_pags_en_mem;
}


//t_list* sortear_segun_marco(t_list* lista_de_tps){
//	t_list* lista_sorteada = list_create();
//
//	lista_sorteada = list_sort(lista_de_tps, marcosMin);
//
//	return lista_sorteada;
//}

bool marcosMin(t_p_2* tp1, t_p_2* tp2){
	return (tp1->marco > tp2->marco);
}
