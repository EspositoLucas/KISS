#include "swap.h"

char* armarPath(int idProceso){

	char* swap =strcat(string_itoa(idProceso),".swap");
	puts(swap);
	char* barra = "/" ;

	char* path ;
	if((path = malloc(strlen(pathSwap)+strlen(barra)+ strlen(swap)+1)) != NULL){
	    path[0] = '\0';   // ensures the memory is an empty string
	    strcat(path,pathSwap);
	    strcat(path,barra);
	    strcat(path,swap);
	} else {
	    puts("Fallo el malloc");
	}
	puts(path);
	return path;
}

void crearSwap(uint32_t idProceso,uint32_t tamanio_proceso){ // el swap se crea una sola vez
	int fd;
	char* path=armarPath((int)idProceso);
	if((fd=open(path,O_TRUNC|O_CREAT|O_EXCL,S_IROTH|S_IWOTH))==-1){
		log_info(memoria_logger,"Error al crear el archivo swap del proceso: %d\n",idProceso);
		exit(-1);
	} else {
		log_info(memoria_logger,"Se creo el archivo swap del proceso: %d\n",idProceso);
	}
	ftruncate(fd,tamanio_proceso);

	// hacer el mmap para que quede siempre abierto

	archivo_swap = mmap(NULL,tamanio_proceso,PROT_WRITE|PROT_READ,MAP_SHARED,fd,0);
	archivos_swap* archivo= malloc(sizeof(archivos_swap));
	archivo->pid = idProceso;
	archivo->archivo = archivo_swap;
	archivo->fd = fd;
	list_add(archivos,archivo);

}
void eliminarSwap(pcb* pcb){
	pthread_mutex_lock(&mutex_comparador_archivo_pid);
	comparador_archivo=pcb->id_proceso;
	pthread_mutex_unlock(&mutex_comparador_archivo_pid);
	pthread_mutex_lock(&mutex_lista_archivo);
	archivos_swap* archivo = (archivos_swap*)list_remove_by_condition(archivos,archivos_con_pid);
	pthread_mutex_unlock(&mutex_lista_archivo);
	//munmap(archivo->archivo,pcb->tamanio_proceso); // una vez que se escribio en swap y libero el espacio, ahi recien se hace el free del mmap
	//close(archivo->fd); // cierro el archivo swap una vez que s etermino de liberar el archivo swap con el munmap
	if(remove(armarPath(pcb->id_proceso))==-1){
		log_info(memoria_logger,"Error al borrar el archivo swap del proceso: %d\n",pcb->id_proceso);
		exit(-1);
	} else {
		log_info(memoria_logger,"Se borro con exito el archivo swap del proceso: %d\n",pcb->id_proceso);
	}
}

bool archivos_con_pid(uint32_t pid) {
	return comparador_archivo == pid;
}

void suspender_proceso(int socket_cliente) { // aca hay que desasignar las paginas del proceso en los marcos en los que están asignadas y escribir en swap si el bit de modificado es 1

	pcb* pcb = recibirPcb(socket_cliente);

	//asignarAlArchivo(pcb->id_proceso);

	//chequeo bit modificado de las paginas y las escribo en swap si esta en 1

	usleep(config_valores_memoria.retardo_swap); // retardo swap antes de escribir paginas modificadas

	escribirPaginasModificadas(pcb);


}
bool modificados(t_p_2* marq){
	return marq->m;
}
t_list* marcosMod(t_list* marquinhos){
	t_list* modificado=list_filter(marquinhos,modificados);
	return modificado;
}
void escribirPagEnSwap(t_p_2* pag){
	usleep(config_valores_memoria.retardo_swap);
	pthread_mutex_lock(&mutex_memoria_usuario);
	memcpy(archivo_swap+get_marco(pag->indice),memoria_usuario+get_marco(pag->marco),config_valores_memoria.tam_pagina);
	pthread_mutex_unlock(&mutex_memoria_usuario);
	log_info(memoria_logger,"Se escribio pagina en swap \n");
}

void escribirPaginasModificadas(pcb* pcb){
	t_list* paginasProc=marcosMod(paginasEnMemoria(pcb->id_proceso));

	//asignarAlArchivo(pcb->id_proceso);

	for(int i=0;i<list_size(paginasProc);i++){
		t_p_2* pag=list_get(paginasProc,i);
		escribirPagEnSwap(pag);
		msync(archivo_swap,pcb->tamanio_proceso,MS_SYNC);
		cambiarMdePagina(pag->indice,pcb->id_proceso,0);
		log_info(memoria_logger,"Se escribio pagina modificada en swap \n");
		liberarMarco(pag->marco); // despues de escribir la pag, libero el marco de esa pagina
		log_info(memoria_logger,"Se libero el marco de la pagina modificada en swap \n");
	}
}

void* traerPaginaDeSwap(uint32_t numPag){
	usleep(config_valores_memoria.retardo_swap);
	void* pag=malloc(sizeof(config_valores_memoria.tam_pagina));
	memcpy(pag,archivo_swap + get_marco(numPag),config_valores_memoria.tam_pagina);
	return pag;
}

void asignarAlArchivo(uint32_t pid) {
	pthread_mutex_lock(&mutex_lista_archivo);
	archivos_swap* archivo = (archivos_swap*)list_find(archivos,archivos_con_pid);
	pthread_mutex_unlock(&mutex_lista_archivo);

	pthread_mutex_lock(&mutex_archivo_swap);
	archivo_swap = archivo->archivo;
	pthread_mutex_unlock(&mutex_archivo_swap);
}
