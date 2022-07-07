#include "swap.h"

char* armarPath(int idProceso){
	char* path=string_new();
	path=strcat(pathSwap,strcat("/",strcat(string_itoa(idProceso),".swap")));
	return path;
}

void crearSwap(uint32_t idProceso){
	int fd;
	char* path=armarPath((int)idProceso);
	if((fd=open(path,O_TRUNC|O_CREAT|O_EXCL,S_IROTH|S_IWOTH))==-1){
		log_info(logger,"Error al crear el archivo swap del proceso: %d\n",idProceso);
		exit(-1);
	}else{
		log_info(logger,"Se creo el archivo swap del proceso: %d\n",idProceso);
	}
	close(fd);
}
void eliminarSwap(int idProceso){
	if(remove(armarPath(idProceso))==-1){
		log_info(logger,"Error al borrar el archivo swap del proceso: %d\n",idProceso);
		exit(-1);
	}
	else{
		log_info(logger,"Se borro con exito el archivo swap del proceso: %d\n",idProceso);
	}
}

void supender_proceso(int socket_cliente) { // aca hay que desasignar las paginas del proceso en los marcos en los que están asignadas y escribir en swap si el bit de modificado es 1

	pcb* pcb = recibirPcb(socket_cliente);
	uint32_t pagina_a_escribir;
	t_list* paginas_proceso;

	// armo y abro el swap
	int fd ;
	char* path=armarPath(pcb->id_proceso);
	fd=open(path,O_RDWR);
	// mapeo el swap a void* para el mmap
	void* archivo_swap = mmap(NULL,pcb->tamanio_proceso,PROT_WRITE|PROT_READ,MAP_SHARED,fd,0);
	pagina_a_escribir=obtenerPaginaAReemplazar(pcb->valor_tabla_paginas);
	//munmap();
	// aca hay que chequear bit modificado de las paginas y escribirlas en swap

	 //int offset= pagina_a_escribir * config_valores_memoria.tam_pagina ;
	// memcpy(archivo_swap + offset, pagina_a_escribir, config_valores_memoria.tam_pagina); // aca se copia la pagina con el tam_pagina ?


}



