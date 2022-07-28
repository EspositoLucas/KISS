#include "memoria.h"

int comparador;

int main(void) {

    memoria_logger = log_create("/home/utnso/tp-2022-1c-Ubunteam/memoria/memoria.log", "Servidor Memoria", 1, LOG_LEVEL_INFO);


    ///CARGAR LA CONFIGURACION
    cargar_configuracion();


    inicializar_memoria();
    printf("Por iniciar servidor\n");
    int server_fd = iniciar_servidor(config_valores_memoria.ip_memoria,config_valores_memoria.puerto_escucha);
    printf("Servidor creado\n");
    log_info(memoria_logger, "Memoria lista para recibir al modulo cliente \n");

    while(atender_clientes_memoria(server_fd));

	return EXIT_SUCCESS;
}


// atender clientes sin diferenciar tipo cliente

int atender_clientes_memoria(int socket_servidor){

	int socket_cliente = esperar_cliente(socket_servidor); // se conecta primero cpu

	if(socket_cliente != -1){
		log_info(memoria_logger, "Se conecto un cliente \n");
		pthread_t hilo_cliente;
		pthread_create(&hilo_cliente, NULL, (void*) manejo_conexiones, (void *)socket_cliente);
		pthread_detach(hilo_cliente);
		return 1;
	}else {
		log_error(memoria_logger, "Error al escuchar clientes... Finalizando servidor \n"); // log para fallo de comunicaciones
	}
	return 0;
}

/// Manejo conexiones - Procesar conexiones con los op code

