#include <cpu.h>


void sighandler(int s) {
	finalizar_cpu();
	exit(0);
}

///SEMAFOROS
pthread_mutex_t pedidofin;

///VARIABLES GLOBALES
int parar_proceso=0;
int ultimo_pid = -1;

int main()
{
	signal(SIGINT, sighandler);
	cpu_logger = log_create("/home/utnso/tp-2022-1c-Ubunteam/cpu/cpu.log", "Servidor CPU", 1, LOG_LEVEL_INFO);
	pcb* pcb_recibido=malloc(sizeof(pcb));
	pthread_mutex_init(&pedidofin,NULL);//INICIA EL MUTEX QUE ENGLOBA A parar_proceso

	///CARGA LA CONFIGURACION DE LA CPU
	cargar_config();
	char* ip=config_valores_cpu.ip_cpu;
	char* puerto_dispatch=config_valores_cpu.puerto_escucha_dispatch;
	char* puerto_interrupt=config_valores_cpu.puerto_escucha_interrupt;
	char* ip_memoria=config_valores_cpu.ip_memoria;
	char* puerto_memoria=config_valores_cpu.puerto_memoria;

	socket_memoria=crear_conexion(ip_memoria,puerto_memoria);
	///HANDSHAKE


	pthread_t conexion_memoria_i;
	conexion_t* datos=malloc(sizeof(conexion_t));
	datos->ip=config_valores_cpu.ip_memoria;
	datos->puerto=config_valores_cpu.puerto_memoria;
	pthread_create(&conexion_memoria_i,NULL,conexion_inicial_memoria,NULL);//INICIA EL HILO DE CONEXION INICIAL CON MEMORIA (HANDSHAKE)
	//pthread_join(conexion_memoria_i,NULL);//ESPERA A RECIBIR EL HANDSHAKE PARA SEGUIR


	//INTERRUPCIONES
	pthread_t manejoInterrupciones;
	conexion_t* conexion_interrupcion=malloc(sizeof(conexion_t));
	conexion_interrupcion->ip=ip;
	conexion_interrupcion->puerto=puerto_interrupt;
	pthread_create(&manejoInterrupciones,NULL,interrupt,conexion_interrupcion);//INICIA EL HILO DE ESCUCHA DE INTERRUPCIONES
	log_info(cpu_logger, "Hilo de interrupciones creado \n");

	///INICIA LA TLB

	crear_tlb();
	log_info(cpu_logger, "Tlb creada \n");

	///CREA LA CONEXION CON EL KERNEL

	int server_fd = iniciar_servidor(ip,puerto_dispatch);
    log_info(cpu_logger, "CPU listo para recibir al modulo cliente \n");
    int cliente_fd = esperar_cliente(server_fd);
    log_info(cpu_logger,"Se conecto un cliente por dispatch\n");


    while (1)
    {
        int cod_op = recibir_operacion_nuevo(cliente_fd);
        switch (cod_op)
        {
        case PCB:
        	parar_proceso=0;//INICIA EL CONTADOR DE PARAR PROCESO
        	pcb_recibido = recibirPcb(cliente_fd);
        	log_info(cpu_logger,"Recibi PCB de Id: %d \n",pcb_recibido->id_proceso);
        	if(ultimo_pid != pcb_recibido->id_proceso && list_size(tlb->lista) != 0){
        		vaciarTlb();
        		log_info(cpu_logger,"Se vacio TLB\n");
        	}
        	ciclo_de_instruccion(pcb_recibido,cliente_fd);//INICIA EL CICLO DE INSTRUCCION
            break;
        case -1:
            log_error(cpu_logger, "Fallo la comunicacion. Abortando \n");
            return EXIT_FAILURE;
        break;
        default:
            log_warning(cpu_logger, "Operacion desconocida \n");
            break;
        }
    }
    return EXIT_SUCCESS;
}

