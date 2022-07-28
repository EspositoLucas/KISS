#include "swap.h"

//char* armarPath(int idProceso){
//
//	char* swap =strcat(string_itoa(idProceso),".swap");
//	puts(swap);
//	char* barra = "/" ;
//
//	char* path ;
//	if((path = malloc(strlen(pathSwap)+strlen(barra)+ strlen(swap)+1)) != NULL){
//	    path[0] = '\0';   // ensures the memory is an empty string
//	    strcat(path,pathSwap);
//	    strcat(path,barra);
//	    strcat(path,swap);
//	} else {
//	    puts("Fallo el malloc");
//	}
//	puts(path);
//	return path;
//}

char* armarPath (uint32_t id) {
    char* ruta = string_new();
    char* numero = string_itoa(id);
	string_append(&ruta, "/home/utnso/swap");
	string_append(&ruta, "/");
    string_append(&ruta, numero);
    string_append(&ruta, ".swap");
    free(numero);
    puts(ruta);
    return ruta;
}

void crearSwap(uint32_t idProceso,uint32_t tamanio_proceso){ // el swap se crea una sola vez
	int fd;
	printf("valor pathSwap %s \n",pathSwap);
	char* path=armarPath(idProceso);
	printf("tamanio proceso %d\n",tamanio_proceso);
	if((fd=open(path,O_CREAT|O_EXCL|O_RDWR,S_IRUSR|S_IWUSR))==-1){
		log_info(memoria_logger,"Error al crear el archivo swap del proceso: %d\n",idProceso);
		exit(-1);
	} else {
		log_info(memoria_logger,"Se creo el archivo swap del proceso: %d\n",idProceso);
	}
	ftruncate(fd,tamanio_proceso);

	archivos_swap* archivo= malloc(sizeof(archivos_swap));
	archivo->pid = idProceso;
	archivo->fd = fd;
	archivo->path_swap = path;


	// hacer el mmap para que quede siempre abierto

	archivo->archivo = mmap(NULL,tamanio_proceso,PROT_WRITE|PROT_READ, MAP_FILE|MAP_SHARED,fd,0);
	if (archivo->archivo == MAP_FAILED)
		    {
		        close(fd);
		        perror("Error mmapping the file");
		    }

	pthread_mutex_lock(&mutex_lista_archivo);
	list_add(archivos,archivo);
	pthread_mutex_unlock(&mutex_lista_archivo);


}

void eliminarSwap(pcb* pcb) {
        bool archivos_con_pid(archivos_swap* un_archivo) {
            return un_archivo->pid == pcb->id_proceso;
        }
	pthread_mutex_lock(&mutex_lista_archivo);
	archivos_swap* archivo = (archivos_swap*)list_remove_by_condition(archivos,archivos_con_pid);
	pthread_mutex_unlock(&mutex_lista_archivo);


		munmap(archivo->archivo,pcb->tamanio_proceso); // una vez que se escribio en swap y libero el espacio, ahi recien se hace el free del mmap
		close(archivo->fd); // cierro el archivo swap una vez que s etermino de liberar el archivo swap con el munmap

//		if(remove(armarPath(pcb->id_proceso))==-1){
//			log_info(memoria_logger,"Error al borrar el archivo swap del proceso: %d\n",pcb->id_proceso);
//			exit(-1);
//		} else {
//			log_info(memoria_logger,"Se borro con exito el archivo swap del proceso: %d\n",pcb->id_proceso);
//		}
		if(remove(archivo->path_swap)==-1){
			log_info(memoria_logger,"Error al borrar el archivo swap del proceso: %d\n",pcb->id_proceso);
			exit(-1);
			} else {
				log_info(memoria_logger,"Se borro con exito el archivo swap del proceso: %d\n",pcb->id_proceso);
			}
}


