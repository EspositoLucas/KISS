#include "swap.h"

void* archivo_swap ; // la hago global para usarlo en otras funciones una vez que se crea el swap

char* armarPath(int idProceso){
	char* path=string_new();
	path=strcat(pathSwap,strcat("/",strcat(string_itoa(idProceso),".swap")));
	return path;
}

void crearSwap(uint32_t idProceso){ // el swap se crea una sola vez
	int fd;
	char* path=armarPath((int)idProceso);
	if((fd=open(path,O_TRUNC|O_CREAT|O_EXCL,S_IROTH|S_IWOTH))==-1){
		log_info(logger,"Error al crear el archivo swap del proceso: %d\n",idProceso);
		exit(-1);
	} else {
		log_info(logger,"Se creo el archivo swap del proceso: %d\n",idProceso);
	}
	ftruncate(fd,idProceso); // pongo el truncate porque el ayudante me dijo que no funcionaba sino el mmap y unmap

	// hacer el mmap para que quede siempre abierto

	archivo_swap = mmap(NULL,idProceso,PROT_WRITE|PROT_READ,MAP_SHARED,fd,0);

	close(fd);
}
void eliminarSwap(int idProceso){
	if(remove(armarPath(idProceso))==-1){
		log_info(logger,"Error al borrar el archivo swap del proceso: %d\n",idProceso);
		exit(-1);
	} else {
		log_info(logger,"Se borro con exito el archivo swap del proceso: %d\n",idProceso);
	}
}

void supender_proceso(int socket_cliente) { // aca hay que desasignar las paginas del proceso en los marcos en los que están asignadas y escribir en swap si el bit de modificado es 1

	pcb* pcb = recibirPcb(socket_cliente);

	// creo el swap directamente con el mmap adentro

	crearSwap(pcb->id_proceso);

	//chequeo bit modificado de las paginas y las escribo en swap si esta en 1

	escribirPaginasModificadas(pcb);

	munmap(archivo_swap,pcb->tamanio_proceso); /// una vez que se escribio en swap y libero el espacio, ahi recien se hace el free del mmap

}
bool modificados(t_p_2* marq){
	return marq->m;
}
t_list* marcosMod(t_list* marquinhos){
	t_list* modificado=list_filter(marquinhos,modificados);
	return modificado;
}
void escribirPagEnSwap(t_p_2* pag,void* swap){
	memcpy(swap+get_marco_offset(pag->indice),memoria_usuario+get_marco_offset(pag->marco),config_valores_memoria.tam_pagina);
}

void escribirPaginasModificadas(pcb* pcb){
		pthread_mutex_lock(&mutex_comparador_pid);
	t_list* paginasProc=marcosMod(paginasEnMemoria(pcb->id_proceso));

	for(int i=0;i<list_size(paginasProc);i++){
		t_p_2* pag=list_get(paginasProc,i);
		escribirPagEnSwap(pag,archivo_swap);
		liberarMarco(pag->marco); // despues de escribir la pag, libero el marco de esa pagina
		liberarPag(pag->indice);
		pthread_mutex_unlock(&mutex_comparador_pid);// libero la pagina del marco relacionado y pongo el bit de presencia en 0
	}
}



