#include "../include/cliente.h"

void *threadLecturaConsola(void *args) {
    printf("Iniciando la consola...\n");
	printf("Para ver los comandos válidos, ingrese 'AIUDA'.\n");

	char **parametros;
	char *mensaje = "", *parametro1 = "", *parametro2 = "", *parametro3 = "", *parametro4 = "";
	int opcion, comando;

  	char *comandoLeido = readline("(=^.^=)~>");

    while (1) {
	
		if (!string_is_empty(comandoLeido)) {
			add_history(comandoLeido);
        	string_to_upper(comandoLeido);
        	string_trim(&comandoLeido);
			parametros = string_split(comandoLeido, " ");
			opcion = clientOptionToKey(getStringKeyValue(procesoConectado, PROCNKEYS));
			mensaje = parametros[0];
			parametro1 = parametros[1]; parametro2 = parametros[2]; parametro3 = parametros[3];	parametro4 = parametros[4];

			comando = commandToString(mensaje);
			if (comando == ERROR) {
				opcion = clientOptionToKey(mensaje);
			}
			logConsoleInput(comandoLeido);
			
			switch (opcion) {
				case OPTION_APP:
					comando = validateCommand(opcion, comando, parametros);
					switch (comando) {
						case CONSULTAR_RESTAURANTES:
							consultarRestaurantes();
							break;
						case SELECCIONAR_RESTAURANTE:
							seleccionarRestaurante(atoi(parametro1), parametro2);
							break;
						case CONSULTAR_PLATOS:
							consultarPlatos("");
							break;
						case CREAR_PEDIDO:
							crearPedido();
							break;
						case ANIADIR_PLATO:
							aniadirPlato(parametro1, atoi(parametro2));
							break;
						case CONFIRMAR_PEDIDO:
							confirmarPedido(atoi(parametro1), "");
							break;
						case PLATO_LISTO:
							platoListo(parametro1, atoi(parametro2), parametro3);
							break;
						case CONSULTAR_PEDIDO:
							consultarPedido(atoi(parametro1));
							break;
						case ERROR:
						default:
							showInvalidCommandMsg(APP);
							break;
					}
					break;
				case OPTION_COMANDA:
					comando = validateCommand(opcion, comando, parametros);
					switch(comando) {
						case GUARDAR_PEDIDO:
							guardarPedido(parametro1, atoi(parametro2));
							break;
						case GUARDAR_PLATO:
							guardarPlato(parametro1, atoi(parametro2), parametro3, atoi(parametro4));
							break;
						case CONFIRMAR_PEDIDO:
							confirmarPedido(atoi(parametro1), parametro2);
							break;
						case PLATO_LISTO:
							platoListo(parametro1, atoi(parametro2), parametro3);
							break;
						case OBTENER_PEDIDO:
							obtenerPedido(parametro1, atoi(parametro2));
							break;
						case ERROR:
						default:
							showInvalidCommandMsg(COMANDA);
							break;
					}
					break;
				case OPTION_RESTAURANTE:
					comando = validateCommand(opcion, comando, parametros);
					switch(comando) {
						case CONSULTAR_PLATOS:
							consultarPlatos("");
							break;
						case CREAR_PEDIDO:
							crearPedido();
							break;
						case ANIADIR_PLATO:
							aniadirPlato(parametro1, atoi(parametro2));
							break;
						case CONFIRMAR_PEDIDO:
							confirmarPedido(atoi(parametro1), "");
							break;
						case CONSULTAR_PEDIDO:
							consultarPedido(atoi(parametro1));
							break;
						case ERROR:
						default:
							showInvalidCommandMsg(RESTAURANTE);
							break;
					}
					break;
				case OPTION_SINDICATO:
					comando = validateCommand(opcion, comando, parametros);
					switch(comando) {
						case OBTENER_RESTAURANTE:
							obtenerRestaurante(parametro1);
							break;
						case CONSULTAR_PLATOS:
							consultarPlatos("");
							break;
						case GUARDAR_PEDIDO:
							guardarPedido(parametro1, atoi(parametro2));
							break;
						case GUARDAR_PLATO:
							guardarPlato(parametro1, atoi(parametro2), parametro3, atoi(parametro4));
							break;
						case CONFIRMAR_PEDIDO:
							confirmarPedido(atoi(parametro1), parametro2);
							break;
						case PLATO_LISTO:
							platoListo(parametro1, atoi(parametro2), parametro3);
							break;
						case OBTENER_PEDIDO:
							obtenerPedido(parametro1, atoi(parametro2));
							break;		
						case ERROR:
						default:
							showInvalidCommandMsg(SINDICATO);
							break;
					}
					break;
				case OPTION_AIUDA:
					mostrarComandosValidos(parametro1);					
					break;
				case OPTION_BAI:
					printf("adiosss (๑♡3♡๑)!\n");
					break;
				case OPTION_CLEAR:
					limpiarPantalla();
					break;
				case ERROR:
				default:
					showInvalidMsg();
					break;
			}

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
	enviarPaquete(conexion, CLIENTE, CONSULTAR_RESTAURANTES, NULL);
    t_header *header = recibirHeaderPaquete(conexion);
    t_list *restaurantes = recibirPayloadPaquete(header, conexion);
	mostrarListaStrings(restaurantes);
	free(restaurantes);
	free(header);
}

void seleccionarRestaurante(int socketCliente, char *nombreRestaurante) {
	printf("recibi cliente: %d\n", socketCliente);
	printf("recibi rest: %s\n", nombreRestaurante);
 	// TODO: Recibe cliente (¿un id o un t_cliente?) y restaurante, retorna Ok/fail
	//enviarPaquete(conexion, CLIENTE, SELECCIONAR_RESTAURANTE, NULL);
}

void obtenerRestaurante(char *nombreRestaurante) {
	enviarPaquete(conexion, CLIENTE, OBTENER_RESTAURANTE, nombreRestaurante);
	t_header *header = recibirHeaderPaquete(conexion);
	md_restaurante *md = recibirPayloadPaquete(header, conexion);
	logMetadata(md);
	free(md);
	free(header);
}

void consultarPlatos(char *nombreRestaurante) {
	enviarPaquete(conexion, CLIENTE, CONSULTAR_PLATOS, nombreRestaurante);
	t_header *header = recibirHeaderPaquete(conexion);
	t_list *platos = recibirPayloadPaquete(header, conexion);
	mostrarListaStrings(platos);
	free(platos);
	free(header);
}

void crearPedido() {
	enviarPaquete(conexion, CLIENTE, CREAR_PEDIDO, NULL);

	t_header *header = recibirHeaderPaquete(conexion);
    int idPedido = recibirPayloadPaquete(header, conexion);

	//va a recibir un ID de pedido 
	printf("Respuesta obtenida: %d\n", idPedido);
}

void guardarPedido(char *nombreRestaurante, int idPedido) {
	t_request *reqGuardarPedido = malloc(sizeof(t_request));
	reqGuardarPedido->nombre = nombreRestaurante;
	reqGuardarPedido->idPedido = idPedido;

	enviarPaquete(conexion, CLIENTE, GUARDAR_PEDIDO, reqGuardarPedido);
	t_header *header = recibirHeaderPaquete(conexion);
	free(reqGuardarPedido);

	t_result *resultGuardarPedido = recibirPayloadPaquete(header, conexion);
	printf("%s\n", resultGuardarPedido->msg);
	printf("Tuvo error: %s\n", resultGuardarPedido->hasError ? "true" : "false");
	log_info(logger, "%s", resultGuardarPedido);
	free(resultGuardarPedido);
	free(header);
}

void aniadirPlato(char *nombrePlato, int idPedido) { 
	t_request *reqAniadir = malloc(sizeof(t_request));
	reqAniadir->nombre = nombrePlato;
	reqAniadir->idPedido = idPedido;

	enviarPaquete(conexion, CLIENTE, ANIADIR_PLATO, reqAniadir);
	t_header *header = recibirHeaderPaquete(conexion);
	free(reqAniadir);

	t_result *resultAniadir = recibirPayloadPaquete(header, conexion);
	printf("%s\n", resultAniadir->msg);
	printf("Tuvo error: %s\n", resultAniadir->hasError ? "true" : "false");
	log_info(logger, "%s", resultAniadir);
	free(resultAniadir);
	free(header);
}

void guardarPlato(char *nombreRestaurante, int idPedido, char *nombrePlato, int cantidadPlato) {
	t_req_plato *reqPlato = malloc(sizeof(t_req_plato));
	reqPlato->restaurante = nombreRestaurante;
	reqPlato->idPedido = idPedido;
	reqPlato->plato = nombrePlato;
	reqPlato->cantidadPlato = cantidadPlato;

	enviarPaquete(conexion, CLIENTE, GUARDAR_PLATO, reqPlato);
	t_header *header = recibirHeaderPaquete(conexion);
	free(reqPlato);

	t_result *resultGuardarPlato = recibirPayloadPaquete(header, conexion);
	printf("%s\n", resultGuardarPlato->msg);
	printf("Tuvo error: %s\n", resultGuardarPlato->hasError ? "true" : "false");
	log_info(logger, "%s", resultGuardarPlato);
	free(resultGuardarPlato);
	free(header);
}

void confirmarPedido(int idPedido, char *nombreRestaurante) {
	t_request *pedidoConf = malloc(sizeof(t_request));
	pedidoConf->idPedido = idPedido;
	pedidoConf->nombre = nombreRestaurante;

	enviarPaquete(conexion, CLIENTE, CONFIRMAR_PEDIDO, pedidoConf);
	t_header *header = recibirHeaderPaquete(conexion);
	free(pedidoConf);

	t_result *resultConfPedido = recibirPayloadPaquete(header, conexion);
	printf("%s\n", resultConfPedido->msg);
	printf("Tuvo error: %s\n", resultConfPedido->hasError ? "true" : "false");
	log_info(logger, "%s", resultConfPedido);
	free(resultConfPedido);
	free(header);
}

void platoListo(char *nombreRestaurante, int idPedido, char *plato) {
	 // TODO: struct que recibe restaurante, idPedido y plato
	//enviarPaquete(conexion, CLIENTE, PLATO_LISTO, params);
}

void consultarPedido(int idPedido) {
	// TODO: El model del TP incluye un restaurante, que falta agregar a nuestro t_pedido
	//enviarPaquete(conexion, CLIENTE, CONSULTAR_PEDIDO, idPedido);
}

void obtenerPedido(char *nombreRestaurante, int idPedido) {
	t_request *pedidoObt = malloc(sizeof(t_request));
	pedidoObt->nombre = nombreRestaurante;
	pedidoObt->idPedido = idPedido;

	enviarPaquete(conexion, CLIENTE, OBTENER_PEDIDO, pedidoObt);
	t_header *header = recibirHeaderPaquete(conexion);
	free(pedidoObt);

	t_pedido *pedidoCompleto = recibirPayloadPaquete(header, conexion);
	mostrarListaPlatos(pedidoCompleto->platos);
	free(pedidoCompleto);
	free(header);
}

void obtenerNombreServidor() {
	enviarPaquete(conexion, CLIENTE, OBTENER_PROCESO, NULL);
	t_header *rtaProceso = recibirHeaderPaquete(conexion);
	procesoConectado = intToPCode(recibirPayloadPaquete(rtaProceso, conexion));
	logConnectionCliente(procesoConectado);
	free(rtaProceso);
}

int main(int argc, char* argv[]) {
	inicializarProceso(CLIENTE);
    conexion = conectarseA(CLIENTE);
	obtenerNombreServidor();

	// Inicio del hilo de la consola y su lectura
	pthread_create(&threadConsola, NULL, (void *) threadLecturaConsola, NULL);
    pthread_detach(threadConsola);

	while (1) {
		//TODO: Lógica del Cliente
	}

	liberarConexion(conexion);
    finalizarProceso();
    return EXIT_SUCCESS;
}