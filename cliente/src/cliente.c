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
							consultarRestaurantesAapp(); //TODO: Implementación
							break;
						case SELECCIONAR_RESTAURANTE:
							seleccionarRestauranteAapp();
							break;
						case CONSULTAR_PLATOS:
							consultarPlatosAapp();
							break;
						case CREAR_PEDIDO:
							crearPedidoAapp();
							break;
						case ANIADIR_PLATO:
							aniadirPlatoAapp();
							break;
						case CONFIRMAR_PEDIDO:
							confirmarPedidoAapp();
							break;
						case PLATO_LISTO:
							platoListoAapp();
							break;
						case CONSULTAR_PEDIDO:
							consultarPedidoAapp();
							break;
						case ERROR:
						default:
							printf("El mensaje ingresado no es válido para el módulo APP (・ε・`*)...!\n");
							break;
					}
					break;
				case COMANDA:
					printf("Se ha seleccionado el módulo COMANDA\n");
					switch(comando) {
						case GUARDAR_PEDIDO:
							guardarPedidoAComanda();
							break;
						case GUARDAR_PLATO:
							guardarPlatoAComanda();
							break;
						case CONFIRMAR_PEDIDO:
							confirmarPedidoAComanda();
							break;
						case PLATO_LISTO:
							platoListoAComanda();
							break;
						case OBTENER_PEDIDO:
							obtenerPedidoAComanda();
							break;
						case ERROR:
							printf("El mensaje ingresado no es válido para el módulo COMANDA (・ε・`*)...!\n");
							break;
					}
					break;
				case RESTAURANTE:
					printf("Se ha seleccionado el módulo RESTAURANTE\n");
					switch(comando) {
						case CONSULTAR_PLATOS:
							consultarPlatosARestaurante();
							break;
						case CREAR_PEDIDO:
							crearPedidoARestaurante();
							break;
						case ANIADIR_PLATO:
							aniadirPlatoARestaurante();
							break;
						case CONFIRMAR_PEDIDO:
							confirmarPedidoAComanda();
							break;
						
						case CONSULTAR_PEDIDO:
							consultarPedidoARestaurante();
							break;
						case ERROR:
							printf("El mensaje ingresado no es válido para el módulo RESTAURANTE (・ε・`*)...!\n");
							break;
					}
					break;
				case SINDICATO:
				printf("Se ha seleccionado el módulo SINDICATO\n");
					switch(comando) {
						case CONSULTAR_PLATOS:
							consultarPlatosASindicato();
							break;
						case OBTENER_RESTAURANTE:
							obtenerRestauranteASindicato();
							break;
						case ANIADIR_PLATO:
							aniadirPlatoARestaurante();
							break;
						case CONFIRMAR_PEDIDO:
							confirmarPedidoAComanda();
							break;
						
						case CONSULTAR_PEDIDO:
							consultarPedidoARestaurante();
							break;
						case ERROR:
							printf("El mensaje ingresado no es válido para el módulo RESTAURANTE (・ε・`*)...!\n");
							break;
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
///// IMPLEMENTACION ENVIO MENSAJES A APP //////////

void consultarRestaurantesAapp() { 
    printf("Buscando Restaurantes ...!!! \n");
    printf("ups... aún estamos trabajando! ~(T-T)~\n");
    // ACA VA LOGICA DEL ENVIO DEL MENSAJE POR SOCKET AL MODULO APP Y ESTA SE ENCARGARA DE IMPLEMENTARLA  
}
void seleccionarRestauranteAapp() {
	printf("\n Usted selecciono SELECCIONAR RESTAURANTE");
}
void consultarPlatosAapp() {
	printf("\n Usted selecciono CONSULTAR PLATOS");
}
void crearPedidoAapp() {
	printf("\n Usted selecciono CREAR PEDIDO");
}
void aniadirPlatoAapp() { 
	printf("\n Usted selecciono: ANIADIR PLATO");
}
void confirmarPedidoAapp() {
	printf("\n Usted selecciono: CONFIRMAR PEDIDO");
}
void platoListoAapp() {
	printf("\n Usted selecciono:PLATO LISTO");
}
void consultarPedidoAapp() {
	printf("\n Usted selecciono: CONSULTAR PEDIDO");
}
///// IMPLEMENTACION ENVIO MENSAJES A COMANDA //////////
void guardarPedidoAComanda() {

}
void guardarPlatoAComanda() {

}
void confirmarPedidoAComanda() {

}
void platoListoAComanda() {

}
void obtenerPedidoAComanda() {

}

///// IMPLEMENTACION ENVIO MENSAJES A RESTAURANTE //////////
void consultarPlatosARestaurante() {
    printf("Buscando Platos ...(°-°)!!! \n");
    printf("ups... disculpe... aún estamos en fase de implementación ...\n");
    // ACA VA LOGICA DEL ENVIO DEL MENSAJE POR SOCKET AL MODULO RESTAURANTE Y ESTA SE ENCARGARA DE IMPLEMENTARLA 
}
void crearPedidoARestaurante() {

}
void aniadirPlatoARestaurante() {

}
void confirmarPedidoARestaurante() {

}
void consultarPedidoARestaurante() {

}


///// IMPLEMENTACION ENVIO MENSAJES A SINDICATO //////////
void obtenerRestauranteASindicato() {

}
void consultarPlatosASindicato() {

}
void guardarPedidoASindicato() {

}
void guardarPlatoASindicato() {

}
void confirmarPedidoASindicato() {

}
void platoListoASindicato() { 

}
void obtenerPedidoASindicato() {

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

/////////////////////////////// FUNCION PRINCIPAL //////////////////////
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