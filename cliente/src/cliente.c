#include "../include/cliente.h"

void *threadLecturaConsola(void *args) {
    printf("Iniciando la consola...\n");
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

			opcion = clientOptionToKey(modulo);
			if (mensaje) { comando = commandToString(mensaje); }
			logConsoleInput(comandoLeido);

			switch (opcion) {
				case OPTION_APP:
					printf("Se ha seleccionado el módulo APP (^-^*)ノ\n");
					switch (comando) {
						case CONSULTAR_RESTAURANTES:
							consultarRestaurantes();
							break;
						case SELECCIONAR_RESTAURANTE:
							seleccionarRestaurante(atoi(parametro1), parametro2);
							break;
						case CONSULTAR_PLATOS:
							consultarPlatos(conexionApp, "");
							break;
						case CREAR_PEDIDO:
							crearPedido(conexionApp);
							break;
						case ANIADIR_PLATO:
							aniadirPlato(conexionApp, parametro1, atoi(parametro2));
							break;
						case CONFIRMAR_PEDIDO:
							confirmarPedido(conexionApp, atoi(parametro1), "");
							break;
						case PLATO_LISTO:
							platoListo(conexionApp, parametro1, atoi(parametro2), parametro3);
							break;
						case CONSULTAR_PEDIDO:
							consultarPedido(conexionApp, atoi(parametro1));
							break;
						case ERROR:
						default:
							printf("El mensaje ingresado no es válido para el módulo APP (・ε・`*)...!\n");
							break;
					}
					break;
				case OPTION_COMANDA:
					printf("Se ha seleccionado el módulo COMANDA (ㆆᴗㆆ)\n");
					switch(comando) {
						case GUARDAR_PEDIDO:
							guardarPedido(conexionComanda, parametro1, atoi(parametro2));
							break;
						case GUARDAR_PLATO:
							guardarPlato(conexionComanda, parametro1, atoi(parametro2), parametro3, atoi(parametro4));
							break;
						case CONFIRMAR_PEDIDO:
							confirmarPedido(conexionComanda, atoi(parametro1), parametro2);
							break;
						case PLATO_LISTO:
							platoListo(conexionComanda, parametro1, atoi(parametro2), parametro3);
							break;
						case OBTENER_PEDIDO:
							obtenerPedido(conexionComanda, parametro1, atoi(parametro2));
							break;
						case ERROR:
						default:
							printf("El mensaje ingresado no es válido para el módulo COMANDA (・ε・`*)...!\n");
							break;
					}
					break;
				case OPTION_RESTAURANTE:
					printf("Se ha seleccionado el módulo RESTAURANTE ᕕ(◉Д◉ )ᕗ\n");
					switch(comando) {
						case CONSULTAR_PLATOS:
							consultarPlatos(conexionRestaurante, "");
							break;
						case CREAR_PEDIDO:
							crearPedido(conexionRestaurante);
							break;
						case ANIADIR_PLATO:
							aniadirPlato(conexionRestaurante, parametro1, atoi(parametro2));
							break;
						case CONFIRMAR_PEDIDO:
							confirmarPedido(conexionRestaurante, atoi(parametro1), "");
							break;
						case CONSULTAR_PEDIDO:
							consultarPedido(conexionRestaurante, atoi(parametro1));
							break;
						case ERROR:
						default:
							printf("El mensaje ingresado no es válido para el módulo RESTAURANTE (・ε・`*)...!\n");
							break;
					}
					break;
				case OPTION_SINDICATO:
					printf("Se ha seleccionado el módulo SINDICATO (σ≧∀≦)σ\n");
					switch(comando) {
						case OBTENER_RESTAURANTE:
							obtenerRestaurante(parametro1);
							break;
						case CONSULTAR_PLATOS:
							consultarPlatos(conexionSindicato, parametro1);
							break;
						case GUARDAR_PEDIDO:
							guardarPedido(conexionSindicato, parametro1, atoi(parametro2));
							break;
						case GUARDAR_PLATO:
							guardarPlato(conexionSindicato, parametro1, atoi(parametro2), parametro3, atoi(parametro4));
							break;
						case CONFIRMAR_PEDIDO:
							confirmarPedido(conexionSindicato, atoi(parametro1), parametro2);
							break;
						case PLATO_LISTO:
							platoListo(conexionSindicato, parametro1, atoi(parametro2), parametro3);
							break;
						case OBTENER_PEDIDO:
							obtenerPedido(conexionSindicato, parametro1, atoi(parametro2));
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

			free(modulo);
			free(mensaje);
			free(parametros);
			free(comandoLeido);
			if (opcion == OPTION_BAI) { break; }
			opcion = comando = ERROR;
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
 	// TODO: Recibe cliente (¿un id o un t_cliente?) y restaurante, retorna Ok/fail
	//enviarPaquete(conexionApp, CLIENTE, SELECCIONAR_RESTAURANTE, NULL);
}

void obtenerRestaurante(char *nombreRestaurante) {
	enviarPaquete(conexionSindicato, CLIENTE, OBTENER_RESTAURANTE, nombreRestaurante);
	t_header *header = recibirHeaderPaquete(conexionSindicato);
	t_posicion *pos = recibirPayloadPaquete(header, conexionSindicato);
	logMetadata(pos);
	free(pos);
	free(header);
}

void consultarPlatos(int conexion, char *nombreRestaurante) {
	enviarPaquete(conexion, CLIENTE, CONSULTAR_PLATOS, nombreRestaurante);
	t_header *header = recibirHeaderPaquete(conexion);
	t_list *platos = recibirPayloadPaquete(header, conexion);
	mostrarListaStrings(platos);
	free(platos);
	free(header);
}

void crearPedido(int conexion) {
	enviarPaquete(conexion, CLIENTE, CREAR_PEDIDO, NULL);

	t_header *header = recibirHeaderPaquete(conexion);
    int idPedido = recibirPayloadPaquete(header, conexion);

	//va a recibir un ID de pedido 
	printf("Respuesta obtenida: %d\n", idPedido);
}

void guardarPedido(int conexion, char *nombreRestaurante, int idPedido) {
	t_req_pedido *reqGuardarPedido = malloc(sizeof(t_req_pedido));
	reqGuardarPedido->restaurante = nombreRestaurante;
	reqGuardarPedido->idPedido = idPedido;

	enviarPaquete(conexion, CLIENTE, GUARDAR_PEDIDO, reqGuardarPedido);
	t_header *header = recibirHeaderPaquete(conexion);
	free(reqGuardarPedido);

	char *resultGuardarPedido = recibirPayloadPaquete(header, conexion);
	printf("%s", resultGuardarPedido);
	log_info(logger, "%s", resultGuardarPedido);
	free(header);
}

void aniadirPlato(int conexion, char *nombrePlato, int idPedido) { 
	// TODO: Generalizar t_req_pedido
	//enviarPaquete(conexion, CLIENTE, ANIADIR_PLATO, params);
}

void guardarPlato(int conexion, char *nombreRestaurante, int idPedido, char *nombrePlato, int cantidadPlato) {
	t_req_plato *reqPlato = malloc(sizeof(t_req_plato));
	reqPlato->restaurante = nombreRestaurante;
	reqPlato->idPedido = idPedido;
	reqPlato->plato = nombrePlato;
	reqPlato->cantidadPlato = cantidadPlato;

	enviarPaquete(conexion, CLIENTE, GUARDAR_PLATO, reqPlato);
	t_header *header = recibirHeaderPaquete(conexion);
	free(reqPlato);

	char *resultGuardarPlato = recibirPayloadPaquete(header, conexion);
	printf("%s", resultGuardarPlato);
	log_info(logger, "%s", resultGuardarPlato);
	free(header);
}

void confirmarPedido(int conexion, int idPedido, char *nombreRestaurante) {
	t_req_pedido *pedidoConf = malloc(sizeof(t_req_pedido));
	pedidoConf->idPedido = idPedido;
	pedidoConf->restaurante = nombreRestaurante;

	enviarPaquete(conexion, CLIENTE, CONFIRMAR_PEDIDO, pedidoConf);
	t_header *header = recibirHeaderPaquete(conexion);
	free(pedidoConf);

	char *resultConfPedido = recibirPayloadPaquete(header, conexion);
	printf("%s", resultConfPedido);
	log_info(logger, "%s", resultConfPedido);
	free(header);
}

void platoListo(int conexion, char *nombreRestaurante, int idPedido, char *plato) {
	 // TODO: struct que recibe restaurante, idPedido y plato
	//enviarPaquete(conexion, CLIENTE, PLATO_LISTO, params);
}

void consultarPedido(int conexion, int idPedido) {
	// TODO: El model del TP incluye un restaurante, que falta agregar a nuestro t_pedido
	//enviarPaquete(conexion, CLIENTE, CONSULTAR_PEDIDO, idPedido);
}

void obtenerPedido(int conexion, char *nombreRestaurante, int idPedido) {
	t_req_pedido *pedidoObt = malloc(sizeof(t_req_pedido));
	pedidoObt->restaurante = nombreRestaurante;
	pedidoObt->idPedido = idPedido;

	enviarPaquete(conexion, CLIENTE, OBTENER_PEDIDO, pedidoObt);
	t_header *header = recibirHeaderPaquete(conexion);
	free(pedidoObt);

	t_pedido *pedidoCompleto = recibirPayloadPaquete(header, conexion);
	mostrarListaPlatos(pedidoCompleto->platos);
	free(pedidoCompleto);
	free(header);
}

void mostrarComandosValidos(char *modulo) {
	void mensajesValidos() {
		printf("Ejemplo: AIUDA\n");
		printf("Ejemplo: CLEAR\n");
		printf("Ejemplo: BAI\n");
		printf("Para obtener los comandos válidos por módulo, ingresar: AIUDA [MODULO]\n");
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
				printf("APP ANIADIR_PLATO [NOMBRE_PLATO] [ID_PEDIDO]\n");
				printf("APP CONFIRMAR_PEDIDO [ID_PEDIDO]\n");
				printf("APP PLATO_LISTO [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_COMIDA]\n");
				printf("APP CONSULTAR_PEDIDO [ID_PEDIDO]\n");
				break;
			case OPTION_COMANDA:
				printf("COMANDA GUARDAR_PEDIDO [NOMBRE_RESTAURANTE] [ID_PEDIDO]\n");
				printf("COMANDA GUARDAR_PLATO [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_COMIDA] [CANTIDAD]\n");
				printf("COMANDA CONFIRMAR_PEDIDO [ID_PEDIDO] [NOMBRE_RESTAURANTE]\n");
				printf("COMANDA PLATO_LISTO [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_COMIDA]\n");
				printf("COMANDA OBTENER_PEDIDO [NOMBRE_RESTAURANTE] [ID_PEDIDO]\n");
				break;
			case OPTION_RESTAURANTE:
				printf("RESTAURANTE CONSULTAR_PLATOS [NOMBRE_RESTAURANTE]\n");
				printf("RESTAURANTE CREAR_PEDIDO\n");
				printf("RESTAURANTE ANIADIR_PLATO [NOMBRE_PLATO] [ID_PEDIDO]\n");
				printf("RESTAURANTE CONFIRMAR PEDIDO [ID_PEDIDO] \n");
				printf("RESTAURANTE CONSULTAR_PEDIDO [ID_PEDIDO]\n");
				break;
			case OPTION_SINDICATO:
				printf("SINDICATO OBTENER_RESTAURANTE [NOMBRE_RESTAURANTE]\n");
				printf("SINDICATO CONSULTAR_PLATOS [NOMBRE_RESTAURANTE]\n");
				printf("SINDICATO GUARDAR_PEDIDO [NOMBRE_RESTAURANTE] [ID_PEDIDO]\n");
				printf("SINDICATO GUARDAR_PLATO [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_COMIDA] [CANTIDAD]\n");
				printf("SINDICATO CONFIRMAR_PEDIDO [ID_PEDIDO] [NOMBRE_RESTAURANTE]\n");
				printf("SINDICATO PLATO_LISTO [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_COMIDA]\n");
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

void liberarConexiones() {
	liberarConexion(conexionApp);
	liberarConexion(conexionComanda);
	liberarConexion(conexionRestaurante);
	liberarConexion(conexionSindicato);
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

	liberarConexiones();
    finalizarProceso();
    return EXIT_SUCCESS;
}