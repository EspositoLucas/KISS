#include "consola.h"


int main(int argc, char **argv){

	if(argc < 3) {
		return EXIT_FAILURE;
    }

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


    // Conexión hacia el kernel

    conexion_consola = crear_conexion(config_valores_consola.ip_kernel, config_valores_consola.puerto_kernel);

    enviar_mensaje(" : Envio a kernel la info del proceso", conexion_consola);

    agregar_entero_a_paquete(paquete, tamanio_proceso);

    enviar_paquete(paquete, conexion_consola);

    paquete_proceso(conexion_consola);


    terminar_programa(conexion_consola, logger, config);
}



// VERSION sin serializar en consola

void paquete_proceso(int conexion){

   
    t_paquete *paquete = crear_paquete();
    char* leido = leer_archivo("instrucciones.txt");

    agregar_a_paquete(paquete, leido, strlen(leido)+1 );

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
    free(cadena);
    fclose(archivo);
    printf("\n Se ha leido el archivo de pseudocodigo correctamente ..");
    return cadena;
}

