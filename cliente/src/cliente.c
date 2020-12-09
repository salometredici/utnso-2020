#include "../include/cliente.h"

void *escuchar_updates(void *conexionNueva)
{
    pthread_data *t_data = (pthread_data*) conexionNueva;
    int socketCliente = t_data->socketThread;
    free(t_data);
	p_code proceso_conectado = ERROR;

	while (1) {
		
    	t_header *header = recibirHeaderPaquete(socketCliente);

		if (header->procesoOrigen == ERROR || header->codigoOperacion == ERROR) {
			log_disconnections_cliente(proceso_conectado, socketCliente);
			liberarConexion(socketCliente);
    		pthread_exit(EXIT_SUCCESS);
			return EXIT_FAILURE;
		} else {
			proceso_conectado = header->procesoOrigen;
		}

    	switch (header->codigoOperacion) {
			case OBTENER_PROCESO:;
				enviarPaquete(socketCliente, CLIENTE, RTA_OBTENER_PROCESO, CLIENTE);
				break;
			case PLATO_LISTO:;
                t_plato_listo *platoListo = recibirPayloadPaquete(header, socketCliente);
                log_PlatoListo(platoListo);
				t_result *result_plato_listo = getTResult("AH OK. slds", false);
				enviarPaquete(socketCliente, CLIENTE, RTA_PLATO_LISTO, result_plato_listo);
				free(result_plato_listo);
				free(platoListo);
            	break;
			case FINALIZAR_PEDIDO:;
				t_request *req_finalizar_pedido = recibirPayloadPaquete(header, socketCliente);
				log_FinalizarPedido(req_finalizar_pedido, header->codigoOperacion);
				t_result *result_finalizar_pedido = getTResult("AH OK. slds", false);
				enviarPaquete(socketCliente, CLIENTE, RTA_FINALIZAR_PEDIDO, result_finalizar_pedido);
				free(result_finalizar_pedido);
				free(req_finalizar_pedido);
				break;
        	default:
            	printf("Operación desconocida. Llegó el código: %d. No quieras meter la pata!!!(｀Д´*)\n", header->codigoOperacion);
            	break;
 	   	}
	}
    pthread_exit(EXIT_SUCCESS);
    return 0;
}

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
			log_console_input(comandoLeido);
			
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
							crear_pedido();
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
						case FINALIZAR_PEDIDO:
							finalizarPedido(parametro1, atoi(parametro2));
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
							crear_pedido();
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
							consultarPlatos(parametro1);
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
						case OBTENER_RECETA:
							obtener_receta(parametro1);
							break;
						case TERMINAR_PEDIDO:
							terminar_pedido(atoi(parametro1), parametro2);
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

void seleccionarRestaurante(char *idCliente, char *restaurante) {
	t_selecc_rest *seleccion = getSeleccRest(idCliente, restaurante);
	enviarPaquete(conexion, CLIENTE, SELECCIONAR_RESTAURANTE, seleccion);
	t_header *header = recibirHeaderPaquete(conexion);
	t_result *result = recibirPayloadPaquete(header, conexion);
	logTResult(result);
	free(result);
	free(header);
}

void obtenerRestaurante(char *nombre_restaurante) {
	enviarPaquete(conexion, CLIENTE, OBTENER_RESTAURANTE, nombre_restaurante);
	t_header *header = recibirHeaderPaquete(conexion);
	free(nombre_restaurante);

	t_md *md = recibirPayloadPaquete(header, conexion);
	log_rta_ObtenerRestaurante(md);
	free(md);
	free(header);
}

void consultarPlatos(char *nombre_rest) {
	enviarPaquete(conexion, CLIENTE, CONSULTAR_PLATOS, nombre_rest);
	t_header *header = recibirHeaderPaquete(conexion);
	t_list *platos = recibirPayloadPaquete(header, conexion);
	log_rta_ConsultarPlatos(platos);
	free(platos);
	free(header);
}

void obtener_receta(char *receta_a_buscar) {
	enviarPaquete(conexion, CLIENTE, OBTENER_RECETA, receta_a_buscar);
	t_header *header = recibirHeaderPaquete(conexion);
	free(receta_a_buscar);

	t_receta *receta = recibirPayloadPaquete(header, conexion);
	log_rta_ObtenerReceta(receta);
	free(receta);
	free(header);
}

void crear_pedido() {
	enviarPaquete(conexion, CLIENTE, CREAR_PEDIDO, NULL);
	t_header *header = recibirHeaderPaquete(conexion);
    int new_id_pedido = recibirPayloadPaquete(header, conexion);
	log_rta_CrearPedido(new_id_pedido);
	free(header);
}

