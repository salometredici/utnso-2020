#include "../include/cliente.h"

void* threadLecturaConsola(void * args) {
    printf("Iniciando la consola ...\n");
	printf("Para ver los comandos válidos, ingrese 'AIUDA'.\n");

	char *comandoOriginal;
	char **parametros;
	char *modulo = "", *mensaje = "", *parametro1 = "", *parametro2 = "", *parametro3 = "", *parametro4 = "";
	int opcion, comando;
  	char *comandoLeido = readline("(=^.^=)~>");

    while (1) {
        add_history(comandoLeido);
        comandoOriginal = malloc(sizeof(char) *strlen(comandoLeido)+1);
    	strcpy(comandoOriginal, comandoLeido);
        string_to_upper(comandoLeido);
        string_trim(&comandoLeido);
		
		// Obtenemos el comando ingresado
        parametros = string_split(comandoLeido, " ");
		modulo = parametros[0];
		mensaje = parametros[1];
		parametro1 = parametros[2]; parametro2 = parametros[3]; parametro3 = parametros[4];	parametro4 = parametros[5];

        free(comandoOriginal);
		log_debug(logger, "Comando ingresado: %s", comandoLeido); // Por ahora, para ver lo que toma

		// Setear módulo/opción y de haberlo, el mensaje
		opcion = clientOptionToKey(modulo);
		if (mensaje) {
			comando = commandToString(mensaje);
		}
		
		switch (opcion) {
			case OPTION_APP:
				printf("Se ha seleccionado el módulo APP\n");
				switch (comando) {
					case CONSULTAR_RESTAURANTES:
						consultarRestaurantesAapp(); //TODO: Implementación
						break;
					case SELECCIONAR_RESTAURANTE:
						seleccionarRestauranteAapp(parametro1, parametro2);
						break;
					case CONSULTAR_PLATOS:
						consultarPlatosAapp(parametro1);
						break;
					case CREAR_PEDIDO:
						crearPedidoAapp();
						break;
					case ANIADIR_PLATO:
						aniadirPlatoAapp(parametro1, parametro2);
						break;
					case CONFIRMAR_PEDIDO:
						confirmarPedidoAapp(parametro1);
						break;
					case PLATO_LISTO:
						platoListoAapp(parametro1, parametro2, parametro3);
						break;
					case CONSULTAR_PEDIDO:
						consultarPedidoAapp(parametro1);
						break;
					case ERROR:
					default:
						printf("El mensaje ingresado no es válido para el módulo APP (・ε・`*)...!\n");
						break;
				}
				break;
			case OPTION_COMANDA:
				printf("Se ha seleccionado el módulo COMANDA\n");
				switch(comando) {
					case GUARDAR_PEDIDO:
						guardarPedidoAComanda(parametro1, parametro2);
						break;
					case GUARDAR_PLATO:
						guardarPlatoAComanda(parametro1, parametro2, parametro3, parametro4);
						break;
					case CONFIRMAR_PEDIDO:
						confirmarPedidoAComanda(parametro1);
						break;
					case PLATO_LISTO:
						platoListoAComanda(parametro1, parametro2, parametro3);
						break;
					case OBTENER_PEDIDO:
						obtenerPedidoAComanda(parametro1, parametro2);
						break;
					case ERROR:
					default:
						printf("El mensaje ingresado no es válido para el módulo COMANDA (・ε・`*)...!\n");
						break;
				}
				break;
			case OPTION_RESTAURANTE:
				printf("Se ha seleccionado el módulo RESTAURANTE\n");
				switch(comando) {
					case CONSULTAR_PLATOS:
						consultarPlatosARestaurante(parametro1);
						break;
					case CREAR_PEDIDO:
						crearPedidoARestaurante();
						break;
					case ANIADIR_PLATO:
						aniadirPlatoARestaurante(parametro1, parametro2);
						break;
					case CONFIRMAR_PEDIDO:
						confirmarPedidoAComanda(parametro1);
						break;
					case CONSULTAR_PEDIDO:
						consultarPedidoARestaurante(parametro1);
						break;
					case ERROR:
					default:
						printf("El mensaje ingresado no es válido para el módulo RESTAURANTE (・ε・`*)...!\n");
						break;
				}
				break;
			case OPTION_SINDICATO:
				printf("Se ha seleccionado el módulo SINDICATO\n");
				switch(comando) {
					case OBTENER_RESTAURANTE:
						obtenerRestauranteASindicato(parametro1);
						break;
					case CONSULTAR_PLATOS:
						consultarPlatosASindicato(parametro1);
						break;
					case GUARDAR_PEDIDO:
						guardarPedidoASindicato(parametro1, parametro2);
							break;
					case GUARDAR_PLATO:
						guardarPlatoASindicato(parametro1, parametro2, parametro3, parametro4);
						break;
					case CONFIRMAR_PEDIDO:
						confirmarPedidoASindicato(parametro1);
						break;
					case PLATO_LISTO:
						platoListoASindicato(parametro1, parametro2, parametro3);
						break;
					case OBTENER_PEDIDO:
						obtenerPedidoASindicato(parametro1,parametro2);
						break;		
					case ERROR:
					default:
						printf("El mensaje ingresado no es válido para el módulo SINDICATO (・ε・`*)...!\n");
						break;
				}
				break;
			case OPTION_AIUDA:
				mostrarComandosValidos();
				break;
			case OPTION_BAI:
				printf("adiosss (๑♡3♡๑)!\n");
				break;
			case OPTION_CLEAR:
				limpiarPantalla();
				break;
			case ERROR:
			default:
				printf("Comando no válido. Escriba 'AIUDA' para ver el formato aceptado つ´Д`)つ\n");
				break;
		}

		free(modulo);
		free(mensaje);
        free(parametros);
    	free(comandoLeido);
		if (opcion == OPTION_BAI) { break; }

		comandoLeido = readline("(=^.^=)~>");
	}

    pthread_exit(EXIT_SUCCESS);
    return 0;
}

