#include "swap.h"

char* armarPath(int idProceso){
	char* path=string_new();
	path=strcat(pathSwap,strcat("/",strcat(string_itoa(idProceso),".swap")));
	return path;
}

void crearSwap(int idProceso/*,int tamanio*/){
	int fd;
	char* path=armarPath(idProceso);
	if((fd=open(path,/*O_RDWR|*/O_TRUNC|O_CREAT|O_EXCL,S_IROTH|S_IWOTH))==-1){
		log_info(logger,"Error al crear el archivo swap del proceso: %d\n",idProceso);
		exit(-1);
	}else{
		log_info(logger,"Se creo el archivo swap del proceso: %d\n",idProceso);
	}
	//void* archivo=mmap(NULL,tamanio,PROT_WRITE|PROT_READ,MAP_SHARED,fd,0);
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