void guardarPedido(char *nombreRestaurante, int idPedido) {
	t_request *req_guardar_pedido = getTRequest(idPedido, nombreRestaurante);

	enviarPaquete(conexion, CLIENTE, GUARDAR_PEDIDO, req_guardar_pedido);
	t_header *header = recibirHeaderPaquete(conexion);
	free(req_guardar_pedido);

	t_result *result_guardar_pedido = recibirPayloadPaquete(header, conexion);
	log_rta_GuardarPedido(result_guardar_pedido);
	free(result_guardar_pedido);
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

void guardarPlato(char *rest, int idPedido, char *plato, int cantPedida) {
	t_req_plato *req_guardar_plato = getTReqPlato(rest, idPedido, plato, cantPedida);

	enviarPaquete(conexion, CLIENTE, GUARDAR_PLATO, req_guardar_plato);
	t_header *header = recibirHeaderPaquete(conexion);
	free(req_guardar_plato);

	t_result *result_guardar_plato = recibirPayloadPaquete(header, conexion);
	log_rta_GuardarPlato(result_guardar_plato);
	free(result_guardar_plato);
	free(header);
}

void confirmarPedido(int idPedido, char *nombreRestaurante) {
	t_request *req_confirmar_pedido = getTRequest(idPedido, nombreRestaurante);

	enviarPaquete(conexion, CLIENTE, CONFIRMAR_PEDIDO, req_confirmar_pedido);
	t_header *header = recibirHeaderPaquete(conexion);
	free(req_confirmar_pedido);

	t_result *result_confirmar_pedido = recibirPayloadPaquete(header, conexion);
	log_rta_ConfirmarPedido(result_confirmar_pedido);
	free(result_confirmar_pedido);
	free(header);
}

void terminar_pedido(int idPedido, char *nombreRestaurante) {
	t_request *req_terminar_pedido = getTRequest(idPedido, nombreRestaurante);

	enviarPaquete(conexion, CLIENTE, TERMINAR_PEDIDO, req_terminar_pedido);
	t_header *header = recibirHeaderPaquete(conexion);
	free(req_terminar_pedido);

	t_result *result_terminar_pedido = recibirPayloadPaquete(header, conexion);
	log_rta_TerminarPedido(result_terminar_pedido);
	free(result_terminar_pedido);
	free(header);
}

void platoListo(char *rest, int idPedido, char *plato) {
	t_plato_listo *plato_listo = getTPlatoListo(rest, idPedido, plato);

	enviarPaquete(conexion, CLIENTE, PLATO_LISTO, plato_listo);
	t_header *header = recibirHeaderPaquete(conexion);
	free(plato_listo);
	
	t_result *result_plato_listo = recibirPayloadPaquete(header, conexion);
	log_rta_PlatoListo(result_plato_listo);
	free(result_plato_listo);
	free(header);
}

void consultarPedido(int idPedido) {
	enviarPaquete(conexion, CLIENTE, CONSULTAR_PEDIDO, idPedido);
	t_header *header = recibirHeaderPaquete(conexion);
	t_pedido *pedidoCons = recibirPayloadPaquete(header, conexion);
	log_rta_ConsultarPedido(pedidoCons, idPedido);
	free(pedidoCons);
	free(header);
}

void obtenerPedido(char *nombreRestaurante, int idPedido) {
	t_request *req_obtener_pedido = getTRequest(idPedido, nombreRestaurante);

	enviarPaquete(conexion, CLIENTE, OBTENER_PEDIDO, req_obtener_pedido);
	t_header *header = recibirHeaderPaquete(conexion);

	t_pedido *pedido_obtenido = recibirPayloadPaquete(header, conexion);
	log_rta_ObtenerPedido(pedido_obtenido, req_obtener_pedido);
	free(req_obtener_pedido);
	free(pedido_obtenido);
	free(header);
}

void finalizarPedido(char *nombreRestaurante, int idPedido) {
	t_request *req_fin_pedido = getTRequest(idPedido, nombreRestaurante);

	enviarPaquete(conexion, CLIENTE, FINALIZAR_PEDIDO, req_fin_pedido);
	t_header *header = recibirHeaderPaquete(conexion);
	free(req_fin_pedido);

	t_result *result_guardar_pedido = recibirPayloadPaquete(header, conexion);
	log_rta_GuardarPedido(result_guardar_pedido);
	free(result_guardar_pedido);
	free(header);
}

void obtenerNombreServidor() {
	enviarPaquete(conexion, CLIENTE, OBTENER_PROCESO, NULL);
	t_header *rtaProceso = recibirHeaderPaquete(conexion);
	procesoServidor = intToPCode(recibirPayloadPaquete(rtaProceso, conexion));
	log_connection_as_cliente(procesoServidor);
	free(rtaProceso);
}

void initVariablesGlobales() {
	dataCliente = malloc(sizeof(t_cliente));
	dataCliente->esRestaurante = false;
	dataCliente->restSelecc = string_new();
	dataCliente->posRest = malloc(sizeof(t_posicion));
	dataCliente->posRest->posX = ERROR;
	dataCliente->posRest->posY = ERROR;
	dataCliente->idCliente = config_get_string_value(config, "ID_CLIENTE");
	dataCliente->posCliente = malloc(sizeof(t_posicion));
	dataCliente->posCliente->posX = config_get_int_value(config, "POSICION_X");
	dataCliente->posCliente->posY = config_get_int_value(config, "POSICION_Y");
	dataCliente->ip_cliente = config_get_string_value(config, "IP");
	dataCliente->puerto_cliente = config_get_int_value(config, "PUERTO_ESCUCHA");
	dataCliente->socket_notifs = ERROR;
	dataCliente->socketCliente = ERROR;
	log_init_data_cliente(dataCliente);
}

void initCliente() {
    conexion = conectarseA(CLIENTE);
	obtenerNombreServidor();
	initVariablesGlobales();
	enviarPaquete(conexion, CLIENTE, ENVIAR_DATACLIENTE, dataCliente);
}

int main(int argc, char* argv[]) {
	inicializarProceso(CLIENTE);
	socketEscucha = iniciarServidor();
	initCliente();

	// Inicio del hilo de la consola y su lectura
	pthread_create(&threadConsola, NULL, (void *)threadLecturaConsola, NULL);
    pthread_detach(threadConsola);
	
	int fd;
	while (1) {
		fd = aceptarCliente(socketEscucha);
		if (fd != ERROR) {
			pthread_data *t_data = (pthread_data *) malloc(sizeof(*t_data));
			t_data->socketThread = fd;
			pthread_create(&threadUpdates, NULL, (void*)escuchar_updates, t_data);
			pthread_detach(threadUpdates);
			log_new_client_connection(fd);
		} else {
			pthread_kill(threadUpdates, SIGTERM);
		}
	}

	liberarConexion(conexion);
    finalizarProceso();
    return EXIT_SUCCESS;
}