void suspender_proceso(int socket_cliente) { // aca hay que desasignar las paginas del proceso en los marcos en los que están asignadas y escribir en swap si el bit de modificado es 1

	pcb* pcb = recibirPcb(socket_cliente);

	asignarAlArchivo(pcb->id_proceso);

	//chequeo bit modificado de las paginas y las escribo en swap si esta en 1

	usleep(config_valores_memoria.retardo_swap); // retardo swap antes de escribir paginas modificadas

	printf("ANTES DE LIBERAR TODOS LOS MARCOS, PRINTEO EL ESTADO FINAL DE LOS MARCOS DEL PROCESO\n");
		t_list* auxiliar = paginasEnMemoria(pcb->id_proceso);
		for (int i = 0; i < list_size(auxiliar); i++){
			t_p_2* aux = list_get(auxiliar,i);
			printf("Pagina numero: %d, U: %d, M: %d, Puntero : %d\n", aux->indice, aux->u,aux->m,aux->puntero_indice);
		}
	escribirPaginasModificadas(pcb);

	liberarMemoriaUsuario(pcb->id_proceso);

	log_info(memoria_logger,"Se libero memoria usuario del proceso %d \n",pcb->id_proceso);

}
bool modificados(t_p_2* marq){
	return marq->m;
}
t_list* marcosMod(t_list* marquinhos){
	t_list* modificado=(t_list*)list_filter(marquinhos,modificados);
	return modificado;
}
void escribirPagEnSwap(t_p_2* pag){

	usleep(config_valores_memoria.retardo_swap);
	pthread_mutex_lock(&mutex_memoria_usuario);
	pthread_mutex_lock(&mutex_archivo_swap);
	memcpy(archivo_swap+get_marco(pag->indice),memoria_usuario+get_marco(pag->marco),config_valores_memoria.tam_pagina);
	printf("valor indice pag %d \n",pag->indice);
	printf("valor marco pag %d \n",pag->marco);
	pthread_mutex_unlock(&mutex_memoria_usuario);
	pthread_mutex_unlock(&mutex_archivo_swap);
	log_info(memoria_logger,"Se escribio pagina en swap \n");
}


void escribirPaginasModificadas(pcb* pcb){
	t_list* paginasProc=marcosMod(paginasEnMemoria(pcb->id_proceso));
	printf("cant de pags en memoria %d \n",list_size(paginasProc));

	printf("ANTES DE LIBERAR TODOS LOS MARCOS, PRINTEO EL ESTADO FINAL DE LOS MARCOS DEL PROCESO\n");
		for (int i = 0; i < list_size(paginasProc); i++){
			t_p_2* aux = list_get(paginasProc,i);
			printf("Pagina numero: %d, U: %d, M: %d, Puntero : %d\n", aux->indice, aux->u,aux->m,aux->puntero_indice);
		}
	asignarAlArchivo(pcb->id_proceso);
	printf("Se asigno al archivo swap el archivo del pid %d \n|",pcb->id_proceso);
	for(int i=0;i<list_size(paginasProc);i++){
		pthread_mutex_lock(&mutex_contador_pid);
		contador_por_pid* contador  = (contador_por_pid*)list_get(contador_pid,pcb->id_proceso);
		contador->contadorAccesoSwap +=1 ;
		pthread_mutex_unlock(&mutex_contador_pid);
		t_p_2* pag=(t_p_2*)list_get(paginasProc,i);
		escribirPagEnSwap(pag);
		msync(archivo_swap,pcb->tamanio_proceso,MS_SYNC);
		cambiarMdePagina(pag->indice,pcb->id_proceso,0);
		log_info(memoria_logger,"Se escribio pagina modificada en swap \n");

		liberarMarco(pag->marco); // despues de escribir la pag, libero el marco de esa pagina

		log_info(memoria_logger,"Se libero el marco de la pagina modificada en swap \n");
		cambiarUdePagina(pag->indice,pcb->id_proceso,0);
		cambiarPdePagina(pag->indice,pcb->id_proceso,0);
		log_info(memoria_logger,"Se setean de la pag modificada los bits de p,u y m en 0 \n");
	}
}

void* traerPaginaDeSwap(uint32_t numPag){
	usleep(config_valores_memoria.retardo_swap);
	void* pag=malloc(config_valores_memoria.tam_pagina);
	pthread_mutex_lock(&mutex_archivo_swap);
	memcpy(pag,archivo_swap + get_marco(numPag),config_valores_memoria.tam_pagina);
	pthread_mutex_unlock(&mutex_archivo_swap);
	return pag;
}

void asignarAlArchivo(uint32_t pid) {
	 bool archivos_con_pid(archivos_swap* un_archivo) {
	            return un_archivo->pid == pid;
	        }
	pthread_mutex_lock(&mutex_lista_archivo);
	archivos_swap* archivo = (archivos_swap*)list_find(archivos,archivos_con_pid);
	pthread_mutex_unlock(&mutex_lista_archivo);

	pthread_mutex_lock(&mutex_archivo_swap);
	archivo_swap = archivo->archivo;
	pthread_mutex_unlock(&mutex_archivo_swap);
}
