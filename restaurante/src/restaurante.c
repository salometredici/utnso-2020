#include "../include/restaurante.h"

void *atenderConexiones(void *conexionNueva)
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
			case CONSULTAR_PLATOS:;
				char *restConsulta = recibirPayloadPaquete(header, socketCliente);
				logConsultaPlatos(restConsulta);
				t_list *platosRest = list_create();
				list_add(platosRest, "Milanesas");
				list_add(platosRest, "Lasagna");
				list_add(platosRest, "Asado");
				enviarPaquete(socketCliente, RESTAURANTE, RTA_CONSULTAR_PLATOS, platosRest);
				free(platosRest);
				break;
			case CREAR_PEDIDO:;
				int newIdPedido = 77;
				enviarPaquete(socketCliente, RESTAURANTE, RTA_CREAR_PEDIDO, newIdPedido);
				break;
			case ANIADIR_PLATO:; // TODO
				break;
			case CONFIRMAR_PEDIDO:;
				t_req_pedido *reqConf = recibirPayloadPaquete(header, socketCliente);
				logRequestPedido(reqConf);
				free(reqConf);
				// TODO: t_result
				char *msjConfPedido = "[CONFIRMAR_PEDIDO] Ok";
				enviarPaquete(socketCliente, SINDICATO, RTA_CONFIRMAR_PEDIDO, msjConfPedido);
				break;
			case CONSULTAR_PEDIDO: // TODO: El model del TP incluye un restaurante, que falta agregar a nuestro t_pedido
				break;
			default:
				printf("Operación desconocida. Llegó el código: %d. No quieras meter la pata!!!(｀Д´*)\n", header->codigoOperacion);
				break;
		}
		free(header);
	}

    pthread_exit(EXIT_SUCCESS);
    return 0;
}

int main(int argc, char *argv[])
{
	inicializarProceso(RESTAURANTE);
	socketServidor = iniciarServidor();
	conexionSindicato = conectarseA(SINDICATO);
	nombreRestaurante = obtenerNombreRestaurante();

	/* Obtener MD del Restaurante */

	enviarPaquete(conexionSindicato, RESTAURANTE, OBTENER_RESTAURANTE, nombreRestaurante);
	t_header *header = recibirHeaderPaquete(conexionSindicato);
	t_posicion *pos = recibirPayloadPaquete(header, conexionSindicato);
	logMetadata(pos);
	free(pos);

	/* Prueba de GUARDAR_PEDIDO */

	t_req_pedido *reqGuardarPedido = malloc(sizeof(t_req_pedido));
	reqGuardarPedido->restaurante = nombreRestaurante;
	reqGuardarPedido->idPedido = 777;

	enviarPaquete(conexionSindicato, RESTAURANTE, GUARDAR_PEDIDO, reqGuardarPedido);
	header = recibirHeaderPaquete(conexionSindicato);
	free(reqGuardarPedido);

	char *resultGuardarPedido = recibirPayloadPaquete(header, conexionSindicato);
	printf("%s", resultGuardarPedido);
	log_info(logger, "%s", resultGuardarPedido);

	/* Prueba de CONSULTAR_PLATOS */

	enviarPaquete(conexionSindicato, RESTAURANTE, CONSULTAR_PLATOS, nombreRestaurante);
	header = recibirHeaderPaquete(conexionSindicato);
	t_list *platos = recibirPayloadPaquete(header, conexionSindicato);
	mostrarListaStrings(platos);
	free(platos);

	/* Prueba de GUARDAR_PLATO */

	t_req_plato *reqPlato = malloc(sizeof(t_req_plato));
	reqPlato->restaurante = nombreRestaurante;
	reqPlato->plato = "Empanadas";
	reqPlato->cantidadPlato = 3;
	reqPlato->idPedido = 777;

	enviarPaquete(conexionSindicato, RESTAURANTE, GUARDAR_PLATO, reqPlato);
	header = recibirHeaderPaquete(conexionSindicato);
	free(reqPlato);

	char *resultGuardarPlato = recibirPayloadPaquete(header, conexionSindicato);
	printf("%s", resultGuardarPlato);
	log_info(logger, "%s", resultGuardarPlato);

	/* Prueba de CONFIRMAR_PEDIDO */

	t_req_pedido *pedidoConf = malloc(sizeof(t_req_pedido));
	pedidoConf->restaurante = nombreRestaurante;
	pedidoConf->idPedido = 777;

	enviarPaquete(conexionSindicato, RESTAURANTE, CONFIRMAR_PEDIDO, pedidoConf);
	header = recibirHeaderPaquete(conexionSindicato);
	free(pedidoConf);

	char *resultConfPedido = recibirPayloadPaquete(header, conexionSindicato);
	printf("%s", resultConfPedido);
	log_info(logger, "%s", resultConfPedido);

	/* Prueba de OBTENER_PEDIDO */

	t_req_pedido *pedidoObt = malloc(sizeof(t_req_pedido));
	pedidoObt->restaurante = nombreRestaurante;
	pedidoObt->idPedido = 777;

	enviarPaquete(conexionSindicato, RESTAURANTE, OBTENER_PEDIDO, pedidoObt);
	header = recibirHeaderPaquete(conexionSindicato);
	free(pedidoObt);

	t_pedido *pedidoCompleto = recibirPayloadPaquete(header, conexionSindicato);
	mostrarListaPlatos(pedidoCompleto->platos);
	free(pedidoCompleto);

	// Creación de las distintas colas de planificación
		//TODO

	// Inicio del bucle que va a generar los diferentes hilos de conexión
	int fd;
	while (1) {
		fd = aceptarCliente(socketServidor);
		if (fd != -1) {
			// Creo un nuevo hilo para la conexión aceptada
			pthread_data *t_data = (pthread_data *) malloc(sizeof(*t_data));
			t_data->socketThread = fd;
			pthread_create(&threadConexiones, NULL, (void*)atenderConexiones, t_data);
			pthread_detach(threadConexiones);
			logNewClientConnection(fd);
		} else {
			pthread_kill(threadConexiones, SIGTERM);
		}
	}
	
	liberarConexion(socketServidor);
    finalizarProceso();
    return EXIT_SUCCESS;
}