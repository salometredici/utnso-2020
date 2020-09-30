#include "../include/cliente.h"

void* threadLecturaConsola(void * args) {
    printf("Iniciando la consola ...\n");
	printf("Para ver los comandos válidos, ingrese 'AIUDA'.\n");

	char **parametros;
	char *modulo = "", *mensaje = "", *parametro1 = "", *parametro2 = "", *parametro3 = "", *parametro4 = "";
	int opcion, comando;
  	char *comandoLeido = readline("(=^.^=)~>");

    while (1) {
	
		if (!string_is_empty(comandoLeido)) {
			add_history(comandoLeido);
        	string_to_upper(comandoLeido);
        	string_trim(&comandoLeido);
			parametros = string_split(comandoLeido, " ");
			modulo = parametros[0];
			mensaje = parametros[1];
			parametro1 = parametros[2]; parametro2 = parametros[3]; parametro3 = parametros[4];	parametro4 = parametros[5];

			// Setear módulo/opción y de haberlo, el mensaje
			opcion = clientOptionToKey(modulo);
			if (mensaje) { comando = commandToString(mensaje); }
			logConsoleInput(comandoLeido);
			
			//validar comando leido, se le envia la opcion el comando y los parametros

			switch (opcion) {
				case OPTION_APP:
					printf("Se ha seleccionado el módulo APP\n");
					switch (comando) {
						case CONSULTAR_RESTAURANTES:
							consultarRestaurantes();
							break;
						case SELECCIONAR_RESTAURANTE:
							seleccionarRestaurante(parametro1, parametro2);
							break;
						case CONSULTAR_PLATOS:
							consultarPlatos(conexionApp, parametro1);
							break;
						case CREAR_PEDIDO:
							crearPedido(conexionApp);
							break;
						case ANIADIR_PLATO:
							aniadirPlato(conexionApp, parametro1, parametro2);
							break;
						case CONFIRMAR_PEDIDO:
							confirmarPedido(conexionApp, parametro1, NULL);
							break;
						case PLATO_LISTO:
							platoListo(conexionApp, parametro1, parametro2, parametro3);
							break;
						case CONSULTAR_PEDIDO:
							consultarPedido(conexionApp, parametro1);
							break;
						case ERROR:
						default:
							printf("El mensaje ingresado no es válido para el módulo APP (・ε・`*)...!\n");
							break;
					}
					break;
				case OPTION_COMANDA:
					printf("Modulo seleccionado: COMANDA\n");
					switch(comando) {
						case GUARDAR_PEDIDO:
							guardarPedido(conexionComanda, parametro1, parametro2);
							break;
						case GUARDAR_PLATO:
							guardarPlato(conexionComanda, parametro1, parametro2, parametro3, parametro4);
							break;
						case CONFIRMAR_PEDIDO:
							confirmarPedido(conexionComanda, parametro1, parametro2);
							break;
						case PLATO_LISTO:
							platoListo(conexionComanda, parametro1, parametro2, parametro3);
							break;
						case OBTENER_PEDIDO:
							obtenerPedido(conexionComanda, parametro1, parametro2);
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
							consultarPlatos(conexionRestaurante, parametro1);
							break;
						case CREAR_PEDIDO:
							crearPedido(conexionRestaurante);
							break;
						case ANIADIR_PLATO:
							aniadirPlato(conexionRestaurante, parametro1, parametro2);
							break;
						case CONFIRMAR_PEDIDO:
							confirmarPedido(conexionRestaurante, parametro1, NULL);
							break;
						case CONSULTAR_PEDIDO:
							consultarPedido(conexionRestaurante, parametro1);
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
							obtenerRestaurante(parametro1);
							break;
						case CONSULTAR_PLATOS:
							consultarPlatos(conexionSindicato, parametro1);
							break;
						case GUARDAR_PEDIDO:
							guardarPedido(conexionSindicato, parametro1, parametro2);
							break;
						case GUARDAR_PLATO:
							guardarPlato(conexionSindicato, parametro1, parametro2, parametro3, parametro4);
							break;
						case CONFIRMAR_PEDIDO:
							confirmarPedido(conexionSindicato, parametro1, parametro2);
							break;
						case PLATO_LISTO:
							platoListo(conexionSindicato, parametro1, parametro2, parametro3);
							break;
						case OBTENER_PEDIDO:
							obtenerPedido(conexionSindicato, parametro1, parametro2);
							break;		
						case ERROR:
						default:
							printf("El mensaje ingresado no es válido para el módulo SINDICATO (・ε・`*)...!\n");
							break;
					}
					break;
				case OPTION_AIUDA:
					mostrarComandosValidos(mensaje);					
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

			opcion = comando = ERROR;
			free(modulo);
			free(mensaje);
			free(parametros);
			free(comandoLeido);
			if (opcion == OPTION_BAI) { break; }
		}
		comandoLeido = readline("(=^.^=)~>");
	}

    pthread_exit(EXIT_SUCCESS);
    return 0;
}


void consultarRestaurantes() {
	enviarPaquete(conexionApp, CLIENTE, CONSULTAR_RESTAURANTES, NULL);
    t_header *header = recibirHeaderPaquete(conexionApp);
    t_list *restaurantes = recibirPayloadPaquete(header, conexionApp);
	mostrarListaStrings(restaurantes);
	free(restaurantes);
	free(header);
}

void seleccionarRestaurante(char *nombreCliente, char *nombreRestaurante) {
	// envia un cliente para asociar a un restaurante y espera resultado ok/fail 
	enviarPaquete(conexionApp, CLIENTE, SELECCIONAR_RESTAURANTE, NULL);
}

void obtenerRestaurante(char *nombreRestaurante) {
	// envia un restaurante a sindicato y espera un metadata restaurante
	enviarPaquete(conexionSindicato, CLIENTE, OBTENER_RESTAURANTE, nombreRestaurante);
}

void consultarPlatos(int conexion, char *nombreRestaurante) {
	// consulta las recetas de un restaurante, espera una lista de recetas
	enviarPaquete(conexion, CLIENTE, CONSULTAR_PLATOS, nombreRestaurante);
}

void crearPedido(int conexion) {
	enviarPaquete(conexion, CLIENTE, CREAR_PEDIDO, NULL);

	t_header *header = recibirHeaderPaquete(conexion);
    char *respuesta = recibirPayloadPaquete(header, conexion);

	//va a recibir un ID de pedido 
	printf("Respuesta obtenida: %s\n", respuesta);
}

void guardarPedido(int conexion, char *nombreRestaurante, char *idPedido) {
	params = list_create();
	list_add(params, nombreRestaurante);
	list_add(params, idPedido);

	enviarPaquete(conexion, CLIENTE, GUARDAR_PEDIDO, params);

	t_header *header = recibirHeaderPaquete(conexion);
    char *respuesta = recibirPayloadPaquete(header, conexion);

	printf("Respuesta obtenida: %s\n", respuesta);
	//envia nombre restaurante y id, recibe ok/fail
}

void aniadirPlato(int conexion, char *nombrePlato, char* idPedido) { 
	params = list_create();
	list_add(params, nombrePlato);
	list_add(params, idPedido);

	enviarPaquete(conexion, CLIENTE, ANIADIR_PLATO, params);
	//envi aplato y ID y recibe un ok/fail
}

void guardarPlato(int conexion, char *nombreRestaurante, char *idPedido, char *nombreComida, char *cantidadPlatos) {
	params = list_create();
	list_add(params, nombreRestaurante);
	list_add(params, idPedido);
	list_add(params, nombreComida);
	list_add(params, cantidadPlatos);

	enviarPaquete(conexion, CLIENTE, GUARDAR_PLATO, params);

	t_header *header = recibirHeaderPaquete(conexion);
    char *respuesta = recibirPayloadPaquete(header, conexion);
	//retorna ok/fail
	printf("Respuesta obtenida: %s\n", respuesta);
}

void confirmarPedido(int conexion, char *idPedido, char *nombreRestaurante) {
	// en caso de parametros opcionales se enviará NULL
	enviarPaquete(conexion, CLIENTE, CONFIRMAR_PEDIDO, idPedido);

	t_header *header = recibirHeaderPaquete(conexion);
    char *respuesta = recibirPayloadPaquete(header, conexion);

	printf("Respuesta obtenida: %s\n", respuesta);
}

void platoListo(int conexion, char *nombreRestaurante, char *idPedido, char *nombreComida) {
	params = list_create();
	list_add(params, nombreRestaurante);
	list_add(params, idPedido);
	list_add(params, nombreComida);

	enviarPaquete(conexion, CLIENTE, PLATO_LISTO, params);

	t_header *header = recibirHeaderPaquete(conexion);
    char *respuesta = recibirPayloadPaquete(header, conexion);
	//retorna ok/fail
	printf("Respuesta obtenida: %s\n", respuesta);
}

void consultarPedido(int conexion, char *idPedido) {
	enviarPaquete(conexion, CLIENTE, CONSULTAR_PEDIDO, idPedido);
	//retorna un pedido 
}

void obtenerPedido(int conexion, char *nombreRestaurante, char *idPedido) {
	params = list_create();
	list_add(params, nombreRestaurante);
	list_add(params, idPedido);

	enviarPaquete(conexion, CLIENTE, OBTENER_PEDIDO, params);

	t_header *header = recibirHeaderPaquete(conexion);
    char *respuesta = recibirPayloadPaquete(header, conexion);

	printf("Respuesta obtenida: %s\n", respuesta);
}

void mostrarComandosValidos(char *modulo) {
	void mensajesValidos() {
		printf("Ejemplo: AIUDA\n");
		printf("Ejemplo: CLEAR\n");
		printf("Ejemplo: BAI\n");
		printf("Para obtener los comandos válidos por módulo, ingresar AIUDA [MODULO]\n");
	}

    printf("-------------------Comandos Válidos-------------------\n");

	if (modulo) {
		int proceso = clientOptionToKey(modulo);
		printf("Formato: [MODULO] [MENSAJE] [PARAMETROS] ~~旦_(･o･;)\n");
		switch (proceso) {
			case OPTION_APP:
				printf("APP CONSULTAR_RESTAURANTES\n");
				printf("APP SELECCIONAR_RESTAURANTE [NOMBRE_CLIENTE] [NOMBRE_RESTAURANTE]\n");
				printf("APP CONSULTAR_PLATOS [NOMBRE_RESTAURANTE]\n");
				printf("APP CREAR_PEDIDO\n");
				printf("APP ANIADIR_PLATO [NOMBRE_PLATO] [ID_PEDIDO] \n");
				printf("APP CONFIRMAR_PEDIDO [ID_PEDIDO]\n");
				printf("APP PLATO_LISTO [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_COMIDA]\n");
				printf("APP CONSULTAR_PEDIDO [ID_PEDIDO] \n");
				break;
			case OPTION_COMANDA:
				printf("COMANDA GUARDAR_PEDIDO [NOMBRE_RESTAURANTE] [ID_PEDIDO] \n");
				printf("COMANDA GUARDAR_PLATO [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_COMIDA] [CANTIDAD]\n");
				printf("COMANDA CONFIRMAR_PEDIDO [ID_PEDIDO] [NOMBRE_RESTAURANTE]\n");
				printf("COMANDA PLATO_LISTO [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_COMIDA]\n");
				printf("COMANDA OBTENER_PEDIDO [NOMBRE_RESTAURANTE] [ID_PEDIDO]\n");
				break;
			case OPTION_RESTAURANTE:
				printf("RESTAURANTE CONSULTAR_PLATOS [NOMBRE_RESTAURANTE] \n");
				printf("RESTAURANTE CREAR_PEDIDO \n");
				printf("RESTAURANTE ANIADIR_PLATO [NOMBRE_PLATO] [ID_PEDIDO] \n");
				printf("RESTAURANTE CONFIRMAR PEDIDO [ID_PEDIDO] \n");
				printf("RESTAURANTE CONSULTAR_PEDIDO [ID_PEDIDO]\n");
				break;
			case OPTION_SINDICATO:
				printf("SINDICATO OBTENER_RESTAURANTE [NOMBRE_RESTAURANTE] \n");
				printf("SINDICATO CONSULTAR_PLATOS [NOMBRE_RESTAURANTE] \n");
				printf("SINDICATO GUARDAR_PEDIDO [NOMBRE_RESTAURANTE] [ID_PEDIDO]\n");
				printf("SINDICATO GUARDAR_PLATO [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_COMIDA] [CANTIDAD]\n");
				printf("SINDICATO CONFIRMAR_PEDIDO [ID_PEDIDO] [NOMBRE_RESTAURANTE] \n");
				printf("SINDICATO PLATO_LISTO [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_COMIDA] \n");
				printf("SINDICATO OBTENER_PEDIDO [NOMBRE_RESTAURANTE] [ID_PEDIDO]\n");
				break;
			default:
				mensajesValidos();
				break;
		}
	} else {
		mensajesValidos();                           
	}
	
	printf("------------------------------------------------------\n");
}

int main(int argc, char* argv[]) {
	inicializarProceso(CLIENTE);
    conexionApp = conectarseA(APP);
	conexionComanda = conectarseA(COMANDA);
	conexionRestaurante = conectarseA(RESTAURANTE);
	conexionSindicato = conectarseA(SINDICATO);

	// Inicio del hilo de la consola y su lectura
	pthread_create(&threadConsola, NULL, (void *) threadLecturaConsola, NULL);
    pthread_detach(threadConsola);

	while (1) {
		//TODO: Lógica del Cliente
	}

	liberarConexion(conexionApp);
	liberarConexion(conexionComanda);
	liberarConexion(conexionRestaurante);
	liberarConexion(conexionSindicato);
    finalizarProceso();
    return EXIT_SUCCESS;
}