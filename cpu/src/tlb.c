#include "tlb.h"

int cantidad_de_traducciones=0;//CONTADOR PARA VER Q TURNO DE REEMPLAZO ASIGNAR A LA TRADUCCION INICIALMENTE (FIFOS)
uint32_t pagina_a_buscar;

///-------------------INICIALIZAR TLB-------------------

int obtener_algoritmo(){
	if(string_equals_ignore_case(config_valores_cpu.reemplazo_tlb,"FIFO")){
		log_info(cpu_logger, "Se elegio algotitmo FIFO \n");
		return FIFO;
	}
	else {
		log_info(cpu_logger, "Se elegio algotitmo LRU \n");
		return LRU;
	}
}

void crear_tlb(){
	tlb=malloc(sizeof(tlb_t));
	tlb->lista=list_create();
	tlb->algoritmo=(algoritmo)obtener_algoritmo();
}

///--------------BUSCAR EN LA TLB-----------------

int buscar_en_la_tlb(uint32_t pagina){
	pagina_a_buscar=pagina;
	if(list_any_satisfy(tlb->lista,pagina_se_encuentra_en_tlb)){
		log_info(cpu_logger, "Se encontro la pagina en la TLB \n");
		traduccion_t* traduccion=(traduccion_t*)list_find(tlb->lista,pagina_se_encuentra_en_tlb);
		tlb_hit(traduccion);
		return traduccion->marco;
	}
	else{
		log_info(cpu_logger, "No se encontro la pagina en la TLB \n");
		return -1;
	}
}

///SE ENCONTRO LA TRADUCCION EN EL TLB
void tlb_hit(traduccion_t* traduccion){

	for(int i = 0 ; i < list_size(tlb->lista) ;i++){
			traduccion_t* pag = (traduccion_t*) list_get(tlb->lista,i);
			log_info(cpu_logger,"Pagina tlb en la posicion despues de TLB_HIT%d : %d \n",i,pag->pagina);
		}
	switch(tlb->algoritmo){
		case FIFO:
			break;
		case LRU:
			tlb->lista=list_map(tlb->lista,sumar_uno_al_turno_de_reemplazo);
			traduccion->turno_reemplazo=0;
			break;
		default:
			break;
		}
}


///NO SE ENCONTRO EN EL TLB

void tlb_miss(traduccion_t* traduccion){

	for(int i = 0 ; i < list_size(tlb->lista) ;i++){
		traduccion_t* pag = (traduccion_t*) list_get(tlb->lista,i);
		log_info(cpu_logger,"Pagina tlb en la posicion despues de TLB_MISS %d : %d \n",i,pag->pagina);
	}

	if(list_size(tlb->lista)<config_valores_cpu.entradas_tlb){
		agregar_a_tlb_menor_a_entradas(traduccion);
	}
	else{
		switch(tlb->algoritmo){
		case FIFO:
			agregar_a_tlb_fifo(traduccion);
			break;
		case LRU:
			agregar_a_tlb_lru(traduccion);
			break;
		default:
			break;
		}
	}
}

//AGREGA LA TRADUCCION A LA TLB CUANDO SE LE PERMITAN CARGAR MAS, POR LAS ENTRADAS PERMITIDAS (ARCHIVO DE CONFIGURACION)

void agregar_a_tlb_menor_a_entradas(traduccion_t* traduccion){
	switch(tlb->algoritmo){
			case FIFO:
				traduccion->turno_reemplazo=cantidad_de_traducciones;//FIFO USA turno_reemplazo COMO CONTADOR DE TRADUCCIONES, SI ES 0 => PROXIMO A REEMPLAZAR
				cantidad_de_traducciones++;//SE SUMA EL CONTADOR DE TRADUCCIONES DE FIFO QUE SIRVE PARA ASIGNAR TURNOS MIENTRAS list_size(tlb.lista)<config_valores_cpu.entradas_tlb
				list_add(tlb->lista,traduccion);
				break;
			case LRU:
				traduccion->turno_reemplazo=0;//LRU USA turno_reemplazo COMO CONTADOR DE USOS, SI ES IGUAL A LA CANTIDAD MAXIMA DE TRADUCCIONES PERMITIDAS => PROXIMO A REEMPLAZAR
				tlb->lista=list_map(tlb->lista,sumar_uno_al_turno_de_reemplazo);
				list_add(tlb->lista,traduccion);
				break;
			default:
				break;
			}
}

