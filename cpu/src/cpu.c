#include "cpu.h"
#include "semaphore.h"

sem_t pedidofin;
int parar_proceso;//Variable


int main(int argc, char *argv[])
{

	logger = log_create("log.log", "Servidor CPU", 1, LOG_LEVEL_DEBUG);
	config= iniciar_config("cfg/cpu.config");
	pcb* pcb_recibido=malloc(sizeof(pcb));
	sem_init(&pedidofin,0,1);
	parar_proceso=0;

	config_valores_cpu.entradas_tlb=config_get_int_value(config,"ENTRADAS");
	config_valores_cpu.reemplazo_tlb=config_get_string_value(config,"REEMPLAZO");
	config_valores_cpu.retardo_NOOP=config_get_int_value(config,"RETARDO_NOOP");
	config_valores_cpu.ip_memoria=config_get_string_value(config,"IP_MEMORIA");
	config_valores_cpu.puerto_memoria=config_get_int_value(config,"PUERTO_MEMORIA");
	config_valores_cpu.puerto_escucha_dispatch=config_get_int_value(config,"PUERTO_ESCUCHA_DISPATCH");
	config_valores_cpu.puerto_escucha_interrupt=config_get_int_value(config,"PUERTO_ESCUCHA_INTERRUPT");


	    int server_fd = iniciar_servidor(IP_CPU,PUERTO_CPU);
	    log_info(logger, "CPU listo para recibir al modulo cliente");
	    int cliente_fd = esperar_cliente(logger,"cpu",server_fd);

	    while (1)
	    {
	        int cod_op = recibir_operacion(cliente_fd);
	        switch (cod_op)
	        {
	        case PCB:
	        	pcb_recibido=recibir_pcb(cliente_fd);
	        	log_info(logger,"Recibi PCB de Id: %d",pcb_recibido->id_proceso);
	        	fetch(pcb_recibido,cliente_fd);
	        	/*buffer = recibir_paquete(cliente_fd);
	            log_info(logger, "Me llegaron los mensajes:\n");
	            list_iterate(lista, (void *)iterator);*/
	            break;
	        case -1:
	            log_error(logger, "Fallo la comunicacion. Abortando");
	            return EXIT_FAILURE;
	        break;
	        default:
	            log_warning(logger, "Operacion desconocida");
	            break;
	        }
	    }
	    return EXIT_SUCCESS;
}

void* fetch(pcb* PCB,int socket_kernel){
	instruccion* instruccionProxima;
	pthread_t manejoInterrupciones;
	parar_proceso=0;
	pthread_create(&manejoInterrupciones,NULL,interrupt,config_valores_cpu.puerto_escucha_interrupt);
	while((int)PCB->program_counter <list_size(PCB->instrucciones)){
		instruccionProxima = list_get(PCB->instrucciones,(int)PCB->program_counter);
		decode(instruccionProxima,PCB);
		PCB->program_counter++;
		if(checkInterrupt()==1){
			enviarPcb(PCB,socket_kernel);
			pthread_join(manejoInterrupciones,NULL);
			return NULL;
		}
	}
	return NULL;
}

void decode(instruccion* instruccion,pcb* PCB){

	switch(instruccion->codigo){
	case NO_OP:
		ejecutarNO_OP();
	break;
	case IO:
			ejecutarIO(instruccion->parametro1,PCB);
	break;
	case EXIT:
			ejecutarEXIT(PCB);
	break;
	default:break;
	}

}
///EXECUTE

void ejecutarNO_OP(){
	usleep(config_valores_cpu.retardo_NOOP);
}

void ejecutarIO(int tiempo,pcb* PCB){
	PCB->tiempo_de_bloqueo=(double)tiempo;
	PCB->estado_proceso=BLOQUEADO;
	sem_wait(&pedidofin);
	parar_proceso++;
	sem_post(&pedidofin);
}

/*void ejecutarREAD(dirLogica){

}

void ejecutarWRITE(dirLogica,valor){

}

void ejecutarCOPY(dirLogicaDestino,dirLogicaOrigen){

}*/

void ejecutarEXIT(pcb* PCB){
	PCB->estado_proceso=FINALIZADO;
	sem_wait(&pedidofin);
	parar_proceso++;
	sem_post(&pedidofin);
}

