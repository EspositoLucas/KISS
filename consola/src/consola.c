#include "consola.h"

int main(int argcv, char **argv)
{
    /*---------------------------------------------------PARTE 2-------------------------------------------------------------*/
    int tamanio_proceso;
    int conexion;
    char *ip_kernel;
    char *puerto_kernel;

    t_log *logger;
    t_config *config;

    /* ---------------- LOGGING CONSOLA  ---------------- */

    logger = iniciar_logger();

    log_info(logger, "INICIANDO CONSOLA");

    /* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

    config = iniciar_config();

    ip_kernel = config_get_string_value(config, "IP_KERNEL");
    puerto_kernel = config_get_int_value(config, "PUERTO_KERNEL");

    /* ---------------- LEER ARCHIVO DE PSEUDOCODIGO ---------------- */

    char *instrucciones = leer_archivo(logger);

    // Conexión hacia el servidor

    conexion = crear_conexion(ip_kernel, puerto_kernel);

    enviar_mensaje(valor, conexion);

    paquete(conexion);

    enviar_paquete(paquete, conexion);

    terminar_programa(conexion, logger, config);
}

t_log *iniciar_logger(void)
{
    t_log *nuevo_logger;

    nuevo_logger = log_create("consola.log", "consola", 1, LOG_LEVEL_INFO);

    if (nuevo_logger == NULL)
    {
        printf("No se pudo crear el logeer\n");
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

char *leer_archivo(char *unPath)
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

// void leer_consola(t_log *logger)
// {
//     char *leido;

//     // La primera te la dejo de yapa
//     leido = readline("> ");

//     // El resto, las vamos leyendo y logueando hasta recibir un string vacío
//     while (strcmp(leido, "") != 0)
//     {
//         leido = readline("> ");
//         log_info(logger, "Lei las lineas  %d\n", strlen(leido) + 1);
//     }

//     // ¡No te olvides de liberar las lineas antes de regresar!
//     free(leido);
// }

void paquete(int conexion)
{

    // char *leido = readline("> ");
    char *leido = leer_archivo()
        t_paquete *paquete = crear_paquete();

    // Leemos y esta vez agregamos las lineas al paquete

    while (strcmp(leido, "") != 0)
    {
        agregar_a_paquete(paquete, leido, strlen(leido) + 1);
        free(leido);
        leido = readline("> ");
    }

    // ¡No te olvides de liberar las líneas y el paquete antes de regresar!
    free(leido);
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
