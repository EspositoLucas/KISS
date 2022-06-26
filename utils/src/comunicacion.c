#include "comunicacion.h"

// NO TOCAR FUNCIONES TP0 - SOLO SE AGREGAN NUEVAS FUNCIONES EN CASO DE SER NECESARIO




//----------------------------------ENVIO - RECIBO DE  PAQUETE/BUFFER/MENSAJE/OPERACION------------------------------------

// OPERACION

int recibir_operacion(int socket_cliente) {  // TP0
   int cod_op;
    if(recv(socket_cliente, &cod_op, sizeof(uint8_t), MSG_WAITALL) != 0)
        return cod_op;
    else
    {
        close(socket_cliente);
        return -1;
    }
}

int recibir_operacion_nuevo(int socket_cliente) {
   int cod_op = 0;
    if(recv(socket_cliente, &cod_op, sizeof(uint8_t), MSG_WAITALL) != 0)
        return cod_op;
    else
    {
        close(socket_cliente);
        return -1;
    }
}

// MENSAJE

void enviar_mensaje(char *mensaje, int socket_cliente) //TP0
{
    t_paquete *paquete = malloc(sizeof(t_paquete));

    paquete->codigo_operacion = MENSAJE;
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->stream_size = strlen(mensaje) + 1;
    paquete->buffer->stream = malloc(paquete->buffer->stream_size);
    memcpy(paquete->buffer->stream, mensaje, paquete->buffer->stream_size);

    int bytes = paquete->buffer->stream_size + 2 * sizeof(int);

    void *a_enviar = serializar_paquete_con_bytes(paquete,bytes);

    for(int i = 0 ; i<bytes; i++){
    	printf("%02x ",((char*) a_enviar)[i]);
    }
    send(socket_cliente, a_enviar, bytes, 0);

    free(a_enviar);
    eliminar_paquete(paquete);
}

void recibir_mensaje(int socket_cliente,t_log* logger) { //TP0
    int size;
    char* buffer = recibir_buffer(&size, socket_cliente);
    log_info(logger, "Me llego el mensaje %s", buffer);
    free(buffer);
}


 // BUFFER

void crear_buffer(t_paquete *paquete)
{
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->stream_size = 0;
    paquete->buffer->stream = NULL;
}

void *recibir_stream(int *size, int socket_cliente)
{
    void *stream;

    recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
    stream = malloc(*size);
    recv(socket_cliente, stream, *size, MSG_WAITALL);

    return stream;
}


// recibir buffer comun sin tamanio proceso

 void* recibir_buffer(int* size, int socket_cliente) {
     void * buffer;

     recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
     buffer = malloc(*size);
     recv(socket_cliente, buffer, *size, MSG_WAITALL);

     return buffer;
 }



void agregar_a_buffer(t_buffer *buffer, void *src, int size) {
	buffer->stream = realloc(buffer->stream, buffer->stream_size + size);
	memcpy(buffer->stream + buffer->stream_size, src, size);
	buffer->stream_size+=size;
}

t_buffer *inicializar_buffer_con_parametros(uint32_t size, void *stream) {
	t_buffer *buffer = (t_buffer *)malloc(sizeof(t_buffer));
	buffer->stream_size = size;
	buffer->stream = stream;
	return buffer;
}

// PAQUETE


void* serializar_paquete_con_bytes(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->stream_size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->stream_size);
	desplazamiento+= paquete->buffer->stream_size;

	return magic;
}

t_buffer* serializar_paquete(t_paquete* paquete) {
    t_buffer * magic = inicializar_buffer_con_parametros(0, NULL);

    agregar_a_buffer(magic, &(paquete->codigo_operacion), sizeof(uint8_t));
    agregar_a_buffer(magic, &(paquete->buffer->stream_size), sizeof(uint32_t));
    agregar_a_buffer(magic, paquete->buffer->stream, paquete->buffer->stream_size);

    return magic;
}

// paquete con codigo de operacion solo paquete

t_paquete *crear_paquete(void){ // TP0
    t_paquete *paquete = malloc(sizeof(t_paquete));

    paquete->codigo_operacion = PAQUETE;
    crear_buffer(paquete);
    return paquete;
}

//paquete con cualquier codigo de operacion

t_paquete *crear_paquete_con_codigo_de_operacion(uint8_t codigo){
    t_paquete *paquete = malloc(sizeof(t_paquete));

    paquete->codigo_operacion = codigo;
    crear_buffer(paquete);
    return paquete;
}


