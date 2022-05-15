#include <stdlib.h>
#include <stdio.h>
#include <commons/collections/list.h>

int main(int argc, char *argv[])
{
	logger = log_create("log.log", "Servidor CPU", 1, LOG_LEVEL_DEBUG);

	    int server_fd = iniciar_servidor();
	    log_info(logger, "CPU listo para recibir al modulo cliente");
	    int cliente_fd = esperar_cliente(server_fd);

	    t_list *lista = list_create();
	    while (1)
	    {
	        int cod_op = recibir_operacion(cliente_fd);
	        switch (cod_op)
	        {
	        case MENSAJE:
	            recibir_mensaje(cliente_fd);
	            break;
	        case PCB:
	            lista = recibir_paquete(cliente_fd);
	            log_info(logger, "Me llegaron los mensajes:\n");
	            list_iterate(lista, (void *)iterator);
	            break;
	        case -1:
	            log_error(logger, "Fallo la comunicacion. Abortando");
	            return EXIT_FAILURE;
	        default:
	            log_warning(logger, "Operacion desconocida");
	            break;
	        }
	    }
	    return EXIT_SUCCESS;
}

void iterator(char *value)
{
    // printf("Valor: %s\n", value);
    log_info(logger, "%s", value);
}

//void fetch(t_pcb* pcb){
//	t_ins* instruccionProxima = ;
//	instruccionProxima = list_get(pcb->instrucciones,pcb->program_counter);
//	decode(instruccionProxima);
//	pcb->program_counter++;
//	free(instruccionProxima);
//}

//void decode(t_ins* instruccion){

//	if(string_contains(instruccion->tipoInstruccion,"NO_OP")){
//		ejecutarNO_OP();
//	}

//	if(string_contains(instruccion->tipoInstruccion,"I/O")){
//		ejecutarIO(instruccion->primerParametro);
//	}

//	if(string_contains(instruccion->tipoInstruccion,"READ")){
//		ejecutarREAD(instruccion->primerParametro);
//	}

//	if(string_contains(instruccion->tipoInstruccion,"WRITE")){
//		ejecutarWRITE(instruccion->primerParametro,instruccion->segundoParametro);
//	}

//	if(string_contains(instruccion->tipoInstruccion,"COPY")){

//		fetchOperands(instruccion->segundoParametro);
//		ejecutarCOPY(instruccion->primerParametro,instruccion->segundoParametro);
//	}

//	if(string_contains(instruccion->tipoInstruccion,"EXIT")){
//		ejecutarEXIT();
//	}

//}

void ejecutarNO_OP(){}

void ejecutarIO(tiempo){

}

void ejecutarREAD(dirLogica){

}

void ejecutarWRITE(dirLogica,valor){

}

void ejecutarCOPY(dirLogicaDestino,dirLogicaOrigen){

}

void ejecutarEXIT(){

}

