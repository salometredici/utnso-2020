#include "../include/cliente.h"

void* threadLecturaConsola(void * args) {
    printf("Iniciando la consola ...\n");
	printf("Para ver los comandos válidos, ingrese 'AIUDA'.\n");

	char *comandoOriginal;
	char **parametros;
	char *modulo;
	char *mensaje;
	int opcion;
    char *comandoLeido = readline("(=^.^=)~>");

    while (1) {

		if (!comandoLeido || string_equals_ignore_case(comandoLeido, "")) {
            continue;
		} else {
            comandoOriginal = malloc(sizeof(char) *strlen(comandoLeido)+1);
            strcpy(comandoOriginal, comandoLeido);
            string_to_upper(comandoLeido);
            string_trim(&comandoLeido);

			// Obtenemos el comando ingresado
            parametros = string_split(comandoLeido, " ");
			modulo = parametros[0];
			mensaje = parametros[1];
            free(comandoOriginal);
			log_debug(logger, "Comando ingresado: %s", comandoLeido); // Por ahora, para ver lo que toma

			opcion = stringAProceso(modulo); 
			switch (opcion) {
				case APP:
					printf("Se ha seleccionado el módulo APP\n");
					if (string_starts_with(mensaje, "CONSULTAR_RESTAURANTES")) {
						consultarRestaurantesAApp(); //TODO: Implementación
					}
					/*
					if (otroMensaje...) {
						...
					}
					*/
					break;
				case COMANDA:
					printf("Se ha seleccionado el módulo COMANDA\n");
					if (string_starts_with(mensaje, "")) {
						//TODO
					}
					break;
				case RESTAURANTE:
					printf("Se ha seleccionado el módulo RESTAURANTE\n");
					if (string_starts_with(mensaje, "")) {
						//TODO
					}
					break;
				case SINDICATO:
					printf("Se ha seleccionado el módulo SINDICATO\n");
					if (string_starts_with(mensaje, "")) {
						//TODO
					}
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
		}

		free(modulo);
		free(mensaje);
        free(parametros);
		if (opcion == BAI) { break; }

    	free(comandoLeido);
		comandoLeido = readline("(=^.^=)~>");
	}

	free(comandoLeido);
    pthread_exit(EXIT_SUCCESS);
    return 0;
}

void consultarRestaurantesAApp() { 
    printf("Buscando Restaurantes ...!!! \n");
    printf("ups... aún estamos trabajando! ~(T-T)~\n");
    // ACA VA LOGICA DEL ENVIO DEL MENSAJE POR SOCKET AL MODULO APP Y ESTA SE ENCARGARA DE IMPLEMENTARLA  
}

void consultarPlatosARestaurante() {
    printf("Buscando Platos ...(°-°)!!! \n");
    printf("ups... disculpe... aún estamos en fase de implementación ...\n");
    // ACA VA LOGICA DEL ENVIO DEL MENSAJE POR SOCKET AL MODULO RESTAURANTE Y ESTA SE ENCARGARA DE IMPLEMENTARLA 
}

void mostrarComandosValidos() {
    printf("-------------------Comandos Válidos-------------------\n");
	printf("Ejemplo: AIUDA\n");
	printf("Ejemplo: CLEAR\n");
	printf("Ejemplo: BAI\n");
    printf("Formato: [MODULO] [MENSAJE] [PARAMETROS]\n");
	printf("Ejemplo: APP CONSULTAR_RESTAURANTES\n");
	printf("Ejemplo: APP SELECCIONAR_RESTAURANTE [NOMBRE_CLIENTE] [NOMBRE_RESTAURANTE]\n");
	printf("Ejemplo: SINDICATO OBTENER_RESTAURANTE [NOMBRE_RESTAURANTE]\n");
	printf("Ejemplo: RESTAURANTE CONSULTAR_PLATOS [NOMBRE_RESTAURANTE]\n");
    printf("------------------------------------------------------\n");
}

int main(int argc, char* argv[]) {
	inicializarProceso(CLIENTE);
    //conexionApp = conectarseA(APP);
	//conexionComanda = conectarseA(COMANDA);
	conexionRestaurante = conectarseA(RESTAURANTE);
	//conexionSindicato = conectarseA(SINDICATO);

	// Inicio del hilo de la consola y su lectura
	pthread_create(&threadConsola, NULL, (void *) threadLecturaConsola, NULL);
    pthread_detach(threadConsola);

	while (1) {
		//TODO: Lógica del Cliente
	}

	liberarConexion(conexionRestaurante);
    finalizarProceso();
    return EXIT_SUCCESS;
}