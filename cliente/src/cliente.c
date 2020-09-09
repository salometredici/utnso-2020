#include "../include/cliente.h"

void* threadLecturaConsola(void * args) {
    printf("Iniciando la consola ...\n");
	printf("Para ver los comandos válidos, ingrese 'AIUDA'.\n");

	char *comandoOriginal;
	char **parametros;
	char *modulo = "";
	char *mensaje = "";
	int opcion;
	int comando;
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

			// Obtenemos el comando ingresado
            parametros = string_split(comandoLeido, " ");
			modulo = parametros[0];
			mensaje = parametros[1];
            free(comandoOriginal);
			log_debug(logger, "Comando ingresado: %s", comandoLeido); // Por ahora, para ver lo que toma

			// Setear módulo/opción y de haberlo, el mensaje
			opcion = stringAKey(modulo, CONSOLENKEYS);
			if (mensaje) {
				comando = stringAKey(mensaje, COMMANDNKEYS);
			}

			switch (opcion) {
				case APP:
					printf("Se ha seleccionado el módulo APP\n");
					switch (comando) {
						case CONSULTAR_RESTAURANTES:
							consultarRestaurantesAApp(); //TODO: Implementación
							break;
						case SELECCIONAR_RESTAURANTE:
							//TODO
							break;
						case CONSULTAR_PLATOS:
							//TODO
							break;
						case CREAR_PEDIDO:
							//TODO
							break;
						case ANIADIR_PLATO:
							//TODO
							break;
						case CONFIRMAR_PEDIDO:
							//TODO
							break;
						case PLATO_LISTO:
							//TODO
							break;
						case CONSULTAR_PEDIDO:
							//TODO
							break;
						case ERROR:
						default:
							printf("El mensaje ingresado no es válido para el módulo APP (・ε・`*)...!\n");
							break;
					}
					break;
				case COMANDA:
					//TODO: Switch con las opciones válidas para COMANDA
					break;
				case RESTAURANTE:
					//TODO: Switch con las opciones válidas para RESTAURANTE
					break;
				case SINDICATO:
					//TODO: Switch con las opciones válidas para SINDICATO
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
				default:
					printf("Comando no válido. Escriba 'AIUDA' para ver el formato aceptado つ´Д`)つ\n");
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