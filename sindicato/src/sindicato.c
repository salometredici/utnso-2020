#include "../include/sindicato.h"

/* Consola */

void* thread_lectura_consola(void * args) {
    printf("Iniciando la consola ..."BREAK);
	printf("Para ver los comandos válidos, ingrese 'AIUDA'."BREAK);

	char **parametros;
	char *mensaje;
	int opcion;
    char *comandoLeido = readline(CYAN"(=^.^=)~>"RESET);

    while (1) {
		if (!string_is_empty(comandoLeido)) {
			add_history(comandoLeido);
			string_to_upper(comandoLeido);
			string_trim(&comandoLeido);

			// Parámetros
			parametros = string_split(comandoLeido, " ");
			mensaje = parametros[0];
			logConsoleInput(comandoLeido);

			opcion = validate_console_command(mensaje, parametros);

			switch (opcion) {
				case OPT_CREAR_RESTAURANTE:
					crear_restaurante(parametros);
					break;
				case OPT_CREAR_RECETA:
					crear_receta(parametros);
					break;
				case OPT_AIUDA:
					show_valid_commands();
					break;
				case OPT_BAI:
					printf("adiosss (๑♡3♡๑)!"BREAK);
					break;
				case OPT_CLEAR:
					limpiarPantalla();
					break;
				case ERROR:
				default:
					show_invalid_command_msg();
					break;
			}

			free(mensaje);
			free(parametros);
			free(comandoLeido);
			if (opcion == OPT_BAI) { break; }
		}
		comandoLeido = readline(CYAN"(=^.^=)~>"RESET);
	}

    pthread_exit(EXIT_SUCCESS);
    return 0;
}

/* Conexiones */