void manejo_conexiones(int socket_cliente){

	while(1){

	int codigo_operacion = recibir_operacion_nuevo(socket_cliente);
	t_list* valores;
	uint32_t tabla,entrada1,entrada2,marco;

	switch(codigo_operacion){
	case HANDSHAKE:
		log_info(memoria_logger,"me llego el handshake de cpu \n");
		usleep(config_valores_memoria.retardo_memoria);
		t_paquete* handshake=preparar_paquete_para_handshake();
		enviar_paquete(handshake,socket_cliente);
		log_info(memoria_logger,"hanshake enviado a cpu \n");
		eliminar_paquete(handshake);
		break;
	case INSTRUCCION_MEMORIA:
		log_info(memoria_logger,"me llego una instruccion de cpu \n");
		t_list* datos = recibir_paquete(socket_cliente);
		manejo_instrucciones(datos,socket_cliente);
		log_info(memoria_logger,"Instruccion de cpu ejecutada \n");
		list_destroy(datos);
		break;
	case TABLA:
		log_info(memoria_logger,"me llego un pedido de entrada a segunda tabla de cpu (mmu) \n");
		valores=recibir_paquete(socket_cliente);
		tabla=*(uint32_t*)list_get(valores,0);
		entrada1=*(uint32_t*)list_get(valores,1);
		entrada2 = devolver_entrada_a_segunda_tabla(tabla, entrada1);
		usleep(config_valores_memoria.retardo_memoria);
//		t_paquete* paquete_tabla= crear_paquete();
//		agregar_a_paquete(paquete_tabla,&entrada2,sizeof(uint32_t));
//		enviar_paquete(paquete_tabla,socket_cliente);
		enviar_datos(socket_cliente,&entrada2,sizeof(uint32_t));
		log_info(memoria_logger,"entrada segundo nivel enviado a cpu \n");
		list_destroy(valores);
//		eliminar_paquete(paquete_tabla);
		break;
	case MARCO:
		log_info(memoria_logger,"me llego un pedido de marco de cpu (mmu) \n");
		valores=recibir_paquete(socket_cliente);
		log_info(memoria_logger,"paquete recibido (mmu) \n");
		tabla=*(uint32_t*)list_get(valores,0);
		log_info(memoria_logger,"valor tabla (mmu) \n");
		entrada2=*(uint32_t*)list_get(valores,1);
		log_info(memoria_logger,"valor entrada2 (mmu) \n");
		marco= devolver_marco(tabla, entrada2);
		printf("valor marco de devolver marco %d \n",marco);
		log_info(memoria_logger,"valor marco (mmu) \n");
		usleep(config_valores_memoria.retardo_memoria);
		t_paquete* paquete_marco= crear_paquete();
		paquete_marco->codigo_operacion = MARCO;
		agregar_a_paquete(paquete_marco,&marco,sizeof(uint32_t));
		enviar_paquete(paquete_marco,socket_cliente);
		log_info(memoria_logger,"paquete marco(mmu) \n");
		log_info(memoria_logger,"marco enviado a cpu \n");
		list_destroy(valores);
		eliminar_paquete(paquete_marco);
		break;
	case INICIALIZAR_ESTRUCTURAS:
		log_info(memoria_logger, "Inicializando estructuras  \n");
		//Recibe el pcb del proceso para iniciar estructuras
		pcb* pcb_recibido=recibirPcb(socket_cliente);
		log_info(memoria_logger, "recibi pcb de id %d desde kernel para inicializar estructuras \n",pcb_recibido->id_proceso);
		printf("valor tamanio_proceso %"PRIu32"\n",pcb_recibido->tamanio_proceso);
		//Averiguamos cuantas pags ocupa el proceso
		int cantidad_de_pags=pags_proceso(pcb_recibido->tamanio_proceso,config_valores_memoria.tam_pagina);

		//Averigua cuantas tablas de segundo significa esa cantidad de paginas
		uint32_t cantidad_de_tp2=(uint32_t)tp2_proceso(cantidad_de_pags,config_valores_memoria.entradas_por_tabla);

		//Veo cual es el tamanio de la pag de 1 nivel (que tambien nos sirve para averiguar el proximo indice a usar)
		uint32_t valorTP1=(uint32_t)list_size(lista_tabla_de_pagina_1_nivel); // aca no calcula el TAMANIO de la tp1 en vez de la PAGINA??
		//No pueden haber mas entradas q las permitidas
//		if(valorTP1+cantidad_de_tp2-1>config_valores_memoria.entradas_por_tabla){
//			log_error(memoria_logger,"Mayor cantidad de entradas en tabla de primer nivel que las permitidas \n");
//			exit(34);
//		}

		//Cargo el num de tabla de la primera pag de 2 nivel del proceso ya que las demas seran contiguas
		pcb_recibido->valor_tabla_paginas=valorTP1;

		t_list* tabla_primer_nivel = list_create();
		//crear tabla de segundo nivel, pasar su numero de tabla a la de primer nivel
		for(uint32_t i=0;i<cantidad_de_tp2;i++){
			tabla_de_segundo_nivel* nueva_tabla = malloc(sizeof(tabla_de_segundo_nivel));
			nueva_tabla->id_tabla = indice_de_tabla2;
			nueva_tabla->lista_paginas = inicializar_tabla_segundo_nivel();
			nueva_tabla->p_id = pcb_recibido->id_proceso;
			log_info(memoria_logger,"tabla segundo nivel del proceso %d creada \n",pcb_recibido->id_proceso);
			t_p_1* entrada_en_tp1=malloc(sizeof(t_p_1));
			entrada_en_tp1->indice=i;
			entrada_en_tp1->numero_de_tabla2=indice_de_tabla2;
			list_add(tabla_primer_nivel,entrada_en_tp1);
			pthread_mutex_lock(&mutex_tabla_pagina_segundo_nivel);
			list_add(lista_tablas_segundo_nivel,nueva_tabla);
			pthread_mutex_unlock(&mutex_tabla_pagina_segundo_nivel);
			log_info(memoria_logger,"numero de tabla de segundo nivel pasado a la de primer nivel \n");
			indice_de_tabla2++;
		}

		pthread_mutex_lock(&mutex_tabla_pagina_primer_nivel);
		list_add(lista_tabla_de_pagina_1_nivel,tabla_primer_nivel);
		pthread_mutex_unlock(&mutex_tabla_pagina_primer_nivel);
		contador_por_pid* contador = malloc(sizeof(contador_por_pid));
		contador->contadorPF = 0;
		contador->contadorAccesoSwap =0;
		pthread_mutex_lock(&mutex_contador_pid);
		list_add_in_index(contador_pid,pcb_recibido->id_proceso,contador);
		pthread_mutex_unlock(&mutex_contador_pid);
		//crea el swap
		crearSwap(pcb_recibido->id_proceso,pcb_recibido->tamanio_proceso);
		log_info(memoria_logger,"Swap creado \n");
		printf("valor tp1 %"PRIu32" \n",valorTP1);
		enviar_datos(socket_cliente,&valorTP1,sizeof(uint32_t));
		printf("valor socket %d \n",socket_cliente);
		log_info(memoria_logger,"Numero tabla de paginas de 2 nivel del proceso enviado a kernel \n");
		break;
	case PCB: ; // finalizar proceso
		log_info(memoria_logger, "Me llego pedido liberar estructuras del proceso \n");
		op_code codigo_pcb = ESTRUCTURAS_LIBERADAS;
		pcb* pcb=recibirPcb(socket_cliente);
		liberarTodosLosMarcos(pcb->id_proceso);
		log_info(memoria_logger,"Se liberaron los marcos del proceso\n");
		// eliminar swap - poner funcion
		eliminarSwap(pcb);
		liberarMemoriaUsuario(pcb->id_proceso);
		enviar_datos(socket_cliente, &codigo_pcb, sizeof(op_code)) ;
		log_info(memoria_logger,"Se elimino swap y liberaron las estructuras del proceso \n");
		log_info(memoria_logger,"Contador Page Fault del proceso: %d \n",contador->contadorPF);
		log_info(memoria_logger,"Contador Acceso a Swap del proceso: %d \n",contador->contadorAccesoSwap);
		break;
	case SUSPENDER_PROCESO:
		log_info(memoria_logger,"me llego mensaje para suspender proceso \n");
		op_code codigo = ESPACIO_PCB_LIBERADO;
		suspender_proceso(socket_cliente); //liberar espacio en memoria del proceso, escribiendo en SWAP la pagina (de tamaño TAM_PAGINA, que está en el marco que indica la tabla de páginas)
		enviar_datos(socket_cliente, &codigo, sizeof(op_code)) ;
		log_info(memoria_logger,"Proceso suspendido \n");
		break;
	default:break;
	}

}
}

