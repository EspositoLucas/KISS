#include <cpu.h>


//sem_t pedidofin;
///SEMAFOROS
pthread_mutex_t pedidofin;


///VARIABLES GLOBALES
int parar_proceso;

int main()
{

	logger = log_create("log.log", "Servidor CPU", 1, LOG_LEVEL_DEBUG);
	config= iniciar_config("cfg/cpu.config");
	pcb* pcb_recibido=malloc(sizeof(pcb));
	//sem_init(&pedidofin,0,1);
	pthread_mutex_init(&pedidofin,NULL);
	parar_proceso=0;

	config_valores_cpu.entradas_tlb=config_get_int_value(config,"ENTRADAS_TLB");
	config_valores_cpu.reemplazo_tlb=config_get_string_value(config,"REEMPLAZO_TLB");
	config_valores_cpu.retardo_NOOP=config_get_int_value(config,"RETARDO_NOOP");
	config_valores_cpu.ip_memoria=config_get_string_value(config,"IP_MEMORIA");
	config_valores_cpu.puerto_memoria=config_get_string_value(config,"PUERTO_MEMORIA");
	config_valores_cpu.puerto_escucha_dispatch=config_get_string_value(config,"PUERTO_ESCUCHA_DISPATCH");
	config_valores_cpu.puerto_escucha_interrupt=config_get_string_value(config,"PUERTO_ESCUCHA_INTERRUPT");

	///HANDSHAKE

	pthread_t conexion_memoria_i;
	pthread_create(&conexion_memoria_i,NULL,conexion_inicial_memoria,&config_valores_cpu.puerto_memoria);
	pthread_join(conexion_memoria_i,NULL);

	///INICIA LA TLB

	crear_tlb();

	///CREA LA CONEXION CON EL KERNEL

	int server_fd = iniciar_servidor(IP_CPU,config_valores_cpu.puerto_escucha_dispatch);
    log_info(logger, "CPU listo para recibir al modulo cliente");
    int cliente_fd = esperar_cliente(logger,"cpu",server_fd);



    while (1)
    {
        int cod_op = recibir_operacion(cliente_fd);
        switch (cod_op)
        {
        case PCB:
        	pcb_recibido = recibir_pcb(cliente_fd);
        	log_info(logger,"Recibi PCB de Id: %d",pcb_recibido->id_proceso);
        	fetch(pcb_recibido,cliente_fd);
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
pthread_create(&manejoInterrupciones,NULL,interrupt,&config_valores_cpu.puerto_escucha_interrupt);
log_info(logger, "Se creo hilo para recibir interrupciones");
while((int)PCB->program_counter <list_size(PCB->instrucciones)){
	instruccionProxima = list_get(PCB->instrucciones,(int)PCB->program_counter);
	decode(instruccionProxima,PCB);
	PCB->program_counter++;
	if(checkInterrupt()==1){
		enviarPcb(PCB,socket_kernel);
		pthread_cancel(manejoInterrupciones);
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
log_info(logger, "Se ejecuto instruccion NO-OP");
}

void ejecutarIO(int tiempo,pcb* PCB){
PCB->tiempo_de_bloqueo=(double)tiempo;
PCB->estado_proceso=BLOQUEADO;
pthread_mutex_lock(&pedidofin);
parar_proceso++;
pthread_mutex_unlock(&pedidofin);
log_info(logger, "Se ejecuto instruccion I/O");
}

/*void ejecutarREAD(dirLogica){

}

void ejecutarWRITE(dirLogica,valor){

}

void ejecutarCOPY(dirLogicaDestino,dirLogicaOrigen){

}*/

void ejecutarEXIT(pcb* PCB){
PCB->estado_proceso=FINALIZADO;
pthread_mutex_lock(&pedidofin);
parar_proceso++;
pthread_mutex_unlock(&pedidofin);
log_info(logger, "Se ejecuto instruccion EXIT");
}

int checkInterrupt(){
pthread_mutex_lock(&pedidofin);
if(parar_proceso>0){
	pthread_mutex_unlock(&pedidofin);
	return 1;
}else{
	pthread_mutex_unlock(&pedidofin);
return 0;}
}
//MANEJO DE INTERRUPCIONES
void* interrupt(void* interrupt){
int server_fd = iniciar_servidor(IP_CPU,interrupt);
log_info(logger, "CPU listo para recibir interrupciones");
int cliente_fd = esperar_cliente(logger,"cpu",server_fd);
log_info(logger,"Se conecto Kernel al puerto interrupt");
while(1){
	int cod_op = recibir_operacion(cliente_fd);
	switch (cod_op){
		case MENSAJE:
			log_info(logger,"Peticion de interrupcion recibida");
			pthread_mutex_lock(&pedidofin);
			parar_proceso++;
			pthread_mutex_unlock(&pedidofin);

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
///CONEXION A MEMORIA: HANDSHAKE
void* conexion_inicial_memoria(void* puerto_memoria){
	socket_memoria=crear_conexion(IP_CPU,puerto_memoria);
	pedir_handshake(socket_memoria);
	int codigo_memoria;
	while(1){
		codigo_memoria=recibir_operacion(socket_memoria);
		switch(codigo_memoria){
			case PAQUETE:
				configuracion_tabla=recibir_handshake(socket_memoria);
				log_info(logger,"Recibi configuracion por handshake");
				return NULL;
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

