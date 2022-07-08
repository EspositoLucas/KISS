#include <cpu.h>


///SEMAFOROS
pthread_mutex_t pedidofin;


///VARIABLES GLOBALES
int parar_proceso=0;

int main()
{

	logger = log_create("log.log", "Servidor CPU", 1, LOG_LEVEL_DEBUG);
	pcb* pcb_recibido=malloc(sizeof(pcb));
	pthread_mutex_init(&pedidofin,NULL);//INICIA EL MUTEX QUE ENGLOBA A parar_proceso

	///CARGA LA CONFIGURACION DE LA CPU
	cargar_config();
	char* ip=config_valores_cpu.ip_cpu;
	char* puerto_dispatch=config_valores_cpu.puerto_escucha_dispatch;
	char* puerto_interrupt=config_valores_cpu.puerto_escucha_interrupt;
	char* ip_memoria=config_valores_cpu.ip_memoria;
	char* puerto_memoria=config_valores_cpu.puerto_memoria;

	///HANDSHAKE

	/*pthread_t conexion_memoria_i;
	pthread_create(&conexion_memoria_i,NULL,conexion_inicial_memoria,&config_valores_cpu.puerto_memoria);//INICIA EL HILO DE CONEXION INICIAL CON MEMORIA (HANDSHAKE)
	pthread_join(conexion_memoria_i,NULL);//ESPERA A RECIBIR EL HANDSHAKE PARA SEGUIR*/

	//INTERRUPCIONES
	pthread_t manejoInterrupciones;
	conexion_t* conexion_interrupcion=malloc(sizeof(conexion_t));
	conexion_interrupcion->ip=ip;
	conexion_interrupcion->puerto=puerto_interrupt;
	pthread_create(&manejoInterrupciones,NULL,interrupt,conexion_interrupcion);//INICIA EL HILO DE ESCUCHA DE INTERRUPCIONES


	///INICIA LA TLB

	crear_tlb();

	///CREA LA CONEXION CON EL KERNEL
	/*puts(config_valores_cpu.ip_cpu);
	puts(config_valores_cpu.puerto_escucha_dispatch);
	puts(config_valores_cpu.puerto_escucha_interrupt);*/
	int server_fd = iniciar_servidor(ip,puerto_dispatch);
    log_info(logger, "CPU listo para recibir al modulo cliente");
    int cliente_fd = esperar_cliente(server_fd);
    printf("Se conecto un cliente por dispatch\n");


    while (1)
    {
    	printf("Miro el op code \n");
        int cod_op = recibir_operacion_nuevo(cliente_fd);
        switch (cod_op)
        {
        case PCB:
        	parar_proceso=0;//INICIA EL CONTADOR DE PARAR PROCESO
        	pcb_recibido = recibirPcb(cliente_fd);
        	log_info(logger,"Recibi PCB de Id: %d",pcb_recibido->id_proceso);
        	ciclo_de_instruccion(pcb_recibido,cliente_fd);//INICIA EL CICLO DE INSTRUCCION
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

void* ciclo_de_instruccion(pcb* PCB,int socket_kernel){//ESTRUCTURA PRINCIPAL DEL CICLO DE INSTRUCCION
instruccion* instruccionProxima;
parar_proceso=0;

log_info(logger, "Se creo hilo para recibir interrupciones");
while((int)PCB->program_counter <list_size(PCB->instrucciones)){
	instruccionProxima = list_get(PCB->instrucciones,(int)PCB->program_counter);//FETCH
	decode(instruccionProxima,PCB);//DECODE (CON EXECUTE INCLUIDO)
	PCB->program_counter++;//ACTUALIZA EL PCB
	if(checkInterrupt()==1){//SE FIJA QUE NO HAYA PEDIDO DE PARAR EL PROCESO ANTES DE SEGUIR CON EL CICLO DE INSTRUCCION
		enviarPcb(socket_kernel,PCB);
		printf("Envio pcb devuelta al kernel \n");
		return NULL;
	}
}
return NULL;
}

void decode(instruccion* instruccion,pcb* PCB){//IDENTIFICA EL TIPO DE INSTRUCCION Y LO ENVIA A SU EXECUTE CORRESPONDIENTE

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
		case READ:
			ejecutarREAD(instruccion->parametro1,PCB);
			break;
		case WRITE:
			ejecutarWRITE(instruccion->parametro1,instruccion->parametro2,PCB);
			break;
		case COPY:
			ejecutarCOPY(instruccion->parametro1,instruccion->parametro2,PCB);
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

void ejecutarREAD(uint32_t dirLogica,pcb* pcb){
	uint32_t dir_fisica=traducir_dir_logica(pcb->valor_tabla_paginas,dirLogica);
	t_paquete* paquete=crear_paquete();
	paquete->codigo_operacion=INSTRUCCION_MEMORIA;
	agregar_entero_a_paquete(paquete,READ);
	agregar_entero_a_paquete(paquete,dir_fisica);
	enviar_paquete(paquete,socket_memoria);
	free(paquete);
	int i=0;
	int size;
	uint32_t valor_leido;
	while(1){
		int cod_op = recibir_operacion(socket_memoria);
		switch (cod_op){
		case PAQUETE:
			valor_leido=(uint32_t)recibir_stream(&size,socket_memoria);
			printf("Valor leido de memoria: %d",valor_leido);
			break;
		case -1:
		     log_error(logger, "Fallo la comunicacion. Abortando");
		     break;
		default:
		     log_warning(logger, "Operacion desconocida");
		     break;
		}
		if(i==1){
			break;
		}
	}
}

void ejecutarWRITE(uint32_t dirLogica,uint32_t valor,pcb* pcb){
	uint32_t dir_fisica=traducir_dir_logica(pcb->valor_tabla_paginas,dirLogica);
	t_paquete* paquete=crear_paquete();
	paquete->codigo_operacion=INSTRUCCION_MEMORIA;
	agregar_entero_a_paquete(paquete,WRITE);
	agregar_entero_a_paquete(paquete,dir_fisica);
	agregar_entero_a_paquete(paquete,valor);
	enviar_paquete(paquete,socket_memoria);
	free(paquete);
}

void ejecutarCOPY(uint32_t dirLogicaDestino,uint32_t dirLogicaOrigen,pcb* pcb){
	uint32_t dir_fisica_destino=traducir_dir_logica(pcb->valor_tabla_paginas,dirLogicaDestino);
	uint32_t dir_fisica_origen=traducir_dir_logica(pcb->valor_tabla_paginas,dirLogicaOrigen);
	t_paquete* paquete=crear_paquete();
	paquete->codigo_operacion=INSTRUCCION_MEMORIA;
	agregar_entero_a_paquete(paquete,COPY);
	agregar_entero_a_paquete(paquete,dir_fisica_destino);
	agregar_entero_a_paquete(paquete,dir_fisica_origen);
	enviar_paquete(paquete,socket_memoria);
	free(paquete);
}

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
void* interrupt(void* datos){
conexion_t* cpu=datos;
int server_fd = iniciar_servidor(cpu->ip,cpu->puerto);
log_info(logger, "CPU listo para recibir interrupciones");
int cliente_fd = esperar_cliente(server_fd);
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
void* conexion_inicial_memoria(void* datos){
	conexion_t* cpu=datos;
	socket_memoria=crear_conexion(cpu->ip,cpu->puerto);
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
///CARGAR CONFIGURACION A CPU

void cargar_config(){
	config= iniciar_config("/home/utnso/tp-2022-1c-Ubunteam/cpu/Default/cpu.config");

	config_valores_cpu.ip_cpu=config_get_string_value(config,"IP_CPU");
	config_valores_cpu.entradas_tlb=config_get_int_value(config,"ENTRADAS_TLB");
	config_valores_cpu.reemplazo_tlb=config_get_string_value(config,"REEMPLAZO_TLB");
	config_valores_cpu.retardo_NOOP=config_get_int_value(config,"RETARDO_NOOP");
	config_valores_cpu.ip_memoria=config_get_string_value(config,"IP_MEMORIA");
	config_valores_cpu.puerto_memoria=config_get_string_value(config,"PUERTO_MEMORIA");
	config_valores_cpu.puerto_escucha_dispatch=config_get_string_value(config,"PUERTO_ESCUCHA_DISPATCH");
	config_valores_cpu.puerto_escucha_interrupt=config_get_string_value(config,"PUERTO_ESCUCHA_INTERRUPT");//LEE Y GUARDA LA CONFIGURACION DESDE cpu.cfg

}