void* ciclo_de_instruccion(pcb* PCB,int socket_kernel){//ESTRUCTURA PRINCIPAL DEL CICLO DE INSTRUCCION
instruccion* instruccionProxima;
parar_proceso=0;

log_info(cpu_logger, "Se creo hilo para recibir interrupciones \n");
while((int)PCB->program_counter <list_size(PCB->instrucciones)){
	instruccionProxima = list_get(PCB->instrucciones,(int)PCB->program_counter);//FETCH
	decode(instruccionProxima,PCB);//DECODE (CON EXECUTE INCLUIDO)
	PCB->program_counter++;//ACTUALIZA EL PCB
	if(checkInterrupt()==1){//SE FIJA QUE NO HAYA PEDIDO DE PARAR EL PROCESO ANTES DE SEGUIR CON EL CICLO DE INSTRUCCION
		enviarPcb(socket_kernel,PCB);
		ultimo_pid = PCB->id_proceso;
		log_info(cpu_logger,"Envio pcb devuelta al kernel \n");
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
usleep(config_valores_cpu.retardo_NOOP* 1000);
log_info(cpu_logger,"Tiempo de retardo del retardo de noop: %d\n",config_valores_cpu.retardo_NOOP);
log_info(cpu_logger, "Se ejecuto instruccion NO-OP \n");
}

void ejecutarIO(int tiempo,pcb* PCB){

PCB->tiempo_de_bloqueo=(double)tiempo;
log_info(cpu_logger,"Variable tiempo que llega por parametro a ejecutarIO es: %d\n", tiempo);
log_info(cpu_logger,"Tiempo de bloqueo del pid[%d] es: %f\n",PCB->id_proceso, PCB->tiempo_de_bloqueo);

PCB->estado_proceso=BLOQUEADO;
pthread_mutex_lock(&pedidofin);
parar_proceso++;
pthread_mutex_unlock(&pedidofin);
log_info(cpu_logger, "Se ejecuto instruccion I/O \n");
}

void ejecutarREAD(uint32_t dirLogica,pcb* pcb){
	uint32_t dir_fisica=traducir_dir_logica(pcb->valor_tabla_paginas,dirLogica);
	log_info(cpu_logger, "Traduccion exitosa \n");
	t_paquete* paquete=crear_paquete();
	codigo_instrucciones codigo = READ;
	paquete->codigo_operacion=INSTRUCCION_MEMORIA;
	agregar_a_paquete(paquete,&codigo,sizeof(codigo));
	agregar_a_paquete(paquete,&dir_fisica,sizeof(uint32_t));
	log_info(cpu_logger,"valor dir_fisica de instruuccion READ %d\n",dir_fisica);
	enviar_paquete(paquete,socket_memoria);
	log_info(cpu_logger, "Pedido de lectura enviado \n");
	eliminar_paquete(paquete);

	int i=0;
	t_list* valores;
	uint32_t valor_leido;
	while(1){
		int cod_op = recibir_operacion_nuevo(socket_memoria);
		switch (cod_op){
		case PAQUETE:
			valores = recibir_paquete(socket_memoria);
			valor_leido= *(uint32_t*)list_get(valores,0);
			log_info(cpu_logger,"Valor leido de memoria: %d \n",valor_leido);
			i++;
			break;
		case -1:
		     log_error(cpu_logger, "Fallo la comunicacion. Abortando \n");
		     break;
		default:
		     log_warning(cpu_logger, "Operacion desconocida \n");
		     break;
		}
		if(i==1){
			break;
		}
	}
}

void ejecutarWRITE(uint32_t dirLogica,uint32_t valor,pcb* pcb){
	uint32_t dir_fisica=traducir_dir_logica(pcb->valor_tabla_paginas,dirLogica);
	log_info(cpu_logger, "Traduccion exitosa \n");
	t_paquete* paquete=crear_paquete();
	codigo_instrucciones codigo = WRITE ;
	log_info(cpu_logger, "paquete creado \n");
	paquete->codigo_operacion=INSTRUCCION_MEMORIA;
	log_info(cpu_logger, "codigo INSTRUCCION asignado a paquete \n");
	agregar_a_paquete(paquete,&codigo,sizeof(codigo));
	log_info(cpu_logger, "codigo WRITE agregado a paquete \n");
	agregar_a_paquete(paquete,&dir_fisica,sizeof(uint32_t));
	log_info(cpu_logger,"valor dir_fisica de instruccion WRITE %d\n",dir_fisica);
	agregar_a_paquete(paquete,&valor,sizeof(uint32_t));
	log_info("valor escritura %d\n",valor);
	log_info(cpu_logger, " paquete antes de enviar \n");
	enviar_paquete(paquete,socket_memoria);
	log_info(cpu_logger, "Pedido de escritura enviado \n");
	eliminar_paquete(paquete);


	//int cod_op = recibir_operacion_nuevo(socket_memoria);
	int cod_op;
	recibir_datos(socket_memoria,&cod_op,sizeof(op_code));
	switch (cod_op){
		case ESCRITURA_OK:
			log_info(cpu_logger, "Se escribio exitosamente en la memoria \n");
			break;
		case ESCRITURA_ERROR:
			log_info(cpu_logger, "Ocurrio un error al escribir en memoria \n");
			break;
		case -1:
			log_error(cpu_logger, "Fallo la comunicacion. Abortando \n");
			break;
		default:
			log_warning(cpu_logger, "Operacion desconocida \n");
			break;
	}


}

void ejecutarCOPY(uint32_t dirLogicaDestino,uint32_t dirLogicaOrigen,pcb* pcb){
	uint32_t dir_fisica_destino=traducir_dir_logica(pcb->valor_tabla_paginas,dirLogicaDestino);
	uint32_t dir_fisica_origen=traducir_dir_logica(pcb->valor_tabla_paginas,dirLogicaOrigen);
	log_info(cpu_logger, "Traducciones exitosas \n");
	t_paquete* paquete=crear_paquete();
	codigo_instrucciones codigo = COPY ;
	paquete->codigo_operacion=INSTRUCCION_MEMORIA;
	agregar_a_paquete(paquete,&codigo,sizeof(codigo));
	agregar_a_paquete(paquete,&dir_fisica_destino,sizeof(uint32_t));
	agregar_a_paquete(paquete,&dir_fisica_origen,sizeof(uint32_t));
	enviar_paquete(paquete,socket_memoria);
	log_info(cpu_logger, "Pedido de copia enviado \n");
	eliminar_paquete(paquete);

	//int cod_op = recibir_operacion_nuevo(socket_memoria);
	int cod_op;
	recibir_datos(socket_memoria,&cod_op,sizeof(op_code));
	switch (cod_op){
		case ESCRITURA_OK:
			log_info(cpu_logger, "Se escribio exitosamente en la memoria \n");
			break;
		case ESCRITURA_ERROR:
			log_info(cpu_logger, "Ocurrio un error al escribir en memoria \n");
			break;
		case -1:
			log_error(cpu_logger, "Fallo la comunicacion. Abortando \n");
			break;
		default:
			log_warning(cpu_logger, "Operacion desconocida \n");
			break;
	}
}

void ejecutarEXIT(pcb* PCB){
PCB->estado_proceso=FINALIZADO;
pthread_mutex_lock(&pedidofin);
parar_proceso++;
pthread_mutex_unlock(&pedidofin);
log_info(cpu_logger, "Se ejecuto instruccion EXIT \n");
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
log_info(cpu_logger, "CPU listo para recibir interrupciones \n");
int cliente_fd = esperar_cliente(server_fd);
log_info(cpu_logger,"Se conecto Kernel al puerto interrupt \n");
while(1){
	int cod_op ;
	recibir_datos(cliente_fd,&cod_op,sizeof(int));
	switch (cod_op){
		case MENSAJE:
			log_info(cpu_logger,"Peticion de interrupcion recibida \n");
			pthread_mutex_lock(&pedidofin);
			parar_proceso++;
			pthread_mutex_unlock(&pedidofin);

		break;
		case -1:
			log_error(cpu_logger, "Fallo la comunicacion. Abortando \n");
			return (void*)(EXIT_FAILURE);
		break;
		default:
			 log_warning(cpu_logger, "Operacion desconocida \n");
		break;
	}
}
return NULL;
}
///CONEXION A MEMORIA: HANDSHAKE
void* conexion_inicial_memoria(){

	pedir_handshake(socket_memoria);
	log_info(cpu_logger, "Pedido de handshake enviado \n");
	int codigo_memoria;
	while(1){
		codigo_memoria=recibir_operacion_nuevo(socket_memoria);
		switch(codigo_memoria){
			case PAQUETE:
				log_info(cpu_logger,"Recibi configuracion por handshake \n");
				configuracion_tabla=recibir_handshake(socket_memoria);
				//log_info(cpu_logger,"Recibi configuracion por handshake \n");
				return NULL;
			break;
			case -1:
				log_error(cpu_logger, "Fallo la comunicacion. Abortando \n");
				return (void*)(EXIT_FAILURE);
			break;
			default:
				 log_warning(cpu_logger, "Operacion desconocida \n");
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

void finalizar_cpu(){
	log_info(cpu_logger,"Finalizando el modulo CPU");
	log_destroy(cpu_logger);
	config_destroy(config);
	liberar_conexion(socket_memoria);
}
