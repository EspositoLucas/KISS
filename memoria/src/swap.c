#include "swap.h"

char* armarPath(int idProceso){
	char* path=string_new();
	path=strcat(pathSwap,strcat("/",strcat(string_itoa(idProceso),".swap")));
	return path;
}

void crearSwap(int idProceso){
	int fd;
	char* path=armarPath(idProceso);
	if((fd=open(path,/*O_RDWR|*/O_TRUNC|O_CREAT|O_EXCL,S_IROTH|S_IWOTH))==-1){
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

void supender_proceso(int socket_cliente) {

	pcb* pcb = recibirPcb(socket_cliente);
	// aca hay que desasignar las paginas del proceso en los marcos en los que están asignadas y escribir en swap si el bit de modificado es 1 pero ni idea como se implementa

	void* archivo_swap = crearSwap(pcb->id_proceso);

	//void* archivo=mmap(NULL,tamanio_proceso,PROT_WRITE|PROT_READ,MAP_SHARED,fd,0);
	//memcpy(archivo_swap + offset, pagina_proceso, tam_pagina) // aca no entiendo que se copia , seria la pagaina con el tam_pagina ?

	// offset= nro_pagina * tam_pagina
}