// App

void consultarRestaurantesAapp() { 
    printf("Buscando Restaurantes ...!!! \n");
    printf("ups... aún estamos trabajando! ~(T-T)~\n");
    // ACA VA LOGICA DEL ENVIO DEL MENSAJE POR SOCKET AL MODULO APP Y ESTA SE ENCARGARA DE IMPLEMENTARLA  
}

void seleccionarRestauranteAapp(char *nombreCliente, char *nombreRestaurante) {
	printf("\n Usted selecciono SELECCIONAR RESTAURANTE");
}

void consultarPlatosAapp(char *nombreRestaurante) {
	printf("\n Usted selecciono CONSULTAR PLATOS");
}

void crearPedidoAapp() {
	printf("\n Usted selecciono CREAR PEDIDO");
}

void aniadirPlatoAapp(char *nombrePlato, char* idPedido) { 
	printf("\n Usted selecciono: ANIADIR PLATO");
}

void confirmarPedidoAapp(char *idPedido) {
	printf("\n Usted selecciono: CONFIRMAR PEDIDO");
}

void platoListoAapp(char *nombreRestaurante, char *idPedido, char *nombreComida) {
	printf("\n Usted selecciono:PLATO LISTO");
}

void consultarPedidoAapp(char *idPedido) {
	printf("\n Usted selecciono: CONSULTAR PEDIDO");
}

// Comanda

void guardarPedidoAComanda(char *nombreRestaurante, char *idPedido) {
	//TODO
}

void guardarPlatoAComanda(char *nombreRestaurante, char *idPedido, char *nombreComida, int cantidadPlatos) {
	//TODO
}

void confirmarPedidoAComanda(char *idPedido) {
	//TODO
}

void platoListoAComanda(char *nombreRestaurante, char *idPedido, char *nombreComida) {
	//TODO
}

void obtenerPedidoAComanda(char *nombreRestaurante, char *idPedido) {
	//TODO
}

// Restaurante

void consultarPlatosARestaurante(char *nombreRestaurante) {
    printf("Buscando Platos ...(°-°)!!! \n");
    printf("ups... disculpe... aún estamos en fase de implementación ...\n");
    // ACA VA LOGICA DEL ENVIO DEL MENSAJE POR SOCKET AL MODULO RESTAURANTE Y ESTA SE ENCARGARA DE IMPLEMENTARLA 
}

void crearPedidoARestaurante() {
	//TODO
}

void aniadirPlatoARestaurante(char *nombrePlato, char *idPedido) {
	//TODO
}

void confirmarPedidoARestaurante(char *idPedido) {
	//TODO
}

void consultarPedidoARestaurante(char *idPedido) {
	//TODO
}

// Sindicato

void obtenerRestauranteASindicato(char *nombreRestaurante) {
	//TODO
}

void consultarPlatosASindicato(char *nombreRestaurante) {
	//TODO
}

void guardarPedidoASindicato(char *nombreRestaurante, char *idPedido) {
	//TODO
}

