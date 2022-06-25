#include "memoria.h"

int main(void) {

    logger = log_create("log.log", "Servidor Memoria", 1, LOG_LEVEL_DEBUG);

    ///CARGAR LA CONFIGURACION
    cargar_configuracion();
    inicializar_memoria();


    int server_fd = iniciar_servidor(config_valores_memoria.ip_memoria,config_valores_memoria.puerto_escucha);
    log_info(logger, "Memoria lista para recibir al modulo cliente");

    if(atender_clientes(server_fd, manejo_conexiones) == -1) {
        		log_error(logger, "Error al escuchar clientes... Finalizando servidor");
        	}

	return EXIT_SUCCESS;
}

// Manejo conexiones - Procesar conexiones con los op code

void manejo_conexiones(t_paquete* paquete,int socket_cliente){

	switch(paquete->codigo_operacion){
	case HANDSHAKE:
		log_info(logger,"me llego el handshake de cpu");
		t_paquete* handshake=preparar_paquete_para_handshake();
		enviar_paquete(handshake,socket_cliente);
		free(handshake);
		break;
	case INSTRUCCION_MEMORIA:
		log_info(logger,"me llego una instruccion de cpu");
		manejo_instrucciones(paquete->buffer->stream,socket_cliente);
		break;
	case TABLA:
		log_info(logger,"me llego un pedido de entrada a segunda tabla de cpu (mmu)");
		uint32_t tabla;
		uint32_t entrada1;
		uint32_t entrada2;
		void* stream=recibir_stream();
		traducir_operandos(stream,&tabla,&entrada1);
		entrada2=devolver_entrada_a_segunda_tabla(tabla,entrada1);
		t_paquete* paquete=crear_paquete();
		agregar_a_paquete(paquete,entrada2,sizeof(uint32_t));
		enviar_paquete(paquete,socket_cliente);
		break;
	case MARCO:
		log_info(logger,"me llego un pedido de marco de cpu (mmu)");

		break;
	case INICIALIZAR_ESTRUCTURAS:
		//crear tabla de segundo nivel, pasar su numero de tabla a la de primer nivel y devolver el indice de la de primer nivel a kernel
		tabla_de_segundo_nivel* nueva_tabla=malloc(sizeof(tabla_de_segundo_nivel));
		nueva_tabla->id_tabla=indice_de_tabla;
		indice_de_tabla++;
		nueva_tabla->lista_marcos=inicializar_tabla_segundo_nivel();
		t_p_1* posicion=buscar_posicion_libre();
		posicion->numero_de_tabla2=nueva_tabla->id_tabla;
		t_paquete* paquete=crear_paquete();
		agregar_entero_a_paquete(paquete,nueva_tabla->id_tabla);
		enviar_paquete(paquete,socket_cliente);
		break;
	case LIBERAR_ESTRUCTURAS:
		//liberar los marcos q ocupaba el proceso y el archivo swap (no las tablas de pagina)
	default:break;
	}
}

///------------------------------INICIALIZAR MEMORIA----------------------------

void inicializar_memoria(){
	cantidad_de_marcos=config_valores_memoria.tam_memoria/config_valores_memoria.tam_pagina;
	memoria_usuario=calloc(cantidad_de_marcos,config_valores_memoria.tam_pagina);
	inicializar_tabla_primer_nivel();
}



///--------------CARGA DE CONFIGURACION----------------------
void cargar_configuracion(){
	t_config* config=iniciar_config("Default/memoria.config");

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

}
///----------------PREPARAR PAQUETE PARA HANDSHAKE------------------

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
		break;
	}
}
void traducir_operandos(void* stream,uint32_t* operando1,uint32_t* operando2){
	memcpy(&operando1,stream,sizeof(uint32_t));
	memcpy(&operando1,stream+sizeof(uint32_t),sizeof(uint32_t));
}
