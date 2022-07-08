#include "memoria.h"

int main(void) {

    logger = log_create("log.log", "Servidor Memoria", 1, LOG_LEVEL_DEBUG);

    ///CARGAR LA CONFIGURACION
    cargar_configuracion();
    inicializar_memoria();


    int server_fd = iniciar_servidor(config_valores_memoria.ip_memoria,config_valores_memoria.puerto_escucha);
    log_info(logger, "Memoria lista para recibir al modulo cliente");

    while(atender_clientes_memoria(server_fd));

	return EXIT_SUCCESS;
}


// atender clientes con dos hilos

//int atender_clientes_memoria(int socket_servidor){
//
//	int socket_cpu = esperar_cliente(socket_servidor); // se conecta primero cpu
//	int socket_kernel = esperar_cliente(socket_servidor); // se conecta kernel
//	while(true){
//		pthread_t hilo_cliente_cpu;
//		pthread_t hilo_cliente_kernel;
//		pthread_create(&hilo_cliente_cpu, NULL, (void*) manejo_conexiones, (void *)socket_cpu);
//		pthread_detach(hilo_cliente_cpu);
//		pthread_create(&hilo_cliente_kernel, NULL, (void*) manejo_conexiones, (void *)socket_kernel);
//		pthread_detach(hilo_cliente_kernel);
//		return 1;
//	}
//	return 0;
//}

// atender clientes con un hilo

int atender_clientes_memoria(int socket_servidor){

	int socket_cliente = esperar_cliente(socket_servidor); // se conecta primero cpu

	while(true){
		pthread_t hilo_cliente;
		pthread_create(&hilo_cliente, NULL, (void*) manejo_conexiones, (void *)socket_cliente);
		pthread_detach(hilo_cliente);
		return 1;
	}
	return 0;
}

// Manejo conexiones - Procesar conexiones con los op code

