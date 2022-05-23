#include "consola.h"
//#include "consolaUtils.h"


int main(int argc, char **argv){

// ./consola 10000 /C:\Users\esposito\Carpetas Compartidas SO\TP\tp-2022-1c-Ubunteam\consola\instrucciones.txt
	if(argc < 3) {
		return EXIT_FAILURE;
    }

	int tamanio_proceso = atoi(argv[1]) ;
	int conexion_consola;

    t_log *logger;
    t_config *config;
    t_paquete* paquete = crear_paquete() ;

    /* ---------------- LOGGING CONSOLA  ---------------- */

    logger = iniciar_logger();

    log_info(logger, "INICIANDO CONSOLA");

    /* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

    config = iniciar_config();

    config_valores_consola.ip_kernel = config_get_string_value(config, "IP_KERNEL");
    config_valores_consola.puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");


    // Conexión hacia el kernel

    conexion_consola = crear_conexion(config_valores_consola.ip_kernel, config_valores_consola.puerto_kernel);

    enviar_mensaje(" : Envio a kernel la info del proceso", conexion_consola);

    agregar_entero_a_paquete(paquete, tamanio_proceso);

    enviar_paquete(paquete, conexion_consola);

    paquete_proceso(conexion_consola);


    terminar_programa(conexion_consola, logger, config);
}


t_log *iniciar_logger(void){

    t_log *nuevo_logger;

    nuevo_logger = log_create("consola.log", "consola", 1, LOG_LEVEL_INFO);

    if (nuevo_logger == NULL){
        printf("No se pudo crear el logeer consola\n");
        exit(1);
    }

    return nuevo_logger;
}


t_config *iniciar_config(void){
    t_config *nuevo_config;

    nuevo_config = config_create("cfg/consola.config");

    if (nuevo_config == NULL){
    	printf("No se pudo leer la config\n");
        exit(2);
    }

    return nuevo_config;
}

//  //VERSION 1 PAQUETE

//  void paquete_proceso(int conexion){

   
//      t_paquete *paquete = crear_paquete();
//      char* leido = leer_archivo("instrucciones.txt");
//      char** split = string_split(leido, "\n");

//      int longitud_instrucciones = string_array_size(split);

//      while (1) {


//      	if(string_contains(split[0], "NO_OP") ) {
//      		char** split_NO_OP = string_split(split[0], " ");
//      		int parametro_NO_OP = atoi(split_NO_OP[1]);
//              for(int i=0; i< parametro_NO_OP  ; i++){
//              	agregar_a_paquete(paquete, split_NO_OP[0], longitud_instrucciones + 1);
//              	printf("NO_OP %d ", parametro_NO_OP);

//              }

//      	} else if (string_contains(split[1], "I/O")){
//      		char** split_IO = string_split(split[1], " ");
//              int parametro_IO = atoi(split_IO[1]);
//              printf("I/O %d ", parametro_IO);
//              agregar_a_paquete(paquete, split[1], longitud_instrucciones + 1);
//      	}
//      	 else if (string_contains(split[2], "READ")){
//      		char** split_READ = string_split(split[2], " ");
//              int parametro_READ = atoi(split_READ[1]);
//              printf("READ %d ", parametro_READ);
//              agregar_a_paquete(paquete, split[2], longitud_instrucciones + 1);

//      	} else if (string_contains(split[3], "WRITE")) {
//      		char** split_WRITE = string_split(split[3], " ");
//              int parametro1_WRITE = atoi(split_WRITE[1]);
//              int parametro2_WRITE = atoi(split_WRITE[2]);
//              printf("WRITE %d %d ", parametro1_WRITE,parametro2_WRITE);
//              agregar_a_paquete(paquete, split[3], longitud_instrucciones + 1);

//      	} else if (string_contains(split[4], "COPY")){
//      		char** split_COPY = string_split(split[4], " ");
//              int parametro1_COPY = atoi(split_COPY[1]);
//              int parametro2_COPY = atoi(split_COPY[2]);
//              printf("COPY %d %d ", parametro1_COPY,parametro2_COPY);
//              agregar_a_paquete(paquete, split[4], longitud_instrucciones + 1);

//      	} else if (string_contains(split[5], "EXIT")){

//      		printf("EXIT");
//      		agregar_a_paquete(paquete, split[5], longitud_instrucciones + 1);
//      	}
//  //    	agregar_a_paquete(paquete, split, longitud_instrucciones + 1);
//  //    	free(leido);
//  //    	free(split);

//      }


//      enviar_paquete(paquete, conexion);
//      eliminar_paquete(paquete);
//      free(leido);
//      free(split);

//  }

// // VERSION 2 PAQUETE - Serializar en consola

// void paquete_proceso(int conexion){

   
//     t_paquete *paquete = crear_paquete();
//     char* leido = leer_archivo("instrucciones.txt");
//     char** split = string_split(leido, "\n");
//     int longitud_instrucciones = string_array_size(split_una_vez_separado);
//     int indice_split = 0 ;

//     while (split[indice_split] != NULL) {

//     	char* palabras = string_split(split[indice_split], " ") ;

//     	if(string_contains(palabras[0], "NO_OP") ) {
//     		int parametro_NO_OP = atoi(palabras[1]);
//             for(int i=0; i< parametro_NO_OP  ; i++){
//             	printf("NO_OP %d ", parametro_NO_OP);

//             }

//     	} else if (string_contains(palabras[0], "I/O")){
//             int parametro_IO = atoi(palabras[1]);
//             printf("I/O %d ", parametro_IO);
//     	}
//     	 else if (string_contains(palabras[0], "READ")){
//             int parametro_READ = atoi(palabras[1]);
//             printf("READ %d ", parametro_READ);

//     	} else if (string_contains(palabras[0], "WRITE")) {
//             int parametro1_WRITE = atoi(palabras[1]);
//             int parametro2_WRITE = atoi(palabras[2]);
//             printf("WRITE %d %d ", parametro1_WRITE,parametro2_WRITE);

//     	} else if (string_contains(palabras[0], "COPY")){
//             int parametro1_COPY = atoi(palabras[1]);
//             int parametro2_COPY = atoi(palabras[2]);
//             printf("COPY %d %d ", parametro1_COPY,parametro2_COPY);

//     	} else if (string_contains(palabras[0], "EXIT")){

//     		printf("EXIT");
//     	}
//       	agregar_a_paquete(paquete, split_una_vez_separado, longitud_instrucciones + 1);
// //    	free(leido);
// //    	free(palabras);
//     }
//     enviar_paquete(paquete, conexion);
//     eliminar_paquete(paquete);
//     free(leido);
//     free(split);

// }

// VERSION 3 PAQUETE - Sin serializar en consola 

void paquete_proceso(int conexion){

   
    t_paquete *paquete = crear_paquete();
    char* leido = leer_archivo("instrucciones.txt");
//    char** split = string_split(leido, "\n");
//    int longitud_instrucciones = string_array_size(split);
//    int indice_split = 0 ;

    agregar_a_paquete(paquete, leido, strlen(leido)+1 );
//    	free(leido);

    enviar_paquete(paquete, conexion);
    eliminar_paquete(paquete);
//    free(leido);

}


void terminar_programa(int conexion, t_log *logger, t_config *config){

    if (logger != NULL){
        log_destroy(logger);
    }

    if (config != NULL){
        config_destroy(config);
    }

    liberar_conexion(conexion);
}



// LEER ARCHIVO

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
    free(cadena);
    fclose(archivo);
    printf("\n Se ha leido el archivo de pseudocodigo correctamente ..");
    return cadena;
}

