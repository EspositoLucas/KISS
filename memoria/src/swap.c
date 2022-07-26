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
    //printf("valor config valores path swap %s \n",pathSwap);
	string_append(&ruta, "/home/utnso/swap");
	string_append(&ruta, "/");
    string_append(&ruta, numero);
    string_append(&ruta, ".swap");
    free(numero);
    //printf("valor config valores path swap despues append %s \n",pathSwap);
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

	printf("antes de iniciar archivo swap \n");
	archivos_swap* archivo= malloc(sizeof(archivos_swap));
	printf("despues de inicar archivo swap \n");
	archivo->pid = idProceso;
	printf("id de swap : %d \n",archivo->pid);
	printf("archivo swap void \n");
	archivo->fd = fd;
	printf("fd de swap : %d \n",archivo->fd);
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
    printf("tamanio lista archivos %d \n", list_size(archivos));
	pthread_mutex_lock(&mutex_lista_archivo);
	archivos_swap* archivo = (archivos_swap*)list_remove_by_condition(archivos,archivos_con_pid);
	pthread_mutex_unlock(&mutex_lista_archivo);

	printf("tamanio lista archivos %d \n", list_size(archivos));
	printf("id de swap : %d \n",archivo->pid);
				printf("fd de swap : %d \n",archivo->fd);

		munmap(archivo->archivo,pcb->tamanio_proceso); // una vez que se escribio en swap y libero el espacio, ahi recien se hace el free del mmap
		//printf("tamanio lista archivos %d \n", list_size(archivos));
		close(archivo->fd); // cierro el archivo swap una vez que s etermino de liberar el archivo swap con el munmap
		//printf("tamanio lista archivos %d \n", list_size(archivos));
			printf("id de swap : %d \n",archivo->pid);
			printf("fd de swap : %d \n",archivo->fd);
			printf("armar path valor : %s \n",armarPath(pcb->id_proceso));
			printf("valor path swap : %s \n",archivo->path_swap);
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
	pthread_mutex_lock(&mutex_archivo_swap);
	memcpy(archivo_swap+get_marco(pag->indice),memoria_usuario+get_marco(pag->marco),config_valores_memoria.tam_pagina);
	printf("valor indice pag %d \n",pag->indice);
	printf("valor marco pag %d \n",pag->marco);
	pthread_mutex_unlock(&mutex_memoria_usuario);
	pthread_mutex_lock(&mutex_archivo_swap);
	log_info(memoria_logger,"Se escribio pagina en swap \n");
}


void escribirPaginasModificadas(pcb* pcb){
	t_list* paginasProc=marcosMod(paginasEnMemoria(pcb->id_proceso));

	asignarAlArchivo(pcb->id_proceso);

	for(int i=0;i<list_size(paginasProc);i++){
		t_p_2* pag=(t_p_2*)list_get(paginasProc,i);
		escribirPagEnSwap(pag);
		msync(archivo_swap,pcb->tamanio_proceso,MS_SYNC);
		cambiarMdePagina(pag->indice,pcb->id_proceso,0);
		log_info(memoria_logger,"Se escribio pagina modificada en swap \n");
		liberarMarco(pag->marco); // despues de escribir la pag, libero el marco de esa pagina
		log_info(memoria_logger,"Se libero el marco de la pagina modificada en swap \n");
		pag->u = 0;
		pag->m = 0 ;
		pag->p = 0;
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
	printf("tamanio lista archivos %d \n", list_size(archivos));
	archivos_swap* archivo = (archivos_swap*)list_find(archivos,archivos_con_pid);
	printf("tamanio lista archivos %d \n", list_size(archivos));
	printf("archivo pid %d \n", archivo->pid);
	pthread_mutex_unlock(&mutex_lista_archivo);

	pthread_mutex_lock(&mutex_archivo_swap);
	archivo_swap = archivo->archivo;
	pthread_mutex_unlock(&mutex_archivo_swap);
}