void manejo_conexiones(int socket_cliente){

	int codigo_operacion = recibir_operacion_nuevo(socket_cliente);
	t_list* valores;
	uint32_t tabla,entrada1,entrada2,marco;

	switch(codigo_operacion){
	case HANDSHAKE:
		log_info(logger,"me llego el handshake de cpu");
		t_paquete* handshake=preparar_paquete_para_handshake();
		enviar_paquete(handshake,socket_cliente);
		eliminar_paquete(handshake);
		break;
	case INSTRUCCION_MEMORIA:
		log_info(logger,"me llego una instruccion de cpu");
		t_list* datos = recibirPaquete(socket_cliente);
		manejo_instrucciones(datos,socket_cliente);
		list_destroy(datos);
		break;
	case TABLA:
		log_info(logger,"me llego un pedido de entrada a segunda tabla de cpu (mmu)");
		valores=recibir_paquete(socket_cliente);
		tabla=*(uint32_t*)list_get(valores,0);
		entrada1=*(uint32_t*)list_get(valores,1);
		entrada2 = devolver_entrada_a_segunda_tabla(tabla, entrada1);
		t_paquete* paquete_tabla= crear_paquete();
		agregar_a_paquete(paquete_tabla,&entrada2,sizeof(uint32_t));
		enviar_paquete(paquete_tabla,socket_cliente);
		list_destroy(valores);
		eliminar_paquete(paquete_tabla);
		break;
	case MARCO:
		log_info(logger,"me llego un pedido de marco de cpu (mmu)");
		valores=recibir_paquete(socket_cliente);
		tabla=*(uint32_t*)list_get(valores,0);
		entrada2=*(uint32_t*)list_get(valores,1);
		marco= devolver_marco(tabla, entrada2);
		t_paquete* paquete_marco= crear_paquete();
		agregar_a_paquete(paquete_marco,&marco,sizeof(uint32_t));
		enviar_paquete(paquete_marco,socket_cliente);
		list_destroy(valores);
		eliminar_paquete(paquete_marco);
		// falta algoritmo
		break;
	case INICIALIZAR_ESTRUCTURAS:
		log_info(logger, "Inicializando estructuras");
		//Recibe el pcb del proceso para iniciar estructuras
		pcb* pcb_recibido=recibirPcb(socket_cliente);

		//Averiguamos cuantas pags ocupa el proceso
		int cantidad_de_pags=pags_proceso(pcb_recibido->tamanio_proceso,config_valores_memoria.tam_pagina);

		//Averigua cuantas tablas de segundo significa esa cantidad de paginas
		uint32_t cantidad_de_tp2=(uint32_t)tp2_proceso(cantidad_de_pags,config_valores_memoria.entradas_por_tabla);

		//Veo cual es el tamanio de la pag de 1 nivel (que tambien nos sirve para averiguar el proximo indice a usar)
		uint32_t valorTP1=*(uint32_t*)list_size(tabla_de_pagina_1_nivel); // aca no calcula el TAMANIO de la tp1 en vez de la PAGINA??

		//No pueden haber mas entradas q las permitidas
		if(valorTP1+cantidad_de_tp2-1>config_valores_memoria.entradas_por_tabla){
			log_info(logger,"Mayor cantidad de entradas en tabla de primer nivel que las permitidas");
			exit(34);
		}

		//Cargo el num de tabla de la primera pag de 2 nivel del proceso ya que las demas seran contiguas
		pcb_recibido->valor_tabla_paginas=valorTP1;

		//crear tabla de segundo nivel, pasar su numero de tabla a la de primer nivel
		for(uint32_t i=0;i<cantidad_de_tp2;i++){
			tabla_de_segundo_nivel* nueva_tabla = malloc(sizeof(tabla_de_segundo_nivel));
			nueva_tabla->id_tabla = indice_de_tabla2;
			nueva_tabla->lista_paginas = inicializar_tabla_segundo_nivel();
			t_p_1* entrada_en_tp1=malloc(sizeof(t_p_1));
			entrada_en_tp1->indice=valorTP1+i;
			entrada_en_tp1->numero_de_tabla2=indice_de_tabla2;
			list_add(tabla_de_pagina_1_nivel,entrada_en_tp1);
			list_add(lista_tablas_segundo_nivel,nueva_tabla);
			indice_de_tabla2++;
		}
		//crea el swap
		crearSwap(pcb_recibido->id_proceso);

		//Envia el num de tabla de la primera pag de 2 nivel del proceso
		t_paquete* paquete_ini = crear_paquete();
		agregar_a_paquete(paquete_ini,&valorTP1,sizeof(uint32_t));
		enviar_paquete(paquete_ini,socket_cliente);
		free(paquete_ini);
		break;
	case LIBERAR_ESTRUCTURAS: // finalizar proceso
		pcb* pcb=recibirPcb(socket_cliente);
		//liberar los marcos q ocupaba el proceso
		t_list* paginas_proceso = paginas_por_proceso(pcb->id_proceso);
		t_list* paginas_en_memoria = list_filter(paginas_proceso,pagina_con_presencia);
		t_p_2* aux;
		for (int i = 0; i < list_size(paginas_en_memoria); i++){
				aux = list_get(paginas_en_memoria,i);
				liberar_marco(pcb->id_proceso,aux);
			}
		// eliminar swap - poner funcion
		break;
	case SUSPENDER_PROCESO:
		log_info(logger,"me llego mensaje para supender proceso");
		op_code codigo = ESPACIO_PCB_LIBERADO;
		suspender_proceso(socket_cliente); //liberar espacio en memoria del proceso, escribiendo en SWAP la pagina (de tamaño TAM_PAGINA, que está en el marco que indica la tabla de páginas)
		enviar_datos(socket_cliente, &codigo, sizeof(op_code)) ;
		break;
	default:break;
	}
}

///------------------------------INICIALIZAR MEMORIA----------------------------

