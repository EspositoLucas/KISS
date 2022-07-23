#include "mmu.h"
#include "math.h"
uint32_t traducir_dir_logica(uint32_t primera_tabla, uint32_t direccion_logica){

	//FORMULAS DEL ENUNCIADO

	uint32_t num_pagina =  (uint32_t) floor((double)direccion_logica / (double)configuracion_tabla->tam_pagina); //CASTEO XQ FLOOR DEVUELVE DOUBLE
	uint32_t entrada_tabla_1 =  (uint32_t) floor((double)num_pagina / (double)configuracion_tabla->entradas);
	uint32_t entrada_tabla_2 =  num_pagina % (configuracion_tabla->entradas);
	uint32_t offset =  direccion_logica - num_pagina * configuracion_tabla->tam_pagina ;
	log_info(cpu_logger,"Direccion logica %d : [%d|%d|%d] \n",direccion_logica,entrada_tabla_1,entrada_tabla_2,offset);

	int marco=buscar_en_la_tlb(num_pagina);

	if(marco==-1){ //SITUACION DE TLB MISS
		//USO DE FUNCIONES PARA TENER LOS VALORES
		log_info(cpu_logger,"Antes de entrar a segunda tabla \n");
		uint32_t segunda_tabla=obtener_segunda_tabla(primera_tabla,entrada_tabla_1);
		log_info(cpu_logger,"despues de entrar a segunda tabla \n");
		marco=(int)obtener_marco(segunda_tabla, entrada_tabla_2);
		traduccion_t* trad=malloc(sizeof(traduccion_t));
		trad->marco=(uint32_t)marco;
		trad->pagina=num_pagina;
		tlb_miss(trad);
	}

	//LA SITUACION DE TLB HIT ESTA IMPLEMENTADA ADENTRO DE BUSCAR EN LA TLB

	//CALCULO DE DIRECCION FISICA

	uint32_t direccion_fisica=marco*configuracion_tabla->tam_pagina+offset;
	log_info(cpu_logger,"Direccion fisica traducida: %d \n",direccion_fisica);

	return direccion_fisica;

}

//PRIMER ACCESO A MEMORIA

uint32_t obtener_segunda_tabla(uint32_t primera_tabla, uint32_t entrada_tabla_1){
	log_info(cpu_logger,"antes de pedir tabla \n");
	pedir_tabla_pagina(socket_memoria,primera_tabla,entrada_tabla_1);
	log_info(cpu_logger,"despues de pedir tabla \n");
	uint32_t segunda_tabla;

		while(1) {
		int codigo_op=recibir_operacion_nuevo(socket_memoria);
		printf("op code memoria obtener segunda tabla %d\n",socket_memoria);
		int size;
		t_list* valores;
		switch(codigo_op){
		case PAQUETE:
					log_info(cpu_logger,"a punto de recibir paquete valores \n");
					valores = recibirPaquete(socket_memoria);
					printf("se recibio paquete \n");
					segunda_tabla=*(uint32_t*)list_get(valores,0);
					printf("\n valor segunda tabla en obtener segunda tabla %"PRIu32" \n",segunda_tabla);
		        	log_info(cpu_logger,"Recibi valor de segunda tabla \n");
		        	return segunda_tabla;
		            break;
		        case -1:
		            log_error(cpu_logger, "Fallo la comunicacion. Abortando \n");
		            return (uint32_t)EXIT_FAILURE;
		        break;
		        default:
		        	log_warning(cpu_logger, "Operacion desconocida \n");
		        	exit(35);
		        	break;
		        }
		}
	}

//SEGUNDO ACCESO A MEMORIA

uint32_t obtener_marco(uint32_t segunda_tabla, uint32_t entrada_tabla_2){
	uint32_t marco;
	pedir_marco(socket_memoria,segunda_tabla,entrada_tabla_2);
	while(1){
			int codigo_op=recibir_operacion_nuevo(socket_memoria);
			t_list* valores;
			int size;
			switch(codigo_op){
				case MARCO:
					valores = recibir_paquete(socket_memoria);
					marco=*(uint32_t*)list_get(valores,0);
					log_info(cpu_logger,"Recibi valor de segunda tabla \n");
					return marco;
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
}
