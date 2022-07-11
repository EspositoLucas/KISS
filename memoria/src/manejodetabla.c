#include "manejodetabla.h"

int pid_comparador;
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
		pthread_mutex_lock(&mutex_comparador);
		if(cantidadUsadaMarcos(tabla_elegida->p_id)<config_valores_memoria.marcos_por_proceso){
			pagina->marco=OcuparMarcolibre(tabla_elegida->p_id);
			pagina->p=true;
			return pagina->marco;
			pthread_mutex_unlock(&mutex_comparador_pid);
		}
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

	uint32_t pagina_reemplazo;
	numero_tabla_2p = devolver_entrada_a_segunda_tabla(0,entrada_primer_nivel);

	t_list* tabla_auxiliar =(t_list*) list_filter(lista_tablas_segundo_nivel,condicion_misma_numero_p_id);
	t_list* tabla_marcos = list_create();

	for(int i = 0 ; i<list_size(tabla_auxiliar);i++){
		tabla_de_segundo_nivel* tabla_auxiliar2 = list_get(tabla_auxiliar,i);
		t_list* paginas_en_memoria =(t_list*) list_filter(tabla_auxiliar2->lista_paginas,pagina_con_presencia);
		list_add_all(tabla_marcos,paginas_en_memoria);
	}
		list_sort(tabla_marcos, marcosMin); // ordeno la lista por numero de marco

 		switch(algoritmo_memoria){
 		case CLOCK:
 			pagina_reemplazo = obtenerPaginaClock(tabla_marcos);
 			break;
 		case CLOCK_M:
 			pagina_reemplazo = obtenerPaginaClockM(tabla_marcos);
 			break;
 		default:break;
 		}

 	return pagina_reemplazo;
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

	tabla_reemplazo_clock_clock_modificado tabla_reemplazo;

	int pagina_reemplazada = -1;

	pagina_reemplazada = uso_en_cero(tabla_marcos); // itera la primera vez

	if (pagina_reemplazada == -1){ // si no habia ninguno en 0 entonces paso todos los uno a 0
		pagina_reemplazada = uso_en_cero(tabla_marcos); // itero denuevo hasta encontrar el primero en 0
	}
	t_p_2* entrada_segundo_nivel ;

	int posicion_en_lista_de_pagina;

	for (int i = 0 ; i < list_size(tabla_marcos); i++){
		t_p_2 *aux = list_get(tabla_marcos,i);
		if (aux->indice == pagina_reemplazada){
			posicion_en_lista_de_pagina = i;
		}
	}

	reasignar_puntero(tabla_marcos, posicion_en_lista_de_pagina+1); // a la posicion i+1 le pone el bit de puntero en true

//	for(int i = 0 ; i < list_size(tabla_marcos);i++){
//		entrada = list_get(tabla_marcos,i);
//	if()
//	}
//	entrada_segundo_nivel->puntero_indice = 1;
	return pagina_reemplazada;
}

void reasignar_puntero(t_list* lista, int posicion){
	t_p_2 *aux = list_remove(lista,posicion); // lo saca
	aux->puntero_indice = true;
	list_add(lista, aux); // lo vuelve a meter acutalizado
}

uint32_t uso_en_cero(t_list* lista){

	int pagina = -1;
	tabla_reemplazo_clock_clock_modificado *aux;
	for (int i =0 ; i < lista->elements_count; i++){
		aux = list_get(lista, i);
		if (aux->bit_uso == 0){
			 pagina = aux->nro_pagina;
		} else {
			aux->bit_uso = 0;
		}
	}

	return pagina;
}