void agregar_a_paquete(t_paquete *paquete, void *valor, int tamanio_valor) {
    agregar_a_buffer(paquete->buffer, &tamanio_valor, sizeof(int)); // Para que no se envie el valor del tamanio del stream size
    agregar_a_buffer(paquete->buffer, valor, tamanio_valor);
}


void agregar_entero_a_paquete(t_paquete *paquete, int tamanio_proceso) // Agregar un entero a un paquete (ya creado)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->stream_size + sizeof(int));
	int size_tamanio = sizeof(int);
	memcpy(paquete->buffer->stream , &size_tamanio, sizeof(int));
    memcpy(paquete->buffer->stream , &tamanio_proceso, sizeof(int));
    paquete->buffer->stream_size += tamanio_proceso + sizeof(int);
}


void enviar_paquete(t_paquete *paquete, int socket_cliente) //TP0
{
    t_buffer *a_enviar = serializar_paquete(paquete);
    unsigned char* stream = (unsigned char*)(a_enviar->stream);
//    printf("enviar paquete \n");
//    for(int i=0 ; i<a_enviar->stream_size;i++){
//    	printf("%02X ",stream[i]);
//    }
    send(socket_cliente, a_enviar->stream, a_enviar->stream_size, 0);
    free(a_enviar->stream);
    free(a_enviar);
}

// Recibir paquete como lista 

t_list *recibir_paquete(int socket_cliente)  // TP0
{
    int size;
    int desplazamiento = 0;
    void *stream;
    t_list *valores = list_create();
    int tamanio;

    stream = recibir_stream(&size, socket_cliente);

    while (desplazamiento < size)
    {
        memcpy(&tamanio, stream + desplazamiento, sizeof(int));
        desplazamiento += sizeof(int);
        char *valor = malloc(tamanio);
        memcpy(valor, stream + desplazamiento, tamanio);
        desplazamiento += tamanio;
        list_add(valores, valor);
    }
    free(stream);
    return valores;
}

// Recibir paquete como paquete 

t_paquete* recibe_paquete(int socket){
	int size;
	void *stream;

	op_code codigo=recibir_operacion(socket);
	t_paquete* paquete=crear_paquete_con_codigo_de_operacion(codigo);

	stream = recibir_stream(&size, socket);
	unsigned char* otro_stream = (unsigned char*)(stream);
	memcpy(&paquete->buffer->stream_size, &size, sizeof(int));
	printf("%s \n",otro_stream);
	paquete->buffer->stream = malloc(size);
	memcpy(paquete->buffer->stream, stream, size);

	free(stream);
	return paquete;

}

void eliminar_paquete(t_paquete* paquete) {
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);
}


// DATOS


//Envio y recibo de datos

int enviar_datos(int socket_fd, void *source, uint32_t size) {
	return send(socket_fd, source, size, 0);
}

int recibir_datos(int socket_fd, void *dest, uint32_t size) {
	return recv(socket_fd, dest, size, MSG_WAITALL); // cuantos bytes a recibir y a donde los quiero recibir
}


//----------------------------------SERIALIZAR_PCB---------------------------------------

void agregaABuffer(t_buffer *buffer, void *src, int size){
	buffer->stream = realloc(buffer->stream, buffer->stream_size + size);
	memcpy(buffer->stream + buffer->stream_size, src, size);
	buffer->stream_size+=size;
}

void agregaAPaquete(t_paquete* paquete,void* valor,int tamanio){
	agregar_a_buffer(paquete->buffer, &tamanio, sizeof(int));
	agregar_a_buffer(paquete->buffer, valor, tamanio);
}
void armarPaquete(t_paquete* paquete,pcb* pcb){
	agregaAPaquete(paquete,&pcb->estado_proceso,sizeof(estado));
	agregaAPaquete(paquete,&pcb->estimacion_rafaga,sizeof(float));
	agregaAPaquete(paquete,&pcb->id_proceso,sizeof(uint32_t));
	agregaAPaquete(paquete,&pcb->program_counter,sizeof(uint32_t));
	agregaAPaquete(paquete,&pcb->rafaga_anterior,sizeof(uint8_t));
	agregaAPaquete(paquete,&pcb->tamanio_proceso,sizeof(uint32_t));
	agregaAPaquete(paquete,&pcb->tiempo_de_bloqueo,sizeof(double));
	agregaAPaquete(paquete,&pcb->valor_tabla_paginas,sizeof(uint32_t));
	uint32_t cant=(uint32_t)list_size(pcb->instrucciones);
	agregaAPaquete(paquete,&cant,sizeof(int));
	for(int i=0;i<cant;i++){
		instruccion* inst=list_get(pcb->instrucciones,i);
		agregaAPaquete(paquete,&inst->codigo,sizeof(codigo_instrucciones));
		agregaAPaquete(paquete,&inst->parametro1,sizeof(uint32_t));
		agregaAPaquete(paquete,&inst->parametro2,sizeof(uint32_t));
	}
}