///------------------------------INICIALIZAR MEMORIA----------------------------

void inicializar_memoria(){
	memoria_usuario=malloc(config_valores_memoria.tam_memoria);
	lista_tabla_de_pagina_1_nivel=list_create();
	lista_tablas_segundo_nivel = list_create();
	inicializar_marcos();
	algoritmo_memoria=obtener_algoritmo();
	pathSwap = config_valores_memoria.path_swap;
	indice_de_tabla2=0;
	archivos= list_create();
	contador_pid = list_create();
	pthread_mutex_init(&mutex_comparador_pid,NULL);
	pthread_mutex_init(&mutex_comparador_indice,NULL);
	pthread_mutex_init(&mutex_comparador,NULL);
	pthread_mutex_init(&mutex_marcos,NULL);
	pthread_mutex_init(&mutex_memoria_usuario,NULL);
	pthread_mutex_init(&mutex_comparador_archivo_pid,NULL);
	pthread_mutex_init(&mutex_archivo_swap,NULL);
	pthread_mutex_init(&mutex_tabla_pagina_primer_nivel,NULL);
	pthread_mutex_init(&mutex_tabla_pagina_segundo_nivel,NULL);
	pthread_mutex_init(&mutex_lista_archivo,NULL);
	pthread_mutex_init(&mutex_contador_pid,NULL);

	log_info(memoria_logger, "Memoria inicializada \n");
}
int get_marco(int marco){
	return marco*config_valores_memoria.tam_pagina;
}
void escribirPagEnMemoria(void* pagina,uint32_t numMarco){
	pthread_mutex_lock(&mutex_memoria_usuario);
	memcpy(memoria_usuario + get_marco(numMarco),pagina,config_valores_memoria.tam_pagina);
	pthread_mutex_unlock(&mutex_memoria_usuario);
}


