#include "../include/sindicato.h"

void mostrarComandosValidos() {
    printf("-------------------Comandos Válidos-------------------\n");
    printf("Formato: [MENSAJE] [PARAMETROS]\n");
	printf("Ejemplo: CrearRestaurante [NOMBRE] [CANTIDAD_COCINEROS] [POSICION] [AFINIDAD_COCINEROS] [PLATOS] [PRECIO_PLATOS] [CANTIDAD_HORNOS]\n");
	printf("Ejemplo: CrearReceta [NOMBRE] [PASOS] [TIEMPO_PASOS] \n");
    printf("------------------------------------------------------\n");
}

void* threadLecturaConsola(void * args) {
    printf("Iniciando la consola ...\n");
	printf("Para ver los comandos válidos, ingrese 'AIUDA'.\n");

	char *comandoOriginal;
	char **parametros;
	char *mensaje;
	int opcion;
    char *comandoLeido = readline("(=^.^=)~>");

    while (1) {
        if (!comandoLeido || string_equals_ignore_case(comandoLeido, "")) {
            continue;
        } else {
            add_history(comandoLeido);
            comandoOriginal = malloc(sizeof(char) *strlen(comandoLeido)+1);
            strcpy(comandoOriginal, comandoLeido);
            string_to_upper(comandoLeido);
            string_trim(&comandoLeido);
            parametros = string_split(comandoLeido, " ");
			mensaje = parametros[0];
            free(comandoOriginal);

            opcion = stringAProceso(mensaje); 
			switch (opcion) {
				case CREARRESTAURANTE:
					printf("Crear Restaurante: Se deberá crear una nueva carpeta restaurante, con su respectivo info.AFIP explicado anteriormente\n");
					break;
				case CREARRECETA:
					printf("Crear Receta: Se deberá crear un nuevo archivo de receta siguiendo los lineamientos de lo detallado anteriormente.\n");
					break;
				case AIUDA:
					mostrarComandosValidos();
					break;
				case BAI:
					printf("adiosss (๑♡3♡๑)!\n");
					break;
				case CLEAR:
					limpiarPantalla();
				case ERROR:
					printf("Comando no válido. Escriba 'AIUDA' para ver el formato aceptado.\n");
					break;
			}

			free(mensaje);
            free(parametros);
            if (opcion == BAI) { break; }

            free(comandoLeido);
			comandoLeido = readline("(=^.^=)~>");
        }
	}
	free(comandoLeido);
    pthread_exit(EXIT_SUCCESS);
    return 0;
}

int main(int argc, char ** argv){
    inicializarProceso(SINDICATO);

	// Inicio del hilo de la consola y su lectura
	pthread_create(&threadConsola, NULL, (void *) threadLecturaConsola, NULL);
    pthread_detach(threadConsola);

	while (1) {
		//TODO: Lógica del Cliente
	}

    finalizarProceso();
    return EXIT_SUCCESS;
}