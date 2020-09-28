#include "../include/restaurante.h"

void iterator(char *value) {
	printf("%s\n", value);
}

void *atenderConexiones(void *conexionNueva) {
    pthread_data *t_data = (pthread_data*) conexionNueva;
    int socket = t_data->socketThread;
    free(t_data);
	
	t_list *lista;
	while(1) {
		t_buffer *payload; // Ver cómo liberar este payload después
		t_header *header = recibirHeaderPaquete(socket);

		if (header->procesoOrigen == ERROR) { // Ver cómo manejar esta desconexión
			log_error("El cliente %d se desconectó. Finalizando hilo...╮(╯_╰)╭", socket);
			liberarConexion(socket);
    		pthread_exit(EXIT_SUCCESS);
			return EXIT_FAILURE;
		}

		switch (header->codigoOperacion) { // Mensajes y/o rtas que puede manejar Restaurante
			case RTA_OBTENER_RESTAURANTE:
				payload = recibirPayloadPaquete(header, socket);
				printf("Me llegó un payload de tamaño %d, Nombre del restaurante: %s\n", payload->size , payload->stream);
				break;
			case ERROR:
				log_error(logger, "El cliente %d se desconectó. Finalizando el hilo...╮(╯_╰)╭\n", socket);
				return EXIT_FAILURE;
			
			// Mensajes desde CLIENTE
			case CONSULTAR_PLATOS:
				printf("Operacion recibida: CONSULTAR_PLATOS\n");

				payload = recibirPayloadPaquete(header, socket);
				printf("Parametro recibido: %s\n", payload->stream);
				
				respuesta = "Mensaje de respuesta a CONSULTAR_PLATOS";
				enviarPaquete(socket, RESTAURANTE, RTA_CONSULTAR_PLATOS, respuesta);
				break;
			case CREAR_PEDIDO:
				printf("Operacion recibida: CREAR_PEDIDO\n");

				printf("No se recibieron parametros\n");
				
				respuesta = "Mensaje de respuesta a CREAR_PEDIDO";
				enviarPaquete(socket, RESTAURANTE, RTA_CREAR_PEDIDO, respuesta);
				break;
			case ANIADIR_PLATO:
				printf("Operacion recibida: ANIADIR_PLATO\n");

				payload = recibirPayloadPaquete(header, socket);
				printf("Parametros recibidos:\n");
				mostrarListaStrings(payload->stream);

				respuesta = "Mensaje de respuesta a ANIADIR_PLATO";
				enviarPaquete(socket, RESTAURANTE, RTA_ANIADIR_PLATO, respuesta);
				break;
			case CONFIRMAR_PEDIDO:
				printf("Operacion recibida: CONFIRMAR_PEDIDO\n");

				payload = recibirPayloadPaquete(header, socket);
				printf("Parametro recibido: %s\n", payload->stream);
				
				respuesta = "Mensaje de respuesta a CONFIRMAR_PEDIDO";
				enviarPaquete(socket, RESTAURANTE, RTA_CONFIRMAR_PEDIDO, respuesta);
				break;
			case CONSULTAR_PEDIDO:
				printf("Operacion recibida: CONSULTAR_PEDIDO\n");

				payload = recibirPayloadPaquete(header, socket);
				printf("Parametro recibido: %s\n", payload->stream);
				
				respuesta = "Mensaje de respuesta a CONSULTAR_PEDIDO";
				enviarPaquete(socket, RESTAURANTE, RTA_CONSULTAR_PEDIDO, respuesta);
				break;
			default:
				printf("Operacion desconocida. No quieras meter la pata!!!\n");
				break;
		}
	}

    pthread_exit(EXIT_SUCCESS);
    return 0;
}

