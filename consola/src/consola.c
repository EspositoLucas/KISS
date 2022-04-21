#include "consola.h"

int main(int argcv, char **argv)
{

    t_log *logger;
    t_config *config;

    /* ---------------- LOGGING CONSOLA  ---------------- */

    logger = iniciar_logger();

    log_info(logger, "INICIANDO CONSOLA");

    /* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

    config = iniciar_config();

    config_valores.ip_kernel = config_get_string_value(config, "IP_KERNEL");
    config_valores.puerto_kernel = config_get_int_value(config, "PUERTO_KERNEL");

    /* ---------------- LEER ARCHIVO DE PSEUDOCODIGO ---------------- */

//    char *instrucciones = leer_archivo(logger);

    // Conexión hacia el servidor

    conexion_consola = crear_conexion(ip_kernel, puerto_kernel);

//    enviar_mensaje(instrucciones, conexion);

    paquete_proceso(conexion_consola);

    enviar_paquete(paquete, conexion_consola);

    terminar_programa(conexion_consola), logger, config);
}

t_log *iniciar_logger(void)
{
    t_log *nuevo_logger;

    nuevo_logger = log_create("consola.log", "consola", 1, LOG_LEVEL_INFO);

    if (nuevo_logger == NULL)
    {
        printf("No se pudo crear el logeer consola\n");
        exit(1);
    }

    return nuevo_logger;
}

t_config *iniciar_config(void)
{
    t_config *nuevo_config;

    nuevo_config = config_create("./consola.config");

    if (nuevo_config == NULL)
    {
        printf("No se pudo leer la config\n");
        exit(2);
    }

    return nuevo_config;
}

// VERSION 1 - leer_archivo(void)

// char *leer_archivo(char *unPath) // para usar el path, ver funcion split
// {

//     // char instrucciones[100];

//     // strcpy(instrucciones, "../shared/TP 1c2022/consola");

//     // strcat(instrucciones, unPath);

//     FILE *archivo = fopen(instrucciones, "r");

//     if (archivo == NULL)
//     {
//         perror("Error al abrir el archivo");
//         return 1;
//     }

//     fseek(archivo, 0, SEEK_END);         // mover el archivo al final
//     int cant_elementos = ftell(archivo); // cantidad total de elementos que tiene el archivo
//     rewind(file);                        //mover archivo al inicio del txt

//     char *cadena = calloc(sizeof(char) + 1, cant_elementos); //arreglo dinamico de caracteres para almacenar en cadena el contenido del archivo
//     if (cadena == NULL)
//     {
//         perror("Error en la reserva de memoria") return 2;
//     }
//     int cant_elementos_leidos = fread(cadena, sizeof(char), cant_elementos, archivo);
//     if (cant_elementos_leidos != cant_elementos)
//     {
//         perror("Error leyendo el archivo") return 3;
//     }

//     printf("&s", cadena);
//     free(cadena);
//     fclose(archivo);
//     printf("\n Se ha leido el archivo de pseudocodigo correctamente ..");
//     return cadena;
// }


void paquete_proceso(int conexion)
{

    // char *leido = readline("> ");
    //    char *leido = leer_archivo(split[2]);
//    int tamanio_proceso = sizeof(char); // Cual seria el tamanio del proceso, el de tipo leido o el de todo el archivo ?
   
    t_paquete *paquete = crear_paquete();

    while (1) {
	  char* leido = leer_archivo();

	  char** split = string_split(leido, "\n");
      int tamanio_instrucciones = string_arr_size(split);
     if(string_equals_ignore_case(split[0], "NO_OP") )
	   	    	{
                    parametro_especial = list_get(split[0],)
                   for(int i=0; i<  ; i++)
					{
						imprimirTripulante(list_get(tripulantesNuevos,i));
					}    
	   	    	}
       agregar_a_paquete(paquete, split, tamanio_instrucciones + 1);
       agregar_entero_a_paquete(paquete, tamanio_instrucciones) ;
        free(leido);
        free(split);
 }

    enviar_paquete(paquete, conexion);
    eliminar_paquete(paquete);
}

void terminar_programa(int conexion, t_log *logger, t_config *config)
{

    if (logger != NULL)
    {
        log_destroy(logger);
    }

    if (config != NULL)
    {
        config_destroy(config);
    }

    liberar_conexion(conexion);
}


// VERSION 2 - leer_archivo()

char *leer_archivo(char *unPath) // para usar el path, ver funcion split
{

    // char instrucciones[100];

    // strcpy(instrucciones, "../shared/TP 1c2022/consola");

    // strcat(instrucciones, unPath);

    FILE *archivo = fopen(instrucciones, "r");

    if (archivo == NULL)
    {
        perror("Error al abrir el archivo");
        return 1;
    }

    fseek(archivo, 0, SEEK_END);         // mover el archivo al final
    int cant_elementos = ftell(archivo); // cantidad total de elementos que tiene el archivo
    rewind(file);                        //mover archivo al inicio del txt

    char *cadena = calloc(sizeof(char) + 1, cant_elementos); //arreglo dinamico de caracteres para almacenar en cadena el contenido del archivo
    if (cadena == NULL)
    {
        perror("Error en la reserva de memoria") return 2;
    }
    int cant_elementos_leidos = fread(cadena, sizeof(char), cant_elementos, archivo);
    if (cant_elementos_leidos != cant_elementos)
    {
        perror("Error leyendo el archivo") return 3;
    }

    printf("&s", cadena);
    free(cadena);
    fclose(archivo);
    printf("\n Se ha leido el archivo de pseudocodigo correctamente ..");
    return cadena;
}

void manejarConsola(void){

	log_info(logger,"INICIANDO CONSOLA");
	
    while (1) {
	  printf("ingrese un comando: \n");
	  char* leido = readline(">");

	  char** split = string_split(leido, " ");
      int tamanio_proceso = string_arr_size(split);

	 


		   	   			free(split);
		   	   			free(leido);
						// BORRAR PAQUETE
						eliminar_paquete(nuevoPaquete);
						// LIBERAR CONEXION
						liberar_conexion(socket_cliente);
		   	   	   //}

	          }
	  
}