void *atender_conexiones(void *conexionNueva)
{
    pthread_data *t_data = (pthread_data*) conexionNueva;
    int socketCliente = t_data->socketThread;
    free(t_data);

	while (1) {

		t_header *header = recibirHeaderPaquete(socketCliente);

		if (header->procesoOrigen == ERROR || header->codigoOperacion == ERROR) {
			logClientDisconnection(socketCliente);
			liberarConexion(socket);
    		pthread_exit(EXIT_SUCCESS);
			return EXIT_FAILURE;
		}	

		switch (header->codigoOperacion) {
			case OBTENER_PROCESO:;
				enviarPaquete(socketCliente, SINDICATO, RTA_OBTENER_PROCESO, SINDICATO);
				break;
			case OBTENER_RESTAURANTE:;
				char *nombre_restaurante = recibirPayloadPaquete(header, socketCliente);
				log_ObtenerRestaurante(nombre_restaurante);
				t_md *md;
				if (!existe_restaurante(nombre_restaurante)) {
					md = getEmptyMd();
				} else {
					md = obtener_restaurante(nombre_restaurante);
				}
				enviarPaquete(socketCliente, SINDICATO, RTA_OBTENER_RESTAURANTE, md);
				free(nombre_restaurante); free(md);
				break;
			case CONSULTAR_PLATOS:;
				char *rest_consulta = recibirPayloadPaquete(header, socketCliente);
				log_ConsultarPlatos(rest_consulta);
				t_list *platos_restaurante = list_create();
				if (!existe_restaurante(rest_consulta)) {
					list_add(platos_restaurante, REST_NO_EXISTE);
				} else {
					list_add_all(platos_restaurante, obtener_platos_restaurante(rest_consulta));
				}
				enviarPaquete(socketCliente, SINDICATO, RTA_CONSULTAR_PLATOS, platos_restaurante);
				free(rest_consulta); free(platos_restaurante);
				break;
			case GUARDAR_PEDIDO:;
				t_request *req_guardar_pedido = recibirPayloadPaquete(header, socketCliente);
				logRequest(req_guardar_pedido, header->codigoOperacion);
				t_result *result_guardar_pedido;
				if (!existe_restaurante(req_guardar_pedido->nombre)) {
					 result_guardar_pedido = getTResult(REST_NO_EXISTE, true);
				} else if (!existe_pedido(req_guardar_pedido)) {
					crear_pedido(req_guardar_pedido);
					result_guardar_pedido = getTResult(PEDIDO_CREADO, false);
				} else {
					result_guardar_pedido = getTResult(YA_EXISTE_PEDIDO, true);
				}
				enviarPaquete(socketCliente, SINDICATO, RTA_GUARDAR_PEDIDO, result_guardar_pedido);
				free(req_guardar_pedido); free(result_guardar_pedido);	
				break;
			case GUARDAR_PLATO:;
				t_req_plato *req_guardar_plato = recibirPayloadPaquete(header, socketCliente);
				log_GuardarPlato(req_guardar_plato);
				t_result *result_guardar_plato;
				if (!existe_restaurante(req_guardar_plato->restaurante)) {
					result_guardar_plato = getTResult(REST_NO_EXISTE, true);
				} else if (sabe_preparar_plato_restaurante(req_guardar_plato)) {
					result_guardar_plato = check_and_add_plato(req_guardar_plato);
				} else {
					result_guardar_plato = getTResult(NO_CONOCE_PLATO, true);
				}
				enviarPaquete(socketCliente, SINDICATO, RTA_GUARDAR_PLATO, result_guardar_plato);
				free(req_guardar_pedido); free(result_guardar_plato);
				break;
			case CONFIRMAR_PEDIDO:;
				t_request *reqConf = recibirPayloadPaquete(header, socketCliente);
				logRequest(reqConf, header->codigoOperacion);
				free(reqConf);

				// TODO:
				// 1. Verificar si R existe en FS... etc.
				// 2. Verificar si el Pedido existe en FS, buscando en dir de R si existe el Pedido - Si no existe informarlo
				// 3. Verificar que el Pedido esté en estado "Pendiente" - En caso contrario informar situación
				// 4. Cambiar el estado del Pedido de "Pendiente" a "Confirmado" - Truncar el archivo de ser necesario
				// 5. Responder el mensaje con Ok/fail

				t_result *resCP = malloc(sizeof(t_result));
				resCP->msg = "[CONFIRMAR_PEDIDO] OK";
				resCP->hasError = false;
				enviarPaquete(socketCliente, SINDICATO, RTA_CONFIRMAR_PEDIDO, resCP);
				break;
			case OBTENER_PEDIDO:;
				t_request *req_obtener_pedido = recibirPayloadPaquete(header, socketCliente);
				log_ObtenerPedido(req_obtener_pedido, header->codigoOperacion);
				t_pedido *pedido;
				if (!existe_restaurante(req_obtener_pedido->nombre)) {
					pedido = getEmptyPedido(REST_INEXISTENTE);
				} else if (!existe_pedido(req_obtener_pedido)) {
					pedido = getEmptyPedido(PEDIDO_INEXISTENTE);
				} else {
					pedido = obtener_pedido(req_obtener_pedido);
				}
				printf("%s\n", list_get(pedido->platos, 0));
				enviarPaquete(socketCliente, SINDICATO, RTA_OBTENER_PEDIDO, pedido);
				free(req_obtener_pedido); free(pedido);
				break;
			case PLATO_LISTO:; // TODO
				break;
			case TERMINAR_PEDIDO:; // TODO: Recibe idPedido y restaurante, retorna Ok/fail
				break;
			case OBTENER_RECETA:;
				char *receta_a_buscar = recibirPayloadPaquete(header, socketCliente);
				log_ObtenerReceta(receta_a_buscar);
				t_receta *receta;
				if (!existe_receta(receta_a_buscar)) {
					receta = getEmptyRecipe();
				} else {
					receta = obtener_receta(receta_a_buscar);
				}
				enviarPaquete(socketCliente, SINDICATO, RTA_OBTENER_RECETA_2, receta);
				free(receta_a_buscar); free(receta);
				break;
			default:
				printf("Operación desconocida. Llegó el código: %d. No quieras meter la pata!!!(｀Д´*)"BREAK, header->codigoOperacion);
				break;
		}
		free(header);
	}
    pthread_exit(EXIT_SUCCESS);
    return 0;
}

int main(int argc, char **argv)
{
    inicializarProceso(SINDICATO);
	socketServidor = iniciarServidor();

	init();

	// Inicio del hilo de la consola y su lectura
	pthread_create(&threadConsola, NULL, (void *)thread_lectura_consola, NULL);
    pthread_detach(threadConsola);

	int fd;
	while (1) {
		fd = aceptarCliente(socketServidor);
		if (fd != ERROR) {
			// Creo un nuevo hilo para la conexión aceptada
			pthread_data *t_data = (pthread_data *) malloc(sizeof(*t_data));
			t_data->socketThread = fd;
			pthread_create(&threadConexiones, NULL, (void*)atender_conexiones, t_data);
			pthread_detach(threadConexiones);
			logNewClientConnection(fd);
		} else {
			pthread_kill(threadConexiones, SIGTERM);
		}
	}

    finalizarProceso();
    return EXIT_SUCCESS;
}