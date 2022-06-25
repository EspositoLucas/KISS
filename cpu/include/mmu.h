#ifndef INCLUDE_MMU_H_
#define INCLUDE_MMU_H_

#include "cpu.h"
#include "tlb.h"

uint32_t traducir_dir_logica(uint32_t , uint32_t );
uint32_t obtener_segunda_tabla(uint32_t, uint32_t);
uint32_t obtener_marco(uint32_t, uint32_t);

#endif /* INCLUDE_MMU_H_ */