void enviarPcb(int socket,pcb* pcb){
	t_paquete* paquete=crear_paquete();
	armarPaquete(paquete,pcb);
	enviar_paquete(paquete,socket);
}

///----------------------------------RECIBIR PCBS----------------------------------

t_list *recibirPaquete(int socket_cliente)
{
    int size;
    int desplazamiento = 0;
    void *stream;
    t_list *valores = list_create();
    int tamanio;

    stream = recibir_stream(&size, socket_cliente);

    while (desplazamiento < size)
    {
        memcpy(&tamanio, stream + desplazamiento, sizeof(int));
        desplazamiento += sizeof(int);
        void *valor = malloc(tamanio);
        memcpy(valor, stream + desplazamiento, tamanio);
        desplazamiento += tamanio;
        list_add(valores, valor);
    }
    free(stream);
    return valores;
}

pcb* recibirPcb(int socket){
	t_list* valores=recibirPaquete(socket);
	pcb* recibido=malloc(sizeof(pcb));
	recibido->instrucciones=list_create();
	int i=0;
	estado* est=list_get(valores,i);i++;
	recibido->estado_proceso=*est;
	float* rafaga=list_get(valores,i);i++;
	recibido->estimacion_rafaga=*rafaga;
	uint32_t* id=(uint32_t)list_get(valores,i);i++;
	recibido->id_proceso=*id;
	uint32_t* pc=(uint32_t)list_get(valores,i);i++;
	recibido->program_counter=*pc;
	uint8_t* rafa=list_get(valores,i);i++;
	recibido->rafaga_anterior=*rafa;
	uint32_t* tamanio=(uint32_t)list_get(valores,i);i++;
	recibido->tamanio_proceso=*tamanio;
	double* bloqueo=list_get(valores,i);i++;
	recibido->tiempo_de_bloqueo=*bloqueo;
	uint32_t* valor_tp=(uint32_t)list_get(valores,i);i++;
	recibido->valor_tabla_paginas=*valor_tp;
	uint32_t* cant=(uint32_t)list_get(valores,i);i++;
	codigo_instrucciones* code;
	uint32_t*para1;
	uint32_t *para2;
	for(uint32_t y=0;y<(*cant);y++){
		instruccion* inst=malloc(sizeof(instruccion));
		code=list_get(valores,i);i++;
		inst->codigo=*code;
		para1=(uint32_t)list_get(valores,i);i++;
		inst->parametro1=*para1;
		para2=(uint32_t)list_get(valores,i);i++;
		inst->parametro2=*para2;

		list_add(recibido->instrucciones,inst);
	}

	list_destroy(valores);

	return recibido;
}




//----------------------------------SERIALIZAR_PCB---------------------------------------

void *serializar_pcb(pcb* pcb)
{
    t_buffer* buffer = malloc(sizeof(t_buffer));
    buffer->stream_size = sizeof(uint32_t) * 4 // Para los unint32
             + sizeof(1) * 1 // Para los int ;
             + sizeof(float) * 1 // Para los float
             + sizeof(uint8_t) * 1 // Para los uint_8_t
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
    memcpy(stream + offset, &pcb->rafaga_anterior, sizeof(uint8_t));
    offset += sizeof(uint8_t);

    //Serializar los campos enum

    memcpy(stream + offset, &pcb->estado_proceso, sizeof(estado));
    offset += sizeof(estado);


    //Serializar lista instrucciones

    int cantidad_instrucciones = list_size(pcb->instrucciones) ;

    memcpy(stream + offset, &cantidad_instrucciones, sizeof(1)); // primero se copia el tamnio de la lista para despues ir agregando los otros campos de la lista
    offset += sizeof(1);

    for (int i = 0; i < cantidad_instrucciones; i++){ // hacemos un char* a la vez

        memcpy(stream + offset, list_get(pcb->instrucciones,i), sizeof(instruccion));
        offset += sizeof(instruccion);
    }

    buffer->stream = stream;


    list_destroy(pcb->instrucciones);


    return buffer ;

}


