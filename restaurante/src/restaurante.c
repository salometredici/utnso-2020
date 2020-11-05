#include "../include/restaurante.h"




void *atenderConexiones(void *conexionNueva)
{
    pthread_data *t_data = (pthread_data*) conexionNueva;
    int socketCliente = t_data->socketThread;
    free(t_data);

	int socketSindicato = ERROR;

	while (1) {

		t_header *header = recibirHeaderPaquete(socketCliente);

		if (header->procesoOrigen == ERROR || header->codigoOperacion == ERROR) {
			logClientDisconnection(socketCliente);
			liberarConexion(socket);
    		pthread_exit(EXIT_SUCCESS);
			return EXIT_FAILURE;
		}

		socketSindicato = conectarseA(SINDICATO);

		switch (header->codigoOperacion) {
			case OBTENER_PROCESO:;
				enviarPaquete(socketCliente, RESTAURANTE, RTA_OBTENER_PROCESO, RESTAURANTE);
				break;
			case CONSULTAR_PLATOS:;
				char *restConsulta = recibirPayloadPaquete(header, socketCliente); free(restConsulta);
				// Se consultan los platos de este restaurante a Sindicato
				enviarPaquete(conexionSindicato, RESTAURANTE, CONSULTAR_PLATOS, nombreRestaurante);
				t_header *hConsulta = recibirHeaderPaquete(conexionSindicato);
				t_list *platosRest = recibirPayloadPaquete(hConsulta, conexionSindicato);
				free(hConsulta);
				// Se contesta con los platos obtenidos
				enviarPaquete(socketCliente, RESTAURANTE, RTA_CONSULTAR_PLATOS, platosRest);
				free(platosRest);
				break;
			case CREAR_PEDIDO:;
				int newIdPedido = 77;
				enviarPaquete(socketCliente, RESTAURANTE, RTA_CREAR_PEDIDO, newIdPedido);
				break;
			case ANIADIR_PLATO:;
				t_request *reqAniadir = recibirPayloadPaquete(header, socketCliente);
				logRequest(reqAniadir, header->codigoOperacion);
				free(reqAniadir);

				t_result *resAP = malloc(sizeof(t_result));
				resAP->hasError = false;
				resAP->msg = "[ANIADIR_PLATO] OK";
				enviarPaquete(socketCliente, RESTAURANTE, RTA_ANIADIR_PLATO, resAP);
				free(resAP);
				break;
			case CONFIRMAR_PEDIDO:;
				t_request *reqConf = recibirPayloadPaquete(header, socketCliente);
				logRequest(reqConf, header->codigoOperacion);
				// free(reqConf);

				// Sólo recibe el ID del pedido
				// 1. Obtener el Pedido desde Sindicato

				reqConf->nombre = nombreRestaurante;
				enviarPaquete(conexionSindicato, RESTAURANTE, OBTENER_PEDIDO, reqConf);
				t_header *hRConf = recibirHeaderPaquete(conexionSindicato);
				t_pedido *pedidoConf = recibirPayloadPaquete(header, conexionSindicato);
				mostrarListaPlatos(pedidoConf->platos);
				free(hRConf);

				// 2. Generar PCB de cada plato y dejarlo en el ciclo de planificación
				// Obtener receta de Sindicato para saber trazabilidad al momento de ejecución
				// El número de pedido se deberá guardar dentro del PCB

				



				// 3. Informar a quien lo invocó que su pedido fue confirmado

				t_result *resCP = malloc(sizeof(t_result));
				resCP->hasError = false;
				resCP->msg = "[CONFIRMAR_PEDIDO] OK";
				enviarPaquete(socketCliente, SINDICATO, RTA_CONFIRMAR_PEDIDO, resCP);
				free(resCP);
				break;
			case CONSULTAR_PEDIDO: // TODO: El model del TP incluye un restaurante, que falta agregar a nuestro t_pedido
				break;
			default:
				printf("Operación desconocida. Llegó el código: %d. No quieras meter la pata!!!(｀Д´*)\n", header->codigoOperacion);
				break;
		}
		free(header);
	}

	liberarConexion(socketSindicato); // si no es error, lo liberamo'
    pthread_exit(EXIT_SUCCESS);
    return 0;
}

int main(int argc, char *argv[])
{
	inicializarProceso(RESTAURANTE);
	socketServidor = iniciarServidor();
	initRestaurante();








	/* Prueba de GUARDAR_PEDIDO */

	t_request *reqGuardarPedido = malloc(sizeof(t_request));
	reqGuardarPedido->nombre = nombreRestaurante;
	reqGuardarPedido->idPedido = 777;

	enviarPaquete(conexionSindicato, RESTAURANTE, GUARDAR_PEDIDO, reqGuardarPedido);
	t_header *header = recibirHeaderPaquete(conexionSindicato);
	free(reqGuardarPedido);

	t_result *resultGuardarPedido = recibirPayloadPaquete(header, conexionSindicato);
	printf("%s\n", resultGuardarPedido->msg);
	printf("Tuvo error: %s\n", resultGuardarPedido->hasError ? "true" : "false");
	log_info(logger, "%s", resultGuardarPedido->msg);

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

	t_result *resultGuardarPlato = recibirPayloadPaquete(header, conexionSindicato);
	printf("%s\n", resultGuardarPlato->msg);
	printf("Tuvo error: %s\n", resultGuardarPlato->hasError ? "true" : "false");
	log_info(logger, "%s", resultGuardarPlato);

	/* Prueba de CONFIRMAR_PEDIDO */

	t_request *pedidoConf = malloc(sizeof(t_request));
	pedidoConf->nombre = nombreRestaurante;
	pedidoConf->idPedido = 777;

	enviarPaquete(conexionSindicato, RESTAURANTE, CONFIRMAR_PEDIDO, pedidoConf);
	header = recibirHeaderPaquete(conexionSindicato);
	free(pedidoConf);

	t_result *resultConfPedido = recibirPayloadPaquete(header, conexionSindicato);
	printf("%s\n", resultConfPedido->msg);
	printf("Tuvo error: %s\n", resultConfPedido->hasError ? "true" : "false");
	log_info(logger, "%s", resultConfPedido);

	/* Prueba de OBTENER_PEDIDO */

	t_request *pedidoObt = malloc(sizeof(t_request));
	pedidoObt->nombre = nombreRestaurante;
	pedidoObt->idPedido = 777;

	enviarPaquete(conexionSindicato, RESTAURANTE, OBTENER_PEDIDO, pedidoObt);
	header = recibirHeaderPaquete(conexionSindicato);
	free(pedidoObt);

	t_pedido *pedidoCompleto = recibirPayloadPaquete(header, conexionSindicato);
	mostrarListaPlatos(pedidoCompleto->platos);
	free(pedidoCompleto);

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