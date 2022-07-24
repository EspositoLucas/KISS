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
uint32_t comparador_archivo ;

//STRUCTS

typedef struct
 {
 	uint32_t pid ;
 	void* archivo;
 	int fd;
 	char* path_swap;

 } archivos_swap;


//FUNCIONES

char* armarPath(int );
void crearSwap(uint32_t,uint32_t );
void eliminarSwap(pcb* );
void suspender_proceso(int ) ;
bool modificados(t_p_2* );
t_list* marcosMod(t_list*);
void escribirPagEnSwap(t_p_2*);
void escribirPaginasModificadas(pcb* );
void* traerPaginaDeSwap(uint32_t );
void asignarAlArchivo(uint32_t );
//bool archivos_con_pid(uint32_t );

#endif /* INCLUDE_SWAP_H_ */
