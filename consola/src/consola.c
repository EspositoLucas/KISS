#include "consola.h"


int main(int argc, char **argv){

	if(argc < 2) {
		return EXIT_FAILURE;
    }

	printf("%s\n", argv[2]);

	int tamanio_proceso = atoi(argv[1]) ;
	int conexion_consola;

    t_log *logger;
    t_config *config;
    t_paquete* paquete = crear_paquete() ;


    /* ---------------- LOGGING CONSOLA  ---------------- */

    logger = iniciar_logger("consola.log", "consola", 1, LOG_LEVEL_INFO);

    log_info(logger, "INICIANDO CONSOLA");

    /* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

    config = iniciar_config("cfg/consola.config");

    config_valores_consola.ip_kernel = config_get_string_value(config, "IP_KERNEL");
    config_valores_consola.puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");

    t_list *instrucciones = parsear_instrucciones(argv[2]);

    // Conexión hacia el kernel

    conexion_consola = crear_conexion(config_valores_consola.ip_kernel, config_valores_consola.puerto_kernel);

    log_info(logger, "conexion kernel");

    enviar_mensaje(" : Envio a kernel la info del proceso", conexion_consola);

    log_info(logger, "mensaje kernel");

    paquete_proceso(conexion_consola,paquete,tamanio_proceso);


    log_info(logger, "paquete proceso");

    terminar_programa(conexion_consola, logger, config);

}

t_list *parsear_instrucciones(char *path) {
	t_list *instrucciones = list_create();
	codigo_instrucciones identificador;
	uint32_t parametro1;
	uint32_t parametro2;
	char *pseudo_codigo_leido = leer_archivo(path);
	char **split_instrucciones = string_split(pseudo_codigo_leido, "\n");
	int indice_split = 0;
	while(split_instrucciones[indice_split] != NULL) {
		char **palabras = string_split(split_instrucciones[indice_split], " ");
		if(string_equals_ignore_case(palabras[0], "NO_OP")){
			parametro1 = atoi(palabras[1]);
			for(int i=0; i<parametro1; i++){
				list_add(instrucciones, armar_estructura_instruccion(NO_OP, 0, 0));
			}
		}
		else if(string_equals_ignore_case(palabras[0], "I/O")){
			parametro1 = atoi(palabras[1]);
			list_add(instrucciones, armar_estructura_instruccion(IO, parametro1, 0));
		}
		else if(string_equals_ignore_case(palabras[0], "READ")){
			parametro1 = atoi(palabras[1]);
			list_add(instrucciones, armar_estructura_instruccion(READ, parametro1, 0));
		}
		else if(string_equals_ignore_case(palabras[0], "WRITE")){
			parametro1 = atoi(palabras[1]);
			parametro2 = atoi(palabras[2]);
			list_add(instrucciones, armar_estructura_instruccion(WRITE, parametro1, parametro2));
		}
		else if(string_equals_ignore_case(palabras[0], "COPY")){
			parametro1 = atoi(palabras[1]);
			parametro2 = atoi(palabras[2]);
			list_add(instrucciones, armar_estructura_instruccion(COPY, parametro1, parametro2));
		}
		else if(string_equals_ignore_case(palabras[0], "EXIT")){
			list_add(instrucciones, armar_estructura_instruccion(EXIT, 0, 0));
		}
		indice_split++;
		free(palabras);
	}
	return instrucciones;
}

instruccion *armar_estructura_instruccion(codigo_instrucciones id, uint32_t parametro1, uint32_t parametro2) {
	instruccion *estructura = malloc(sizeof(instruccion));
	estructura->codigo = id;
	estructura->parametro1 = parametro1;
	estructura->parametro2 = parametro2;
	return estructura;
}

// VERSION SIN DESERIALIZAR EN CONSOLA

void paquete_proceso(int conexion,t_paquete* paquete, int tamanio_proceso){

    char* leido = leer_archivo("instrucciones.txt");

// SE AGREGA EN UN SOLO PAQUETE PRIMERO EL TAMANIO DEL PROCESO Y DESPUES LAS INSTRUCCIONES

    agregar_datos_consola(paquete,leido,strlen(leido)+1,tamanio_proceso);
    enviar_paquete(paquete, conexion);
    eliminar_paquete(paquete);
}



// LEER ARCHIVO DE INSTRUCCIONES

char *leer_archivo(char *unPath)
{

    char instrucciones[100];
    strcpy(instrucciones, unPath);

    FILE *archivo = fopen(instrucciones, "r");

    if (archivo == NULL)
    {
        perror("Error al abrir el archivo");
    }

    fseek(archivo, 0, SEEK_END);         // mover el archivo al final
    int cant_elementos = ftell(archivo); // cantidad total de elementos que tiene el archivo
    rewind(archivo);                        //mover archivo al inicio del txt

    char *cadena = calloc(sizeof(char) + 1, cant_elementos); //arreglo dinamico de caracteres para almacenar en cadena el contenido del archivo
    if (cadena == NULL)
    {
        perror("Error en la reserva de memoria") ;
    }
    int cant_elementos_leidos = fread(cadena, sizeof(char), cant_elementos, archivo);
    if (cant_elementos_leidos != cant_elementos)
    {
        perror("Error leyendo el archivo") ;
    }

    printf("%s", cadena);
    fclose(archivo);
    printf("\n Se ha leido el archivo de pseudocodigo correctamente ..");
    return cadena;
}

