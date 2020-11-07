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
			opcion = clientOptionToKey(getStringKeyValue(procesoServidor, PROCNKEYS));
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
							seleccionarRestaurante(dataCliente->idCliente, parametro1);
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

void seleccionarRestaurante(char *idCliente, char *nombreRestaurante) {
	t_selecc_rest *seleccion = malloc(sizeof(t_selecc_rest));
	seleccion->idCliente = idCliente;
	seleccion->restauranteSeleccionado = nombreRestaurante;
	enviarPaquete(conexion, CLIENTE, SELECCIONAR_RESTAURANTE, seleccion);
	t_header *header = recibirHeaderPaquete(conexion);
	t_result *result = recibirPayloadPaquete(header, conexion);
	logTResult(result);
	free(result);
	free(header);
}

void obtenerRestaurante(char *nombreRestaurante) {
	enviarPaquete(conexion, CLIENTE, OBTENER_RESTAURANTE, nombreRestaurante);
	t_header *header = recibirHeaderPaquete(conexion);
	t_md *md = recibirPayloadPaquete(header, conexion);
	logMetadata(md);
	free(md);
	free(header);
}

void consultarPlatos(char *nombreRestaurante) { // Es un string vacío
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
	printf("Se ha creado el pedido #%d\n", idPedido);
	log_info(logger, "Se ha creado el pedido #%d", idPedido);
	free(header);
}

void guardarPedido(char *nombreRestaurante, int idPedido) {
	t_request *reqGuardarPedido = malloc(sizeof(t_request));
	reqGuardarPedido->nombre = nombreRestaurante;
	reqGuardarPedido->idPedido = idPedido;

	enviarPaquete(conexion, CLIENTE, GUARDAR_PEDIDO, reqGuardarPedido);
	t_header *header = recibirHeaderPaquete(conexion);
	free(reqGuardarPedido);

	t_result *resultGP = recibirPayloadPaquete(header, conexion);
	logTResult(resultGP);
	free(resultGP);
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
	logTResult(resultAniadir);
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
	logTResult(resultGuardarPlato);
	free(resultGuardarPlato);
	free(header);
}

void confirmarPedido(int idPedido, char *nombreRestaurante) {
	t_request *pedidoConf = malloc(sizeof(t_request));
	pedidoConf->idPedido = idPedido;
	pedidoConf->nombre = nombreRestaurante; // Va a ser un string vacío

	enviarPaquete(conexion, CLIENTE, CONFIRMAR_PEDIDO, pedidoConf);
	t_header *header = recibirHeaderPaquete(conexion);
	free(pedidoConf);

	t_result *resultConfPedido = recibirPayloadPaquete(header, conexion);
	logTResult(resultConfPedido);
	free(resultConfPedido);
	free(header);
}

void platoListo(char *nombreRestaurante, int idPedido, char *plato) {
	t_plato_listo *platoListo = malloc(sizeof(t_plato_listo));
	platoListo->restaurante = nombreRestaurante;
	platoListo->idPedido = idPedido;
	platoListo->plato = plato;
	enviarPaquete(conexion, CLIENTE, PLATO_LISTO, platoListo);
	t_header *header = recibirHeaderPaquete(conexion);
	t_result *resPL = recibirPayloadPaquete(header, conexion);
	logTResult(resPL);
	free(resPL);
	free(header);
}

void consultarPedido(int idPedido) {
	enviarPaquete(conexion, CLIENTE, CONSULTAR_PEDIDO, idPedido);
	t_header *header = recibirHeaderPaquete(conexion);
	t_pedido *pedidoCons = recibirPayloadPaquete(header, conexion);
	logConsultarPedido(pedidoCons, idPedido);
	free(pedidoCons);
	free(header);
}

void obtenerPedido(char *nombreRestaurante, int idPedido) {
	t_request *pedidoObt = malloc(sizeof(t_request));
	pedidoObt->nombre = nombreRestaurante;
	pedidoObt->idPedido = idPedido;

	enviarPaquete(conexion, CLIENTE, OBTENER_PEDIDO, pedidoObt);
	t_header *header = recibirHeaderPaquete(conexion);
	free(pedidoObt);

	t_pedido *pedidoCompleto = recibirPayloadPaquete(header, conexion);
	logObtenerPedido(pedidoCompleto, idPedido);
	free(pedidoCompleto);
	free(header);
}

void obtenerNombreServidor() {
	enviarPaquete(conexion, CLIENTE, OBTENER_PROCESO, NULL);
	t_header *rtaProceso = recibirHeaderPaquete(conexion);
	procesoServidor = intToPCode(recibirPayloadPaquete(rtaProceso, conexion));
	logConnectionCliente(procesoServidor);
	free(rtaProceso);
}

void initVariablesGlobales() {
	dataCliente = malloc(sizeof(t_cliente));
	dataCliente->esRestaurante = false;
	dataCliente->restSeleccionado = string_new();
	dataCliente->idCliente = config_get_string_value(config, "ID_CLIENTE");
	dataCliente->posCliente = malloc(sizeof(t_posicion));
	dataCliente->posCliente->posX = config_get_int_value(config, "POSICION_X");
	dataCliente->posCliente->posY = config_get_int_value(config, "POSICION_Y");
	logInitDataCliente(dataCliente);
}

void initCliente() {
    conexion = conectarseA(CLIENTE);
	obtenerNombreServidor();
	initVariablesGlobales();
	if (procesoServidor == APP) { enviarPaquete(conexion, CLIENTE, ENVIAR_DATACLIENTE, dataCliente); }
}

int main(int argc, char* argv[]) {
	inicializarProceso(CLIENTE);
	initCliente();

	// Inicio del hilo de la consola y su lectura
	pthread_create(&threadConsola, NULL, (void *)threadLecturaConsola, NULL);
    pthread_detach(threadConsola);

	while (1) {
		//TODO: Lógica del Cliente
	}

	liberarConexion(conexion);
    finalizarProceso();
    return EXIT_SUCCESS;
}