//"AGREGAN" LA TRADUCCION A LA TLB CUANDO NO SE LE PERMITAN CARGAR MAS

void agregar_a_tlb_fifo(traduccion_t* traduccion){
	traduccion_t* a_reemplazar=list_find(tlb->lista,encontrar_fifo);//ENCUENTRA EL QUE SE AGREGO PRIMERO
	log_info(cpu_logger, "pagina reemplazada tlb FIFO %d \n",a_reemplazar->pagina);
	tlb->lista=list_map(tlb->lista,restar_uno_al_turno_de_reemplazo);//LE RESTA 1 A LOS turno_reemplazo DE TODOS LOS DE LA LISTA (EXPLICACION linea 83)
	a_reemplazar->marco=traduccion->marco;//REEMPLAZA EL VALOR DE MARCO
	a_reemplazar->pagina=traduccion->pagina;//REEMPLAZA EL VALOR DE PAGINA
	log_info(cpu_logger, "pagina que se asigno tlb FIFO %d \n",a_reemplazar->pagina);
	a_reemplazar->turno_reemplazo=cantidad_de_traducciones-1;//ASIGNA cantidad_de_traducciones COMO turno_reemplazo  QUE ES IGUAL A
														   //LA CANTIDAD MAXIMA DE TRADUCCIONES PARA QUE SEA EL ULTIMO EN REEMPLAZARSE
}
void agregar_a_tlb_lru(traduccion_t* traduccion){
	traduccion_t* a_reemplazar=(traduccion_t*) list_get_maximum(tlb->lista, (void*)traduccionConMayorEspera);//ENCUENTRA EL QUE LLEVA MAS TIEMPO SIN SER USADO
	log_info(cpu_logger, "pagina reemplazada tlb LRU %d \n",a_reemplazar->pagina);
	tlb->lista=list_map(tlb->lista,sumar_uno_al_turno_de_reemplazo);//LE SUMA 1 A LOS turno_reemplazo DE TODOS LOS DE LA LISTA (EXPLICACION linea 88)
	a_reemplazar->marco=traduccion->marco;//REEMPLAZA EL VALOR DE MARCO
	a_reemplazar->pagina=traduccion->pagina;//REEMPLAZA EL VALOR DE PAGINA
	log_info(cpu_logger, "pagina que se asigno tlb LRU %d \n",a_reemplazar->pagina);
	a_reemplazar->turno_reemplazo=0;//ASIGNA 0 COMO turno_reemplazo QUE REFIERE A QUE SE ACABA DE USAR
}

//------------------FUNCIONES AUXILIARES PARA LOS list_find, list_map Y list_any_satisfy-----------------

traduccion_t* restar_uno_al_turno_de_reemplazo(traduccion_t* trad){
	trad->turno_reemplazo--;
	return trad;
}
traduccion_t* sumar_uno_al_turno_de_reemplazo(traduccion_t* trad){
	trad->turno_reemplazo++;
	return trad;
}
bool encontrar_fifo(traduccion_t* trad){
	return trad->turno_reemplazo==0;
}
void* traduccionConMayorEspera(traduccion_t* trad1, traduccion_t* trad2){
	return trad1->turno_reemplazo > trad2->turno_reemplazo ? trad1 : trad2;
}
bool pagina_se_encuentra_en_tlb(traduccion_t* trad){
	return trad->pagina==pagina_a_buscar;
}
void vaciarTlb(){
	list_clean_and_destroy_elements(tlb->lista,free);
}
