#include "../include/sindicato.h"

/* Consola */

void* thread_lectura_consola(void * args) {
    printf("Iniciando la consola ..."BREAK);
	printf("Para ver los comandos válidos, ingrese 'AIUDA'."BREAK);

	char **parametros;
	char *mensaje;
	int opcion;
    char *comandoLeido = readline("(=^.^=)~>");

    while (1) {
		if (!string_is_empty(comandoLeido)) {
			add_history(comandoLeido);
			string_to_upper(comandoLeido);
			string_trim(&comandoLeido);

			// Parámetros
			parametros = string_split(comandoLeido, " ");
			mensaje = parametros[0];
			log_console_input(comandoLeido);

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
		}
		comandoLeido = readline("(=^.^=)~>");
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
			log_common_client_disconnection(socketCliente);
			liberarConexion(socket);
    		pthread_exit(EXIT_SUCCESS);
			return EXIT_FAILURE;
		}	

		switch (header->codigoOperacion) {
			case OBTENER_PROCESO:;
				enviarPaquete(socketCliente, SINDICATO, RTA_OBTENER_PROCESO, SINDICATO);
				break;
			case ENVIAR_NOMBRE:;
				char *cliente = recibirPayloadPaquete(header, socketCliente);
				log_EnviarNombre(cliente);
				free(cliente);
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
				} else if (!sabe_preparar_plato_restaurante(req_guardar_plato)) {
					result_guardar_plato = getTResult(NO_CONOCE_PLATO, true);
				} else {
					result_guardar_plato = check_and_add_plato(req_guardar_plato);
				}

				enviarPaquete(socketCliente, SINDICATO, RTA_GUARDAR_PLATO, result_guardar_plato);
				free(req_guardar_pedido); free(result_guardar_plato);
				break;
			case CONFIRMAR_PEDIDO:;
				t_request *req_confirmar_pedido = recibirPayloadPaquete(header, socketCliente);
				logRequest(req_confirmar_pedido, header->codigoOperacion);
				t_result *result_confirmar_pedido;

				if (!existe_restaurante(req_confirmar_pedido->nombre)) {
					result_confirmar_pedido = getTResult(REST_NO_EXISTE, true);
				} else if (!existe_pedido(req_confirmar_pedido)) {
					result_confirmar_pedido = getTResult(PEDIDO_NO_EXISTE, true);
				} else {
					result_confirmar_pedido = check_and_confirm_pedido(req_confirmar_pedido);
				}

				enviarPaquete(socketCliente, SINDICATO, RTA_CONFIRMAR_PEDIDO, result_confirmar_pedido);
				free(req_confirmar_pedido); free(result_confirmar_pedido);
				break;
			case OBTENER_PEDIDO:;
				t_request *req_obtener_pedido = recibirPayloadPaquete(header, socketCliente);
				log_ObtenerPedido(req_obtener_pedido, header->codigoOperacion);
				t_pedido *pedido;

				if (!existe_restaurante(req_obtener_pedido->nombre)) {
					pedido = getEmptyPedido_with_error(REST_INEXISTENTE);
				} else if (!existe_pedido(req_obtener_pedido)) {
					pedido = getEmptyPedido_with_error(PEDIDO_INEXISTENTE);
				} else {
					pedido = obtener_pedido(req_obtener_pedido);
				}

				enviarPaquete(socketCliente, SINDICATO, RTA_OBTENER_PEDIDO, pedido);
				free(req_obtener_pedido); free(pedido);
				break;
			case PLATO_LISTO:;
				t_plato_listo *plato_listo = recibirPayloadPaquete(header, socketCliente);
				log_PlatoListo(plato_listo);
				t_result *result_plato_listo;
				t_request *req_plato_listo = getTRequest(plato_listo->idPedido, plato_listo->restaurante);

				if (!existe_restaurante(plato_listo->restaurante)) {
					result_plato_listo = getTResult(REST_INEXISTENTE, true);
				} else if (!existe_pedido(req_plato_listo)) {
					result_plato_listo = getTResult(PEDIDO_NO_EXISTE, true);
				} else {
					result_plato_listo = check_and_set_plato_listo(plato_listo, req_plato_listo);
				}

				enviarPaquete(socketCliente, SINDICATO, RTA_PLATO_LISTO, result_plato_listo);
				free(plato_listo); free(req_plato_listo); free(result_plato_listo);
				break;
			case TERMINAR_PEDIDO:;
				t_request *req_terminar_pedido = recibirPayloadPaquete(header, socketCliente);
				log_TerminarPedido(req_terminar_pedido, header->codigoOperacion);
				t_result *result_terminar_pedido;

				if (!existe_restaurante(req_terminar_pedido->nombre)) {
					result_terminar_pedido = getTResult(REST_INEXISTENTE, true);
				} else if (!existe_pedido(req_terminar_pedido)) {
					result_terminar_pedido = getTResult(PEDIDO_NO_EXISTE, true);
				} else {
					result_terminar_pedido = check_and_terminar_pedido(req_terminar_pedido);
				}

				enviarPaquete(socketCliente, SINDICATO, RTA_TERMINAR_PEDIDO, result_terminar_pedido);
				free(req_terminar_pedido); free(result_terminar_pedido);
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
			log_new_client_connection(fd);
		} else {
			pthread_kill(threadConexiones, SIGTERM);
		}
	}

    finalizarProceso();
    return EXIT_SUCCESS;
}