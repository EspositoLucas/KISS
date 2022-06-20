#include "memoria.h"

int main(void) {

    logger = log_create("log.log", "Servidor Memoria", 1, LOG_LEVEL_DEBUG);

    ///CARGAR LA CONFIGURACION
    cargar_configuracion();


    int server_fd = iniciar_servidor(config_valores_memoria.ip_memoria,config_valores_memoria.puerto_escucha);
    log_info(logger, "Memoria lista para recibir al modulo cliente");


	return EXIT_SUCCESS;
}

// Manejo conexiones - Procesar conexiones con los op code

void manejo_conexiones(t_paquete* paquete,int socket_cliente){
	switch(paquete->codigo_operacion){
	case HANDSHAKE:
		log_info(logger,"me llego el handshake");
		t_paquete* handshake=preparar_paquete_para_handshake();
		enviar_paquete(handshake,socket_cliente);
		free(handshake);
		break;
	case INSTRUCCION_MEMORIA:
		manejo_instrucciones(paquete->buffer->stream,socket_cliente);
		break;
	case TABLA:
		break;

	default:break;
	}
}
///INIIALIZAR MEMORIA
void inicilizar_memoria(){
	memoria_usuario=malloc(config_valores_memoria.tam_memoria);
}





///--------------CARGA DE CONFIGURACION----------------------
void cargar_configuracion(){
	t_config* config=iniciar_config("cfg/memoria.config");

	config_valores_memoria.ip_memoria=config_get_string_value(config,"IP_MEMORIA");
	config_valores_memoria.puerto_escucha=config_get_string_value(config,"PUERTO_ESCUCHA");
	config_valores_memoria.tam_memoria=config_get_int_value(config,"TAM_MEMORIA");
	config_valores_memoria.tam_pagina=config_get_int_value(config,"TAM_PAGINA");
	config_valores_memoria.entradas_por_tabla=config_get_int_value(config,"ENTRADAS_POR_TABLA");
	config_valores_memoria.retardo_memoria=config_get_int_value(config,"RETARDO_MEMORIA");
	config_valores_memoria.algoritmo_reemplazo=config_get_string_value(config,"ALGORITMO_REEMPLAZO");
	config_valores_memoria.marcos_por_proceso=config_get_int_value(config,"MARCOS_POR_PROCESO");
	config_valores_memoria.retardo_swap=config_get_int_value(config,"RETARDO_SWAP");
	config_valores_memoria.path_swap=config_get_string_value(config,"PATH_SWAP");

	config_destroy(config);
}

t_paquete* preparar_paquete_para_handshake(){
	t_paquete* paquete=crear_paquete();
	agregar_entero_a_paquete(paquete,config_valores_memoria.tam_pagina);
	agregar_entero_a_paquete(paquete,config_valores_memoria.entradas_por_tabla);
	return paquete;
}

///------------MANEJO DE INSTRUCCIONES DE MEMORIA---------------
void manejo_instrucciones(void* stream,int socket_cpu){
	op_code tipo_instruccion;
	int desplazamiento=0;

	memcpy(&tipo_instruccion,stream,sizeof(op_code));
	desplazamiento+=sizeof(op_code);

	switch(tipo_instruccion){
	case READ:
//		uint32_t dir_fisica;
//		memcpy(&dir_fisica,stream + desplazamiento,sizeof(uint32_t));
		//EJECUTAR READ

		break;
	case WRITE:
//		uint32_t dir_fisica;
//		uint32_t valor;
//		memcpy(&dir_fisica,stream + desplazamiento,sizeof(uint32_t));
//		desplazamiento+=sizeof(uint32_t);
//		memcpy(&valor,stream + desplazamiento,sizeof(uint32_t));
		//EJECUTAR WRITE

		break;
	case COPY:
//		uint32_t dir_fisica_origen;
//		uint32_t dir_fisica_destino;
//		memcpy(&dir_fisica_destino,stream + desplazamiento,sizeof(uint32_t));
//		desplazamiento+=sizeof(uint32_t);
//		memcpy(&dir_fisica_origen,stream + desplazamiento,sizeof(uint32_t));
		//EJECUTAR COPY

		break;
	default:
		exit(3);
		break;
	}
}