uint32_t  obtenerPaginaClockM(t_list* tabla_marcos) {

	// 1. busco 0/0, sin tocar nada
	// 2. busco 0/1, voy cambiando u a 0
	// 3. busco 0/0
	// 4. busco 0/1


	tabla_reemplazo_clock_clock_modificado tabla_reemplazo ;
	int pagina_reemplazada = -1;

	//Buscar 0/0
	pagina_reemplazada = uso_y_mod_en_cero(tabla_marcos);

	//Buscar 0/1 y voy modificando u a 0

	if(pagina_reemplazada == -1){
		pagina_reemplazada = uso_y_mod_en_cero_uno(tabla_marcos);
	}
	//Buscar 0/0
	if(pagina_reemplazada == -1){
		pagina_reemplazada = uso_y_mod_en_cero(tabla_marcos);
	}

	//Buscar 0/1
	if(pagina_reemplazada == -1){
		pagina_reemplazada = uso_y_mod_en_cero_uno(tabla_marcos);

	}
	return pagina_reemplazada ;
}

uint32_t uso_y_mod_en_cero(t_list* lista){
	int pagina = -1;
	tabla_reemplazo_clock_clock_modificado* aux;
	for (int i = 0; i < lista->elements_count; i++){
		aux = list_get(lista, i);
		if(aux->bit_uso == 0 && aux->bit_modificado == 0){
			pagina = aux->nro_pagina;
		}
	}
	return pagina;
}

uint32_t uso_y_mod_en_cero_uno(t_list* lista){
	int pagina = -1;
	tabla_reemplazo_clock_clock_modificado* aux;
	for (int i = 0; i < lista->elements_count; i++){
		aux = list_get(lista, i);
		if(aux->bit_uso == 0 && aux->bit_modificado == 1){
			pagina = aux->nro_pagina;
		} else {
			aux->bit_uso = 0;
		}
	}
	return pagina;
}

t_list *paginas_por_proceso(int pid){

	t_list* lista_pags_por_proceso = list_create();
	tabla_de_segundo_nivel *aux;
	for (int i = 0; i < list_size(lista_tablas_segundo_nivel); i++){
		aux = list_get(lista_tablas_segundo_nivel,i);
		if (aux->p_id == pid){
			list_add(lista_pags_por_proceso,aux);
		}
	}
	return lista_pags_por_proceso;

}

bool condicion_misma_numero_p_id(void* tabla){
	return ((tabla_de_segundo_nivel *)tabla)->p_id == numero_tabla_2p; // ver con que se compara el p_id

}

bool pagina_con_presencia(void* tabla){
	return ((t_p_2 *)tabla)->p ;

}

bool pagina_con_modificado(t_p_2 *pagina){
	return pagina->m;
}

bool pagConIgualPid(tabla_de_segundo_nivel* tab){
	return tab->p_id==pid_comparador;
}

t_list* paginasEnMemoria(uint32_t pid){

	pthread_mutex_lock(&mutex_comparador_pid);
	pid_comparador=pid;
	pthread_mutex_unlock(&mutex_comparador_pid);
	t_list *lista_pags_en_mem = list_create();
	t_list* tablas_del_proceso=(t_list*)list_filter(lista_de_tablas_de_pagina_2_nivel,pagConIgualPid);
	for(int i=0;i<list_size(tablas_del_proceso);i++){
		tabla_de_segundo_nivel* aux=(tabla_de_segundo_nivel*)list_get(tablas_del_proceso,i);
		t_list* pags_en_memoria=(t_list*)list_filter(aux->lista_paginas,pagina_con_presencia);
		for(int j=0;j<list_size(pags_en_memoria);j++){
			t_p_2* pagReal=malloc(sizeof(t_p_2));
			t_p_2* pagEnMemoria=list_get(pags_en_memoria,j);
			pagReal->m=pagEnMemoria->m;
			pagReal->marco=pagEnMemoria->marco;
			pagReal->p=pagEnMemoria->p;
			pagReal->puntero_indice=pagEnMemoria->puntero_indice;
			pagReal->u=pagEnMemoria->u;
			pagReal->indice=pagEnMemoria->indice+10*i;
			list_add(lista_pags_en_mem,pagReal);
		}
	}

	list_sort(lista_pags_en_mem, marcosMin);
	return lista_pags_en_mem;
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

bool marcosMin(void* tp1, void* tp2){
	return (((t_p_2 *)tp1)->marco < ((t_p_2 *)tp2)->marco);
}
