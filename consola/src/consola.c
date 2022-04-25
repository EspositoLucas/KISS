#include "consola.h"
#include "consolaUtils.h"

int main(int argcv, char **argv)
{

// ./consola 10000 /C:\Users\esposito\Carpetas Compartidas SO\TP\tp-2022-1c-Ubunteam\consola\instrucciones.txt
    
    int tamanio_proceso = atoi(argv[2]) ; 
    int conexion_consola;

    t_log *logger;
    t_config *config;

    /* ---------------- LOGGING CONSOLA  ---------------- */

    logger = iniciar_logger();

    log_info(logger, "INICIANDO CONSOLA");

    /* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

    config = iniciar_config();

    config_valores.ip_kernel = config_get_string_value(config, "IP_KERNEL");
    config_valores.puerto_kernel = config_get_int_value(config, "PUERTO_KERNEL");


    // Conexión hacia el kernel

    conexion_consola = crear_conexion(config_valores.ip_kernel, config_valores.puerto_kernel);

    enviar_mensaje(tamanio_proceso, conexion_consola);

    paquete_proceso(conexion_consola);

//    enviar_paquete(paquete, conexion_consola);

    terminar_programa(conexion_consola, logger, config);
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

    nuevo_config = config_create("cfg/consola.config");

    if (nuevo_config == NULL)
    {
        printf("No se pudo leer la config\n");
        exit(2);
    }


    return nuevo_config;
}


void paquete_proceso(int conexion)
{

    // char *leido = readline("> ");
    //    char *leido = leer_archivo(split[2]);
//    int tamanio_proceso = sizeof(char); // Cual seria el tamanio del proceso, el de tipo leido o el de todo el archivo ?
   
    t_paquete *paquete = crear_paquete();

    while (1) {
    char* leido = leer_archivo();

	char** split = string_split(leido, "\n");

    int longitud_instrucciones = string_array_size(split);

    if(string_contains(split[0], "NO_OP") ) {
        			char** split_NO_OP = string_split(split[0], " ");
                    if(string_contains(split_NO_OP[1], "5")) {
                    int parametro_NO_OP = atoi(split_NO_OP[1]);
                    for(int i=0; i< parametro_NO_OP  ; i++)
					{
						printf("NO_OP %d ", parametro_NO_OP);

					}    
                    }   
                
	   	    	}else if (string_contains(split[1], "I/O")){
                        
                        	char** split_IO = string_split(split[1], " ");
                            if(string_contains(split_IO[1], "3000")) {
                            int parametro_IO = atoi(split_IO[1]);
                            printf("I/O %d ", parametro_IO);
					}    
                    }else if (string_contains(split[2], "READ")){
                            char** split_READ = string_split(split[2], " ");
                            if(string_contains(split_READ[1], "0")) {
                            int parametro_READ = atoi(split_READ[1]);
                            printf("READ %d ", parametro_READ);

                   }else if (string_contains(split[3], "WRITE")) {
                            char** split_WRITE = string_split(split[3], " ");
                            if(string_contains(split_WRITE[1], "4") && string_contains(split_WRITE[2], "42")) {
                                int parametro1_WRITE = atoi(split_WRITE[1]);
                                int parametro2_WRITE = atoi(split_WRITE[2]);
                            printf("WRITE %d %d ", parametro1_WRITE,parametro2_WRITE); }

                   }else if (string_contains(split[4], "COPY")){
                            char** split_COPY = string_split(split[4], " ");
                            if(string_contains(split_COPY[1], "0") && string_contains(split_COPY[2], "4")) {
                                int parametro1_COPY = atoi(split_COPY[1]);
                                int parametro2_COPY = atoi(split_COPY[2]);

                            printf("COPY %d %d ", parametro1_COPY,parametro2_COPY);

                   }else if (string_contains(split[5], "EXIT")){

                            printf("EXIT");
                   }

        agregar_a_paquete(paquete, split, longitud_instrucciones + 1);
        free(leido);
        free(split);
 }
                   }

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

// // VERSION 1 - leer_archivo()
  char *leer_archivo()
  {
        char cadena[100] ;
     FILE *archivo, *salida ;
     archivo = fopen("argv[2]", "r");
     salida = fopen("instrucciones.txt", "r");
     if (archivo == NULL)
      {
          perror("Error al abrir el archivo");
          return 1;
     }
  if (salida == NULL)
      {
          perror("Error al abrir el archivo");
          return 1;
     }
 while(fgets(cadena,100,archivo)) {
     fputs(cadena,salida);
 }

     printf("&s", cadena);
     fclose(archivo);
     fclose(salida);
     printf("\n Se ha leido el archivo de pseudocodigo correctamente ..");
     return cadena;
  }


// VERSION 2 - leer_archivo()

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

// 

// VERSION 3 - leer_archivo(void)

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


// Argumentos en Eclipse : Run ConfiguarAtions - arguments y poner argumentos por espaciso : 10000  /C:\Users\esposito\Carpetas Compartidas SO\TP\tp-2022-1c-Ubunteam\consola\instrucciones.txt