void guardarPlatoASindicato(char *nombreRestaurante, char *idPedido, char *nombreComida, int cantidadPlatos) {
	//TODO
}

void confirmarPedidoASindicato(char *idPedido) {
	//TODO
}

void platoListoASindicato(char *nombreRestaurante, char *idPedido, char *nombreComida) { 
	//TODO
}

void obtenerPedidoASindicato(char *nombreRestaurante, char *idPedido) {
	//TODO
}

void mostrarComandosValidos() {
    printf("-------------------Comandos Válidos-------------------\n");
	printf("Ejemplo: AIUDA\n");
	printf("Ejemplo: CLEAR\n");
	printf("Ejemplo: BAI\n");

	// FORMATO MENSAJES A APP
    printf("Formato: [MODULO] [MENSAJE] [PARAMETROS]\n");
	printf("APP CONSULTAR_RESTAURANTES\n");
	printf("APP SELECCIONAR_RESTAURANTE [NOMBRE_CLIENTE] [NOMBRE_RESTAURANTE]\n");
	printf("APP CONSULTAR_PLATOS [NOMBRE_RESTAURANTE]\n");
	printf("APP CREAR_PEDIDO\n");
	printf("APP ANIADIR_PEDIDO [NOMBRE_PLATO] [ID_PEDIDO]\n");
	printf("APP CONFIRMAR_PEDIDO [ID_PEDIDO]\n");
	printf("APP PLATO_LISTO [NOMBRE_RESTAURANTE][ID_PEDIDO][NOMBRE_COMIDA]\n");
	printf("APP CONSULTAR_PEDIDO [ID_PEDIDO] \n");
	
	// FORMATO MENSAJES A COMANDA
	printf("COMANDA GUARDAR_PEDIDO [NOMBRE_RESTAURANTE] [ID_PEDIDO] \n");
	printf("COMANDA GUARDAR_PLATO [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_COMIDA][CANTIDAD]\n");
	printf("COMANDA CONFIRMAR_PEDIDO [ID_PEDIDO]\n");
	printf("COMANDA PLATO_LISTO [NOMBRE_RESTAURANTE][ID_PEDIDO][NOMBRE_COMIDA]\n");
	printf("COMANDA OBTENER_PEDIDO [NOMBRE_RESTAURANTE] [ID_PEDIDO]\n");
	
	// FORMATO DE MENSAJES A RESTAURANTE
	printf("RESTAURANTE CONSULTAR_PLATOS [NOMBRE_RESTAURANTE] \n");
	printf("RESTAURANTE CREAR_PEDIDO \n");
	printf("RESTAURANTE ANIADIR_PLATO [NOMBRE_PLATO] [ID_PEDIDO] \n");
	printf("RESTAURANTE CONFIRMAR PEDIDO [ID_PEDIDO] \n");
	printf("RESTAURANTE CONSULTAR_PEDIDO [ID_PEDIDO]\n");
	
	// FORMATO DE MENSAJES A SINDICATO
	printf("SINDICATO OBTENER_RESTAURANTE [NOMBRE_RESTAURANTE] \n");
    printf("SINDICATO CONSULTAR_PLATOS [NOMBRE_RESTAURANTE] \n");
	printf("SINDICATO GUARDAR_PEDIDO [NOMBRE_RESTAURANTE] [ID_PEDIDO]\n");
	printf("SINDICATO GUARDAR_PLATO [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_COMIDA] [CANTIDAD]\n");
	printf("SINDICATO CONFIRMAR_PEDIDO [ID_PEDIDO] \n");
	printf("SINDICATO PLATO_LISTO [NOMBRE_RESTAURANTE][ID_PEDIDO][NOMBRE_COMIDA] \n");
	printf("SINDICATO OBTENER_PEDIDO [NOMBRE_RESTAURANTE] [ID_PEDIDO]\n");
	printf("------------------------------------------------------\n");
}

int main(int argc, char* argv[]) {
	inicializarProceso(CLIENTE);
    //conexionApp = conectarseA(APP);
	//conexionComanda = conectarseA(COMANDA);
	conexionRestaurante = conectarseA(RESTAURANTE);
	conexionSindicato = conectarseA(SINDICATO);

	// Inicio del hilo de la consola y su lectura
	pthread_create(&threadConsola, NULL, (void *) threadLecturaConsola, NULL);
    pthread_detach(threadConsola);

	while (1) {
		//TODO: Lógica del Cliente
	}

	liberarConexion(conexionApp);
	liberarConexion(conexionRestaurante);
    finalizarProceso();
    return EXIT_SUCCESS;
}