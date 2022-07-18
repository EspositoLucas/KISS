#include "memoria.h"
#include <sys/mman.h>
#include "string.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#ifndef INCLUDE_SWAP_H_
#define INCLUDE_SWAP_H_

//VARIABLES

void* archivo_swap ; // la hago global para usarlo en otras funciones una vez que se crea el swap
t_list* archivos;
uint32_t archivo ;

//STRUCTS

typedef struct
 {
 	uint32_t pid ;
 	void* archivo;

 } archivos_swap;


//FUNCIONES

char* armarPath(int );
void crearSwap(uint32_t );
void eliminarSwap(pcb* );
void suspender_proceso(int ) ;
bool modificados(t_p_2* );
t_list* marcosMod(t_list*);
void escribirPagEnSwap(t_p_2*);
void escribirPaginasModificadas(pcb* );
void* traerPaginaDeSwap(uint32_t );

#endif /* INCLUDE_SWAP_H_ */
