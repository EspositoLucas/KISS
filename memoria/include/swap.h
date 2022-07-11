#include "memoria.h"
#include <sys/mman.h>
#include "string.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#ifndef INCLUDE_SWAP_H_
#define INCLUDE_SWAP_H_

//FUNCIONES

char* armarPath(int );
void crearSwap(uint32_t );
void eliminarSwap(int );
void supender_proceso(int ) ;
bool modificados(t_p_2* );
t_list* marcosMod(t_list*);
void escribirPagEnSwap(t_p_2* ,void* );
void escribirPaginasModificadas(pcb* );


#endif /* INCLUDE_SWAP_H_ */