uint32_t escribirModificaciones(uint32_t numPagina,uint32_t pid){

	//uint32_t indice_comparador ;
	bool pagina_con_igual_indice(t_p_2* pagina) {

			return pagina->indice == numPagina;
	        }

	t_list* pagsEnMemoria=paginasEnMemoria(pid);
	printf("pags en memoria  %d \n",list_size( pagsEnMemoria));
	//t_p_2* pagElegida=(t_p_2*)list_get(pagsEnMemoria,numPagina);
	t_p_2* pagElegida=(t_p_2*)list_find(pagsEnMemoria,pagina_con_igual_indice);
	if(pagElegida->m){
		pthread_mutex_lock(&mutex_contador_pid);
		contador_por_pid* contador  = (contador_por_pid*)list_get(contador_pid,pid);
		pthread_mutex_unlock(&mutex_contador_pid);
		contador->contadorAccesoSwap +=1 ;
		asignarAlArchivo(pid);
		log_info(memoria_logger,"Se asigno al archivo swap el pid  \n");
		escribirPagEnSwap(pagElegida);
		log_info(memoria_logger,"Se escribio pagina en swap \n");
		msync(archivo_swap,pid,MS_SYNC);
	}
	cambiarPdePagina(numPagina,pid,0);
	cambiarMdePagina(numPagina,pid,0);
	return pagElegida->marco;
}
///---------------------MODIFICAR TABLA DE PAGINAS---------------------------------
void cambiarPdePagina(uint32_t numPagina,uint32_t pid,bool algo){
	uint32_t numeroPagEnTabla=(numPagina)%((uint32_t)config_valores_memoria.entradas_por_tabla);
	uint32_t numTabla=numPagina/config_valores_memoria.entradas_por_tabla;
	pthread_mutex_lock(&mutex_comparador_pid);
	pid_comparador=pid;
	pthread_mutex_unlock(&mutex_comparador_pid);
	t_list* tablas=(t_list*)list_filter(lista_tablas_segundo_nivel,pagConIgualPid);
	printf("tamanio tablas despues filtrar %d \n", list_size(tablas));

	tabla_de_segundo_nivel* tablinha=(tabla_de_segundo_nivel*)list_get(tablas,numTabla);
	t_p_2* pagina=(t_p_2*)list_get(tablinha->lista_paginas,numeroPagEnTabla);
	pagina->p=algo;
	printf(" bit presencia pagina reemplazada  %d \n",pagina->p);
}