void inicializar_memoria(){
	memoria_usuario=malloc(sizeof(config_valores_memoria.tam_memoria));
	tabla_de_pagina_1_nivel=list_create();
	lista_tablas_segundo_nivel = list_create();
	algoritmo_memoria=obtener_algoritmo();
	indice_de_tabla2=0;
	pathSwap=config_valores_memoria.path_swap;
}
void* get_marco(void* memoria,int marco){
	return (memoria+marco*config_valores_memoria.tam_pagina);
}
///
void crearTP2(){

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
	agregar_a_paquete(paquete,&config_valores_memoria.tam_pagina,sizeof(int));
	agregar_a_paquete(paquete,&config_valores_memoria.entradas_por_tabla,sizeof(int));
	return paquete;
}

///------------MANEJO DE INSTRUCCIONES DE MEMORIA---------------
void manejo_instrucciones(t_list* datos,int socket_cpu){
	op_code tipo_instruccion=(op_code)list_get(datos,0);

	switch(tipo_instruccion){
	case READ:
		uint32_t valor_leido;
		uint32_t dir_fisica = (uint32_t)list_get(datos,1);
//		memcpy(&dir_fisica,stream + desplazamiento,sizeof(uint32_t));
		valor_leido = leer(dir_fisica);
		t_paquete* paquete=crear_paquete();
		agregar_entero_a_paquete(paquete,valor_leido);
		enviar_paquete(paquete,socket_cpu);
		break;
	case WRITE:
		uint32_t dir_fisica = (uint32_t)list_get(datos,1);;
		uint32_t valor = (uint32_t)list_get(datos,2);;
//		memcpy(&dir_fisica,stream + desplazamiento,sizeof(uint32_t));
//		desplazamiento+=sizeof(uint32_t);
//		memcpy(&valor,stream + desplazamiento,sizeof(uint32_t));
		escribirEn(dir_fisica,valor);
		break;
	case COPY:
		uint32_t valor_leido;
		uint32_t dir_fisica_destino = (uint32_t)list_get(datos,1);;
		uint32_t dir_fisica_origen = (uint32_t)list_get(datos,2);;
//		memcpy(&dir_fisica_destino,stream + desplazamiento,sizeof(uint32_t));
//		desplazamiento+=sizeof(uint32_t);
//		memcpy(&dir_fisica_origen,stream + desplazamiento,sizeof(uint32_t));
		valor_leido = leer(dir_fisica_origen);
		escribirEn(dir_fisica_destino,valor_leido);
		break;
	default:
		break;
	}
}
void traducir_operandos(void* stream,uint32_t* operando1,uint32_t* operando2){
	memcpy(&operando1,stream,sizeof(uint32_t));
	memcpy(&operando2,stream+sizeof(uint32_t),sizeof(uint32_t));
}
int pags_proceso(uint32_t tamanio_proc,int tamanio_pag){
	int entero=((int)tamanio_proc)/tamanio_pag;
	int resto=((int)tamanio_proc)%tamanio_pag;
	if(resto!=0){
		return entero+1;
	}
	else{
		return entero;
	}
}
int tp2_proceso(int pags,int entradas_tabla){
	int entero=pags/entradas_tabla;
	int resto=pags % entradas_tabla;
	if(resto!=0){
		return entero+1;
	}
	else{
		return entero;
	}
}

uint32_t leer(uint32_t dir_fisica){
	uint32_t marco = (uint32_t) (dir_fisica / config_valores_memoria.tam_pagina);
	tabla_de_segundo_nivel* tabla_donde_leer = (tabla_de_segundo_nivel*) list_find(lista_tablas_segundo_nivel,tiene_mismo_indice);
	t_p_2* indice_segunda_tabla = (t_p_2*) list_find(tabla_donde_leer->lista_paginas,marco);
	// Tengo q completarlo

	return marco ;
}

void escribirEn(uint32_t dir_fisica, uint32_t valor){

}

bool tiene_mismo_indice(tabla_de_segundo_nivel* tabla) {
//	return tabla->id_tabla == pcb->id_proceso;
}

void liberar_marco(uint32_t id_proceso, t_p_2* tp2){

	tp2->marco = NULL ;
	tp2->m = 0 ;
	tp2->p = 0 ;
	tp2->u = 0;

	// aca no se si falta relacionar algo con el id_proceso
}