int checkInterrupt(){
	sem_wait(&pedidofin);
	if(parar_proceso>0){
		sem_post(&pedidofin);
		return 1;
	}else{
	sem_post(&pedidofin);
	return 0;}
}
//MANEJO DE INTERRUPCIONES
void* interrupt(void* interrupt){
	int server_fd = iniciar_servidor(IP_CPU,interrupt);
	log_info(logger, "CPU listo para recibir interrupciones");
	int cliente_fd = esperar_cliente(logger,"cpu",server_fd);
	while(1){
		int cod_op = recibir_operacion(cliente_fd);
		switch (cod_op){
			case MENSAJE:
				log_info(logger,"Peticion de interrupcion recibida");
				sem_wait(&pedidofin);
				parar_proceso++;
				sem_post(&pedidofin);

			break;
			case -1:
				log_error(logger, "Fallo la comunicacion. Abortando");
				return (void*)(EXIT_FAILURE);
			break;
			default:
				 log_warning(logger, "Operacion desconocida");
			break;
		}
	}
	return NULL;
}
void enviarPcb(pcb* proceso,int socket_cliente){
	op_code codigo=PCB;
	t_buffer* buffer=serializar_pcb(proceso);
	int bytes=sizeof(int)*2+buffer->stream_size;
	void* a_enviar=malloc(bytes);
	memcpy(a_enviar,&(codigo),sizeof(int));
	memcpy(a_enviar+sizeof(int),buffer->stream_size,sizeof(int));
	memcpy(a_enviar+sizeof(int)*2,buffer->stream,buffer->stream_size);
	send(socket_cliente,a_enviar,bytes,0);
	free(buffer);
}

pcb* recibir_pcb(int socket_cliente){
	int size;
	void *stream;
	pcb* recibido=malloc(sizeof(pcb));
	stream= recibir_stream(&size, socket_cliente);
	deserializar_pcb(stream,recibido);
	free(stream);
	return recibido;
}
t_buffer *serializar_pcb(pcb* pcb)
{
    t_buffer* buffer = malloc(sizeof(t_buffer));
    buffer->stream_size = sizeof(uint32_t) * 4 // Para los unint32
             + sizeof(1) * 1 // Para los int ;
             + sizeof(float) * 1 // Para los float
             + sizeof(uint8_t) * 2 // Para los uint_8_t
             + list_size(pcb->instrucciones) * sizeof(instruccion);

    void* stream = malloc(buffer->stream_size);
    int offset = 0; // Desplazamiento

    // Serializar los campos int , float y double
    memcpy(stream + offset, &pcb->id_proceso, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &pcb->tamanio_proceso, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &pcb->valor_tabla_paginas, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &pcb->program_counter, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &pcb->estimacion_rafaga, sizeof(float));
    offset += sizeof(float);
    memcpy(stream + offset, &pcb->tiempo_de_bloqueo,sizeof(double));
    offset += sizeof(double);
    memcpy(stream + offset, &pcb->suspendido, sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(stream + offset, &pcb->rafaga_anterior, sizeof(uint8_t));
    offset += sizeof(uint8_t);

    //Serializar los campos enum
    memcpy(stream + offset, &pcb->estado_proceso, sizeof(estado));
    offset += sizeof(estado);

    //Serializar lista instrucciones
    int cantidad_instrucciones = list_size(pcb->instrucciones) ;
    memcpy(stream + offset, &(cantidad_instrucciones), sizeof(1)); // primero se copia el tamnio de la lista para despues ir agregando los otros campos de la lista
    offset += sizeof(1);
    for (int i = 0; i < cantidad_instrucciones; i++){ // hacemos un char* a la vez
        memcpy(stream + offset, list_get(pcb->instrucciones,i), sizeof(instruccion));
        offset += sizeof(instruccion);
    }
    buffer->stream = stream;
    list_destroy(pcb->instrucciones);
    return buffer ;

}

void deserializar_pcb(void* stream,pcb* pcb) {
    //Deserializar los campos int, double  y float

    memcpy(&(pcb->id_proceso), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(pcb->tamanio_proceso), stream ,sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(pcb->valor_tabla_paginas), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(pcb->program_counter), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(pcb->estimacion_rafaga), stream, sizeof(float));
    stream += sizeof(float);
    memcpy(&(pcb->tiempo_de_bloqueo), stream, sizeof(double));
    stream += sizeof(double);
    memcpy(&(pcb->suspendido), stream, sizeof(uint8_t));
    stream += sizeof(uint8_t);

    //Deserializar los campos enum*

    memcpy(&(pcb->estado_proceso), stream, sizeof(estado));
    stream += sizeof(estado);

    //Deserializar lista instrucciones

    pcb->instrucciones = list_create();

    int cant_instrucciones ;
    memcpy(&(cant_instrucciones), stream, sizeof(1));  // primero se deserializa el tamanio de la lista para despues ir deserializando los otros campos de la lista
    stream += sizeof(1);

    for(int i = 0 ; i<cant_instrucciones; i++) {
    	instruccion* instruccion_deserializar = malloc(sizeof(instruccion));
        memcpy(&(instruccion_deserializar->codigo), stream, sizeof(codigo_instrucciones));
        stream += sizeof(codigo_instrucciones);
        memcpy(&(instruccion_deserializar->parametro1), stream, sizeof(1));
        stream += sizeof(1);
        memcpy(&(instruccion_deserializar->parametro2), stream, sizeof(1));
        stream += sizeof(1);

        list_add(pcb->instrucciones,instruccion_deserializar);
        free(instruccion_deserializar);
    }
}