void cambiarUdePagina(uint32_t numPagina,uint32_t pid,bool algo){
	uint32_t numeroPagEnTabla=(numPagina)%((uint32_t)config_valores_memoria.entradas_por_tabla);
	uint32_t numTabla=numPagina/config_valores_memoria.entradas_por_tabla;
	pthread_mutex_lock(&mutex_comparador_pid);
	pid_comparador=pid;
	pthread_mutex_unlock(&mutex_comparador_pid);
	t_list* tablas=(t_list*)list_filter(lista_tablas_segundo_nivel,pagConIgualPid);
	tabla_de_segundo_nivel* tablinha=(tabla_de_segundo_nivel*)list_get(tablas,numTabla);
	t_p_2* pagina=(t_p_2*)list_get(tablinha->lista_paginas,numeroPagEnTabla);
	pthread_mutex_lock(&mutex_tabla_pagina_segundo_nivel);
	pagina->u=algo;
	pthread_mutex_unlock(&mutex_tabla_pagina_segundo_nivel);
	printf(" bit uso %d de pagina de tabla global %d dentro algoritmo  \n",pagina->u,pagina->indice);
}
void cambiarMdePagina(uint32_t numPagina,uint32_t pid,bool algo){
	uint32_t numeroPagEnTabla=(numPagina)%((uint32_t)config_valores_memoria.entradas_por_tabla);
	uint32_t numTabla=numPagina/config_valores_memoria.entradas_por_tabla;
	pthread_mutex_lock(&mutex_comparador_pid);
	pid_comparador=pid;
	pthread_mutex_unlock(&mutex_comparador_pid);
	t_list* tablas=(t_list*)list_filter(lista_tablas_segundo_nivel,pagConIgualPid);

	tabla_de_segundo_nivel* tablinha=(tabla_de_segundo_nivel*)list_get(tablas,numTabla);
	t_p_2* pagina=(t_p_2*)list_get(tablinha->lista_paginas,numeroPagEnTabla);
	pthread_mutex_lock(&mutex_tabla_pagina_segundo_nivel);
	pagina->m=algo;
	pthread_mutex_unlock(&mutex_tabla_pagina_segundo_nivel);
	printf(" bit modificado %d de pagina de tabla global %d dentro algoritmo  \n",pagina->m,pagina->indice);
}
void cambiarPunterodePagina(uint32_t numPagina,uint32_t pid,bool algo){

	uint32_t numeroPagEnTabla=(numPagina)%((uint32_t)config_valores_memoria.entradas_por_tabla);
	uint32_t numTabla=numPagina/config_valores_memoria.entradas_por_tabla;
	pthread_mutex_lock(&mutex_comparador_pid);
	pid_comparador=pid;
	pthread_mutex_unlock(&mutex_comparador_pid);

	t_list* tablas=(t_list*)list_filter(lista_tablas_segundo_nivel,pagConIgualPid);
	tabla_de_segundo_nivel* tablinha=(tabla_de_segundo_nivel*)list_get(tablas,numTabla);
	t_p_2* pagina=(t_p_2*)list_get(tablinha->lista_paginas,numeroPagEnTabla);
	pthread_mutex_lock(&mutex_tabla_pagina_segundo_nivel);
	pagina->puntero_indice=algo;
	pthread_mutex_unlock(&mutex_tabla_pagina_segundo_nivel);
	printf(" bit puntero %d de pagina de tabla global de tabla global %d dentro algoritmo  \n",pagina->puntero_indice,pagina->indice);
}
///--------------CARGA DE CONFIGURACION----------------------
void cargar_configuracion(){
	t_config* config=iniciar_config("/home/utnso/tp-2022-1c-Ubunteam/memoria/Default/memoria.config");
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
	codigo_instrucciones tipo_instruccion = *(codigo_instrucciones*)list_get(datos,0);
	uint32_t dir_fisica;
	uint32_t valor_leido;
	uint32_t valor_escritura;
	op_code codigo;
	int escritura;

	switch(tipo_instruccion){
	case READ: ;
		dir_fisica = *(uint32_t*)list_get(datos,1);

		valor_leido = leer_de_memoria(dir_fisica);

		usleep(config_valores_memoria.retardo_memoria); // retardo memoria antes de responder a cpu
		t_paquete* paquete=crear_paquete();
		agregar_a_paquete(paquete,&valor_leido,sizeof(uint32_t));
		enviar_paquete(paquete,socket_cpu);
		printf("valor leido de memoria %d \n ",valor_leido);
		log_info(memoria_logger,"Valor leido enviado a CPU \n");
		break;
	case WRITE: ;
		dir_fisica = *(uint32_t*)list_get(datos,1);
		printf("valor dir fisica WRITE %d\n",dir_fisica);
		valor_escritura = *(uint32_t*)list_get(datos,2);
		printf("valor antes de escribir en dir fisica %d\n",valor_escritura);
		escritura = escribirEn(dir_fisica, valor_escritura);
		codigo = codigoEscritura(escritura);
		usleep(config_valores_memoria.retardo_memoria);
		enviar_datos(socket_cpu, &codigo, sizeof(op_code)) ;
		log_info(memoria_logger,"Estado de escritura enviado a CPU \n");
		break;
	case COPY: ;
		uint32_t dir_fisica_destino = *(uint32_t*)list_get(datos,1);
		printf("valor dir fisica destino %d \n ",dir_fisica_destino);
		uint32_t dir_fisica_origen = *(uint32_t*)list_get(datos,2);
		printf("valor dir fisica origen %d \n ",dir_fisica_origen);

		valor_leido = leer_de_memoria(dir_fisica_origen);
		printf("valor leido de memoria %d \n ",valor_leido);
		escritura = escribirEn(dir_fisica_destino, valor_leido);
		codigo = codigoEscritura(escritura);
		usleep(config_valores_memoria.retardo_memoria);
		enviar_datos(socket_cpu, &codigo, sizeof(op_code)) ;
		log_info(memoria_logger,"Estado de escritura enviado a CPU \n");
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
	//printf("valor tamanio_proc %d \n",(int)tamanio_proc);
	//printf("valor tamanio_pag %d \n",tamanio_pag);
	double entero=((double)tamanio_proc)/(double)tamanio_pag;
	if((tamanio_proc % tamanio_pag) > 0){
		entero++;

	}
	//printf("valor entero %0.2f \n",ceil(entero));
	return (int)ceil(entero);
}
int tp2_proceso(int pags,int entradas_tabla){
	double entero=(double)pags/(double)entradas_tabla;
	//printf("valor entero %0.2f \n",ceil(entero));
	return (int)ceil(entero);
}

///-----------MANEJO DE MARCOS----------------

//Cree una estructura llamada marquito, que tiene numero de marco y el pid del proceso que lo ocupa
//Si marco=-1 => libre SINO esta ocupado

//Calcula la cantidad de marcos que hay en memoria

double marcosTotales(){
	return (double)(config_valores_memoria.tam_memoria)/(double)(config_valores_memoria.tam_pagina);
}

//Inicializa la lista de marcos

void inicializar_marcos(){
	marcos=list_create();
	for(int i=0;i<marcosTotales();i++){
		marquito* entradaMarco=malloc(sizeof(marquito));
		entradaMarco->pid=-1;
		entradaMarco->numero_de_marco=i;
		list_add(marcos,entradaMarco);
	}
}

//Devuelve la lista de marcos que pertenezcan al proceso

t_list* marcosPid(uint32_t pid){
	pthread_mutex_lock(&mutex_marcos);
	t_list* marc=(t_list*)list_filter(marcos,igualPid);
	pthread_mutex_unlock(&mutex_marcos);
	return marc;
}

//Devuelve la cantidad de marcos que usa un proceso

int cantidadUsadaMarcos(uint32_t pid){
	pthread_mutex_lock(&mutex_comparador);
	comparador=(int)pid;
	pthread_mutex_unlock(&mutex_comparador);
	t_list* marcos_de_proceso=marcosPid(pid);
	return list_size(marcos_de_proceso);
}

//funcion auxiliar
bool igualPid(marquito* marquinhos )
{
	return marquinhos->pid==comparador;
}

//funcion auxiliar

bool estaLibre(marquito* marquinhos){
	return marquinhos->pid==-1;
}

//Ocupa en la lista de marcos el primero que esta libre y devuelve el numero de marco de este

int ocuparMarcoLibre(uint32_t pid){
	pthread_mutex_lock(&mutex_marcos);
	marquito* marco_libre=(marquito*)list_find(marcos,estaLibre);
	marco_libre->pid=pid;
	pthread_mutex_unlock(&mutex_marcos);
	return marco_libre->numero_de_marco;
}

//Libera el marco indicado

void liberarMarco(uint32_t marcoALiberar){
	pthread_mutex_lock(&mutex_marcos);
	marquito* marc=(marquito*)list_get(marcos,marcoALiberar);
	marc->pid=-1;
	pthread_mutex_unlock(&mutex_marcos);
}

//Libera todos los marcos ocupados por el proceso

void liberarTodosLosMarcos(uint32_t pid){

	printf("ANTES DE LIBERAR TODOS LOS MARCOS, PRINTEO EL ESTADO FINAL DE LOS MARCOS DEL PROCESO\n");
	t_list* auxiliar = paginasEnMemoria(pid);
	for (int i = 0; i < list_size(auxiliar); i++){
		t_p_2* aux = list_get(auxiliar,i);
		printf("Pagina numero: %d, U: %d, M: %d, Puntero : %d\n", aux->indice, aux->u,aux->m,aux->puntero_indice);
	}

	t_list* marc=marcosPid(pid);
	for(int i=0;i<list_size(marc);i++){
		marquito*marcoALiberar=(marquito*)list_get(marc,i);
		liberarMarco(marcoALiberar->numero_de_marco);
	}
}

uint32_t leer_de_memoria(uint32_t dir_fisica){
	uint32_t valor_leido_memoria = 0;
	pthread_mutex_lock(&mutex_memoria_usuario);
	memcpy(&valor_leido_memoria,memoria_usuario + dir_fisica,sizeof(uint32_t));
	pthread_mutex_unlock(&mutex_memoria_usuario);
	cambiarMarcoUsoAUno(dir_fisica);
	log_info(memoria_logger,"Lectura satisfactoria \n");
	return valor_leido_memoria;
}

int escribirEn(uint32_t dir_fisica, uint32_t valor){
	if(dir_fisica > config_valores_memoria.tam_memoria) {
		log_error(memoria_logger,"Escritura insatisfactoria \n");
		return 0;
	}
	pthread_mutex_lock(&mutex_memoria_usuario);
	memcpy(memoria_usuario + dir_fisica, &valor, sizeof(uint32_t));
	printf("valor escritura %d \n ",valor);
	pthread_mutex_unlock(&mutex_memoria_usuario);
	cambiarMarcoUsoAUno(dir_fisica);
	cambiarMarcoModificadoAUno(dir_fisica);
	log_info(memoria_logger,"Escritura satisfactoria \n");
	return 1;
}

op_code codigoEscritura(int valor) {
	op_code codigo;
	if (valor == 1) {
		codigo = ESCRITURA_OK;
	} else {
		codigo = ESCRITURA_ERROR;
	}
	return codigo;
}

void cambiarMarcoModificadoAUno(uint32_t dir_fisica){

	uint32_t marco = (int)floor((double)dir_fisica / (double)config_valores_memoria.tam_pagina) ;
	bool marcoConMismoIndice(marquito* m){
				return m->numero_de_marco == marco;
			}
	marquito* aux  = (marquito*)list_find(marcos,marcoConMismoIndice);

	t_list* pagsEnM = (t_list*)paginasEnMemoria(aux->pid);

	bool pagConMismoMarco(t_p_2* pagina){

					return pagina->marco == marco;

	}
					if(list_size(pagsEnM) > 0){
							t_p_2* paginaM = (t_p_2*)list_find(pagsEnM,pagConMismoMarco);

								cambiarMdePagina(paginaM->indice,aux->pid,1);
								printf("valor bit modificado despues de escribir en dir fisica %d \n",paginaM->m);
						}



}
void cambiarMarcoUsoAUno(uint32_t dir_fisica){

	uint32_t marco = (int)floor((double)dir_fisica / (double)config_valores_memoria.tam_pagina) ;
	bool marcoConMismoIndice(marquito* m){
				return m->numero_de_marco == marco;
			}
	marquito* aux  = (marquito*)list_find(marcos,marcoConMismoIndice);
	printf("pid marco aux %d \n", aux->pid);

	t_list* pagsEnM = (t_list*)paginasEnMemoria(aux->pid);

	for(int i = 0 ; i < list_size(pagsEnM); i++){
				t_p_2* aux = (t_p_2*)list_get(pagsEnM,i);
				printf("pag en memoria antes de hacer el list find %d \n",aux->indice);
			}
	printf("tamanio pags en Memoria %d \n",list_size(pagsEnM));

	bool pagConMismoMarco(t_p_2* pagina){
					return pagina->marco == marco;
				}

	if(list_size(pagsEnM) > 0){
		t_p_2* paginaM = (t_p_2*)list_find(pagsEnM,pagConMismoMarco);

			cambiarUdePagina(paginaM->indice,aux->pid,1);
	}


}

// liberar memoria usuario

void liberarMemoriaUsuario(uint32_t pid) {
	t_list* pagsEnMemoria = paginasEnMemoria(pid);
	printf("tamanio pags en memoria %d \n",list_size(pagsEnMemoria));

	for(int i =0 ;i<list_size(pagsEnMemoria);i++){
		t_p_2* aux = (t_p_2*)list_get(pagsEnMemoria,i);
		printf("tamanio pags en memoria %d \n",list_size(pagsEnMemoria));
		limpiarMarco(aux->marco);
		printf("tamanio pags en memoria %d \n",list_size(pagsEnMemoria));
	}

}

void limpiarMarco(uint32_t nroMarco){
	   void* buf = malloc(config_valores_memoria.tam_pagina);
	   pthread_mutex_lock(&mutex_memoria_usuario);
	   memcpy(memoria_usuario + config_valores_memoria.tam_pagina * nroMarco, buf, config_valores_memoria.tam_pagina);
	   pthread_mutex_unlock(&mutex_memoria_usuario);
	   free(buf);
}



