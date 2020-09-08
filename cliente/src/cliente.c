#include "../include/cliente.h"

//void* threadLecturaConsola(void * args)
void threadLecturaConsola()
{
    printf("Iniciando la consola ...\n");
	printf("Para ver los comandos válidos, ingrese 'AIUDA'.\n");

	char *comandoOriginal;
	char **parametros;
	char *modulo;
	char *mensaje;
    char *comandoLeido = readline("(=^.^=)~>");

    while (1) {
        if (!comandoLeido || string_equals_ignore_case(comandoLeido, "")) {
            continue;
        } else {
            //add_history(comandoLeido);
            comandoOriginal = malloc(sizeof(char) *strlen(comandoLeido)+1);
            strcpy(comandoOriginal, comandoLeido);
            string_to_upper(comandoLeido);
            string_trim(&comandoLeido);

			log_debug(logger, "Comando ingresado: %s", comandoLeido); // Por ahora, para ver lo que toma

			// Obtenemos el comando ingresado
            parametros = string_split(comandoLeido, " ");
			modulo = parametros[0];
			mensaje = parametros[1];
            free(comandoOriginal);

			// AIUDA
            if (string_starts_with(modulo, "AIUDA")) {
				mostrarComandosValidos();
			}

			// APP
			else if (string_starts_with(modulo, "APP")) {
				printf("Se ha seleccionado el módulo APP\n");
				if (string_starts_with(mensaje, "CONSULTAR_RESTAURANTES")) {
					consultarRestaurantesAApp(); //TODO: Implementación
				}
				/*
				if (otroMensaje...) {
					...
				}
				*/
            }

			// COMANDA
			else if (string_starts_with(modulo, "COMANDA")) {
				printf("Se ha seleccionado el módulo COMANDA\n");
				if (string_starts_with(mensaje, "")) {
					//TODO
				}
            }

			// RESTAURANTE
			else if (string_starts_with(modulo, "RESTAURANTE")) {
				printf("Se ha seleccionado el módulo RESTAURANTE\n");
				if (string_starts_with(mensaje, "")) {
					//TODO
				}
            }

			// SINDICATO
			else if (string_starts_with(modulo, "SINDICATO")) {
				printf("Se ha seleccionado el módulo SINDICATO\n");
				if (string_starts_with(mensaje, "")) {
					//TODO
				}
            }

			// Cualquier otra cosa
			else {
                printf("Comando no válido. Escriba 'AIUDA' para ver el formato aceptado.\n");
            }

			free(modulo);
			free(mensaje);
            free(parametros);
            free(comandoLeido);

			comandoLeido = readline("(=^.^=)~>");
        }
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

	// Inicio del hilo de la consola y su lectura - Por ahora comentado hasta que veamos cómo hacer para que funcione este thread
	//pthread_create(&threadConsola, NULL, (void *) threadLecturaConsola, NULL);
    //pthread_detach(threadConsola);

	while (1) {
		threadLecturaConsola();
	}

	liberarConexion(conexionRestaurante);
    finalizarProceso();
    return EXIT_SUCCESS;
}