///----------------------------------DESERIALIZAR PCB ----------------------------------

pcb* deserializar_pcb(void* stream) {

	pcb* pcb=malloc(sizeof(pcb));
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
    memcpy(&(pcb->rafaga_anterior), stream, sizeof(uint8_t));
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
    return pcb;
}


//HANDSHAKE
void pedir_handshake(int socket_memoria){
	t_paquete *paquete = malloc(sizeof(t_paquete));
	char* mensaje_handshake="HANDSHAKE";

	   paquete->codigo_operacion = HANDSHAKE;
	    paquete->buffer = malloc(sizeof(t_buffer));
	    paquete->buffer->stream_size = strlen(mensaje_handshake) + 1;
	    paquete->buffer->stream = malloc(paquete->buffer->stream_size);
	    memcpy(paquete->buffer->stream, mensaje_handshake, paquete->buffer->stream_size);

	    int bytes = paquete->buffer->stream_size + 2 * sizeof(int);

	    void *a_enviar = serializar_paquete(paquete);

	    send(socket_memoria, a_enviar, bytes, 0);

	    free(a_enviar);
	    eliminar_paquete(paquete);
}
t_handshake* recibir_handshake(int socket_memoria){
	t_handshake* han=malloc(sizeof(t_handshake));
	int size;
	void* stream=recibir_stream(&size,socket_memoria);
	memcpy(&(han->tam_pagina),stream,sizeof(int));
	memcpy(&(han->entradas),stream+sizeof(int),sizeof(int));
	free(stream);
	return han;
}
//TABLA DE PAGINA
void pedir_tabla_pagina(int socket,uint32_t tabla,uint32_t entrada){
	t_paquete* paquete=crear_paquete();
	paquete->codigo_operacion=TABLA;
	agregar_a_paquete(paquete,&tabla,sizeof(uint32_t));
	agregar_a_paquete(paquete,&entrada,sizeof(uint32_t));
	enviar_paquete(paquete,socket);
}
//MARCO
void pedir_marco(int socket,uint32_t tabla,uint32_t entrada){
	t_paquete* paquete=crear_paquete();
	paquete->codigo_operacion=MARCO;
	agregar_a_paquete(paquete,&tabla,sizeof(uint32_t));
	agregar_a_paquete(paquete,&entrada,sizeof(uint32_t));
	enviar_paquete(paquete,socket);
}

///----------------------------------ATENDER CLIENTES ----------------------------------

int atender_clientes(int socket_servidor, void (*manejo_conexiones)(t_paquete *,int)) {
	int socket_cliente ;
    while(true) {
    	 socket_cliente = esperar_cliente(socket_servidor);
        if(socket_cliente == -1) {
        	break;
        }
        pthread_t th_cliente;
        t_socket *conexion = crear_socket_conexion(socket_cliente, manejo_conexiones);

	    pthread_create(&th_cliente, NULL, (void *)ejecutar_instruccion, conexion);
	    pthread_detach(th_cliente);
    }

    return socket_cliente;
}

// Para recibir paquete y usarlo al procesar las conexiones

void ejecutar_instruccion(t_socket *conexion) {
	t_paquete *paquete;
	int socket=conexion->socket;

	while(true) {
		paquete = recibe_paquete(socket);
		if(socket == -1) {
	    	break;
	    }
	    conexion->manejo_conexiones(paquete,socket);
	    eliminar_paquete(paquete);
	}

	eliminar_paquete(paquete);
	liberar_conexion(socket);
	free(conexion);
}

 t_socket *crear_socket_conexion(int socket_fd, void (*manejo_conexiones)(t_paquete *,int socket)) {
    t_socket *conexion = malloc(sizeof(t_socket));
    conexion->socket = socket_fd;
    conexion->manejo_conexiones = manejo_conexiones;
    return conexion;
}

 // ---------------------------------ELIMINAR PCB ---------------------------------------

 void eliminar_pcb(pcb* proceso){
 	list_destroy_and_destroy_elements(proceso->instrucciones, free);
 	free(proceso);
 }

