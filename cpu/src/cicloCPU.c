#include "cpu.h"

void fetch(pcb* PCB){
	while(1){
		instruccion* instruccionProxima = malloc(sizeof(instruccion));
		instruccionProxima = list_get(PCB->instrucciones,PCB->program_counter);
		decode(instruccionProxima,PCB);
		PCB->program_counter++;
		//interrupt
		free(instruccionProxima);
	}
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
	PCB->program_counter++;
	PCB->tiempo_bloqueado=(double)tiempo;
	//PCB cambia a estado bloqueado
}

/*void ejecutarREAD(dirLogica){

}

void ejecutarWRITE(dirLogica,valor){

}

void ejecutarCOPY(dirLogicaDestino,dirLogicaOrigen){

}*/

void ejecutarEXIT(pcb* PCB){
	PCB->program_counter++;
}



