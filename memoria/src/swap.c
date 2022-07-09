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
	} else {
		log_info(logger,"Se creo el archivo swap del proceso: %d\n",idProceso);
	}
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
	t_list* paginas_proceso = paginas_por_proceso(pcb->id_proceso);
	t_list* paginas_en_memoria = list_filter(paginas_proceso,pagina_con_presencia);
	t_list* paginas_con_bit_modificado = list_filter(paginas_en_memoria, pagina_con_modificado);

	t_p_2* aux;

	// armo y abro el swap
	int fd;
	char* path = armarPath(pcb->id_proceso);
	fd = open(path,O_RDWR);
	// mapeo el swap a void* para el mmap
	void* archivo_swap = mmap(NULL,pcb->tamanio_proceso,PROT_WRITE|PROT_READ,MAP_SHARED,fd,0);



	//chequeo bit modificado de las paginas y las escribo en swap si esta en 1

	for (int i = 0; i < list_size(pagina_con_modificado); i++){
		aux = list_get(paginas_con_bit_modificado,i);
		escribir_en_swap(archivo_swap,aux);
		liberar_marco(pcb->id_proceso,aux);
	}

	//donde hay que usar el munmap() ?
}
bool modificados(t_p_2* marq){
	return marq->m;
}
t_list* marcosMod(t_list* marquinhos){
	t_list* modificado=list_filter(marquinhos,modificados);
	return modificado;
}
void escribirPagenSwap(t_p_2* pag,void* swap){
	memcpy(swap+get_marco_offset(pag->indice),memoria_usuario+get_marco_offset(pag->marco),config_valores_memoria.tam_pagina);
}

void escribirPaginasModificadas(pcb* pcb){
	int fd ;
	char* path=armarPath(pcb->id_proceso);
	fd=open(path,O_RDWR);
	void* archivo_swap = mmap(NULL,pcb->tamanio_proceso,PROT_WRITE|PROT_READ,MAP_SHARED,fd,0);
	t_list* paginasProc=marcosMod(paginasEnMemoria(pcb->id_proceso));
	for(int i=0;i<list_size(paginasProc);i++){
		t_p_2* pag=list_get(paginasProc,i);
		escribirPagenSwap(pag,archivo_swap);
	}
	munmap(archivo_swap,pcb->tamanio_proceso);
	close(fd);
}

void escribir_en_swap(void* archivo, t_p_2* tp2){
	//TODO
	int offset = tp2->marco * config_valores_memoria.tam_pagina ; //desplazamiento
	memcpy(archivo + offset, tp2->marco, config_valores_memoria.tam_pagina); // aca se copia el indice o el marco de la tabla  con el tam_pagina ?
}



