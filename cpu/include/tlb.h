#ifndef INCLUDE_TLB_H_
#define INCLUDE_TLB_H_

#include "mmu.h"
#include "cpu.h"


int obtener_algoritmo();
void crear_tlb();
int buscar_en_la_tlb(uint32_t pagina);
void tlb_hit(traduccion_t*);
void tlb_miss(traduccion_t*);
void agregar_a_tlb_menor_a_entradas(traduccion_t*);
void agregar_a_tlb_fifo(traduccion_t*);
void agregar_a_tlb_lru(traduccion_t*);
traduccion_t* restar_uno_al_turno_de_reemplazo(traduccion_t*);
traduccion_t* sumar_uno_al_turno_de_reemplazo(traduccion_t*);
bool encontrar_fifo(traduccion_t*);
void* traduccionConMayorEspera(traduccion_t*, traduccion_t*);
bool pagina_se_encuentra_en_tlb(traduccion_t*);
void vaciarTlb();
#endif /* INCLUDE_TLB_H_ */