int main(int argc, char* argv[]) {
	inicializarProceso(RESTAURANTE);
	socketServidor = iniciarServidor();
	conexionSindicato = conectarseA(SINDICATO);
	nombreRestaurante = obtenerNombreRestaurante();

	// Obtener metadata del restaurante consultando a Sindicato
	enviarPaquete(conexionSindicato, RESTAURANTE, OBTENER_RESTAURANTE, nombreRestaurante);

	t_header *header = recibirHeaderPaquete(conexionSindicato);
	t_buffer *payload = recibirPayloadPaquete(header, conexionSindicato);

	t_posicion *pr = payload->stream;
	printf("Me llegó size: %d, posX %d, posY %d\n", payload->size, pr->posX, pr->posY);

	// Prueba de GUARDAR_PEDIDO (después va a ir en una función aparte que se desencadene por algún hilo de conexión)

	t_req_pedido *pedido = malloc(sizeof(t_req_pedido));
	pedido->restaurante = nombreRestaurante;
	pedido->idPedido = 777;

	enviarPaquete(conexionSindicato, RESTAURANTE, GUARDAR_PEDIDO, pedido);
	header = recibirHeaderPaquete(conexionSindicato);
	payload = recibirPayloadPaquete(header, conexionSindicato);
	char *resultGuardarPedido = payload->stream;
	log_info(logger, "%s", resultGuardarPedido);

	// Prueba de CONSULTAR_PLATOS (después va a ir en una función aparte que se desencadene por algún hilo de conexión)

	enviarPaquete(conexionSindicato, RESTAURANTE, CONSULTAR_PLATOS, nombreRestaurante);

	header = recibirHeaderPaquete(conexionSindicato);
	payload = recibirPayloadPaquete(header, conexionSindicato);
	t_list *platos = payload->stream;
	mostrarListaStrings(platos);

	// Prueba de GUARDAR_PLATO (después va a ir en una función aparte que se desencadene por algún hilo de conexión)

	t_req_plato *reqPlato = malloc(sizeof(t_req_plato));
	reqPlato->restaurante = nombreRestaurante;
	reqPlato->plato = "Empanadas";
	reqPlato->cantidadPlato = 3;
	reqPlato->idPedido = 777;

	enviarPaquete(conexionSindicato, RESTAURANTE, GUARDAR_PLATO, reqPlato);

	header = recibirHeaderPaquete(conexionSindicato);
	payload = recibirPayloadPaquete(header, conexionSindicato);
	char *resultGuardarPlato = payload->stream;
	log_info(logger, "%s", resultGuardarPlato);

	// Prueba de CONFIRMAR_PEDIDO (después va a ir en una función aparte que se desencadene por algún hilo de conexión)

	t_req_pedido *pedidoConf = malloc(sizeof(t_req_pedido));
	pedidoConf->restaurante = nombreRestaurante;
	pedidoConf->idPedido = 777;

	enviarPaquete(conexionSindicato, RESTAURANTE, CONFIRMAR_PEDIDO, pedido);
	header = recibirHeaderPaquete(conexionSindicato);
	payload = recibirPayloadPaquete(header, conexionSindicato);
	char *resultConfPedido = payload->stream;
	log_info(logger, "%s", resultConfPedido);

	// Prueba de OBTENER_PEDIDO (después va a ir en una función aparte que se desencadene por algún hilo de conexión)

	t_req_pedido *pedidoObt = malloc(sizeof(t_req_pedido));
	pedidoObt->restaurante = nombreRestaurante;
	pedidoObt->idPedido = 777;

	enviarPaquete(conexionSindicato, RESTAURANTE, OBTENER_PEDIDO, pedidoObt);
	header = recibirHeaderPaquete(conexionSindicato);
	payload = recibirPayloadPaquete(header, conexionSindicato);
	t_pedido *pedidoCompleto = payload->stream;
	log_info(logger, "Estado del pedido %d, Precio total: $%d", pedidoCompleto->estado, pedidoCompleto->precioTotal);

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

			log_info(logger, "Nuevo hilo para atender a Cliente con el socket %d", fd);
		}
	}
	
	liberarConexion(socketServidor);
    finalizarProceso();
    return EXIT_SUCCESS;
}