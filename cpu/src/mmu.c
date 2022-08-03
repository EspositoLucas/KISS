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
		uint32_t segunda_tabla=obtener_segunda_tabla(primera_tabla,entrada_tabla_1);
		log_info(cpu_logger,"valor entrada segunda tabla %d \n",segunda_tabla);
		marco=(int)obtener_marco(segunda_tabla, entrada_tabla_2);
		traduccion_t* trad=malloc(sizeof(traduccion_t));
		trad->marco=(uint32_t)marco;
		trad->pagina=num_pagina;
		tlb_miss(trad);
	}

	//LA SITUACION DE TLB HIT ESTA IMPLEMENTADA ADENTRO DE BUSCAR EN LA TLB

	//CALCULO DE DIRECCION FISICA

	uint32_t direccion_fisica=marco*configuracion_tabla->tam_pagina+offset;
	log_info(cpu_logger, "valor marco %d \n", marco);
	log_info(cpu_logger, "valor offset %d \n", offset);
	log_info(cpu_logger, "Direccion fisica traducida: %d \n",direccion_fisica);

	return direccion_fisica;

}

//PRIMER ACCESO A MEMORIA

uint32_t obtener_segunda_tabla(uint32_t primera_tabla, uint32_t entrada_tabla_1){
	pedir_tabla_pagina(socket_memoria,primera_tabla,entrada_tabla_1);
	uint32_t segunda_tabla = 0;

	recibir_datos(socket_memoria,&segunda_tabla,sizeof(uint32_t));
	return segunda_tabla;

	}

//SEGUNDO ACCESO A MEMORIA

uint32_t obtener_marco(uint32_t segunda_tabla, uint32_t entrada_tabla_2){
	uint32_t marco;
	pedir_marco(socket_memoria,segunda_tabla,entrada_tabla_2);
	t_list* valores;
	while(1){
			int codigo_op=recibir_operacion_nuevo(socket_memoria);
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
