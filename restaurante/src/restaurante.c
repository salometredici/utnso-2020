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
			case CONSULTAR_PLATOS:;
				char *restPlatos = recibirPayloadPaquete(header, socket);

				printf("Restaurante: %s\n", restPlatos);
				t_list *platosRest = list_create(); // Va a retornar una lista de todos los platos que puede preparar el restaurante, como enum o como string?
				list_add(platosRest, "Milanesas");
				list_add(platosRest, "Lasagna");
				list_add(platosRest, "Asado");

				enviarPaquete(socket, RESTAURANTE, RTA_CONSULTAR_PLATOS, platosRest);
				break;
			case CREAR_PEDIDO:
				enviarPaquete(socket, RESTAURANTE, RTA_CREAR_PEDIDO, 77);
				break;
			case ANIADIR_PLATO:
				//TODO;
				break;
			case CONFIRMAR_PEDIDO:;
				t_req_pedido *reqConfPedido = recibirPayloadPaquete(header, socket);

				// TODO:
				// 1. Verificar si R existe en FS... etc.
				// 2. Verificar si el Pedido existe en FS, buscando en dir de R si existe el Pedido - Si no existe informarlo
				// 3. Verificar que el Pedido esté en estado "Pendiente" - En caso contrario informar situación
				// 4. Cambiar el estado del Pedido de "Pendiente" a "Confirmado" - Truncar el archivo de ser necesario
				// 5. Responder el mensaje con Ok/Fail
				logRequestPedido(reqConfPedido);
				char *msjConfPedido = "[CONFIRMAR_PEDIDO] Ok";

				enviarPaquete(socket, SINDICATO, RTA_CONFIRMAR_PEDIDO, msjConfPedido);
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
	t_posicion *pr = recibirPayloadPaquete(header, conexionSindicato);
	logMetadata(pr);

	// Prueba de GUARDAR_PEDIDO (después va a ir en una función aparte que se desencadene por algún hilo de conexión)

	t_req_pedido *pedido = malloc(sizeof(t_req_pedido));
	pedido->restaurante = nombreRestaurante;
	pedido->idPedido = 777;

	enviarPaquete(conexionSindicato, RESTAURANTE, GUARDAR_PEDIDO, pedido);
	header = recibirHeaderPaquete(conexionSindicato);
	char *resultGuardarPedido  = recibirPayloadPaquete(header, conexionSindicato);
	log_info(logger, "%s", resultGuardarPedido);

	// Prueba de CONSULTAR_PLATOS (después va a ir en una función aparte que se desencadene por algún hilo de conexión)

	enviarPaquete(conexionSindicato, RESTAURANTE, CONSULTAR_PLATOS, nombreRestaurante);

	header = recibirHeaderPaquete(conexionSindicato);
	t_list *platos = recibirPayloadPaquete(header, conexionSindicato);
	mostrarListaStrings(platos);

	// Prueba de GUARDAR_PLATO (después va a ir en una función aparte que se desencadene por algún hilo de conexión)

	t_req_plato *reqPlato = malloc(sizeof(t_req_plato));
	reqPlato->restaurante = nombreRestaurante;
	reqPlato->plato = "Empanadas";
	reqPlato->cantidadPlato = 3;
	reqPlato->idPedido = 777;

	enviarPaquete(conexionSindicato, RESTAURANTE, GUARDAR_PLATO, reqPlato);
	header = recibirHeaderPaquete(conexionSindicato);
	char *resultGuardarPlato = recibirPayloadPaquete(header, conexionSindicato);
	log_info(logger, "%s", resultGuardarPlato);

	// Prueba de CONFIRMAR_PEDIDO (después va a ir en una función aparte que se desencadene por algún hilo de conexión)

	t_req_pedido *pedidoConf = malloc(sizeof(t_req_pedido));
	pedidoConf->restaurante = nombreRestaurante;
	pedidoConf->idPedido = 777;

	enviarPaquete(conexionSindicato, RESTAURANTE, CONFIRMAR_PEDIDO, pedidoConf);
	header = recibirHeaderPaquete(conexionSindicato);
	char *resultConfPedido = recibirPayloadPaquete(header, conexionSindicato);
	log_info(logger, "%s", resultConfPedido);

	// Prueba de OBTENER_PEDIDO (después va a ir en una función aparte que se desencadene por algún hilo de conexión)

	t_req_pedido *pedidoObt = malloc(sizeof(t_req_pedido));
	pedidoObt->restaurante = nombreRestaurante;
	pedidoObt->idPedido = 777;

	enviarPaquete(conexionSindicato, RESTAURANTE, OBTENER_PEDIDO, pedidoObt);
	header = recibirHeaderPaquete(conexionSindicato);
	t_pedido *pedidoCompleto = recibirPayloadPaquete(header, conexionSindicato);
	mostrarListaPlatos(pedidoCompleto->platos);
	free(pedidoCompleto); // Dejamos este free?

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
		}
	}
	
	liberarConexion(socketServidor);
    finalizarProceso();
    return EXIT_SUCCESS;
}