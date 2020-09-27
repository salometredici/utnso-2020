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
	enviarPaquete(conexionSindicato, RESTAURANTE, OBTENER_RESTAURANTE, nombreRestaurante); // Ver si se puede simplificar o generalizar esto

	t_header *header = recibirHeaderPaquete(conexionSindicato);
	log_info(logger, "Me llegó procesoOrigen: %s, mensaje: %s\n",
				getStringKeyValue(header->procesoOrigen, PROCNKEYS),
				getStringKeyValue(header->codigoOperacion, COMMANDNKEYS));

	t_buffer *buffer = recibirPayloadPaquete(header, conexionSindicato);

	t_posicion *pr = buffer->stream;
	printf("Me llegó size: %d, posX %d, posY %d\n", buffer->size, pr->posX, pr->posY);
  
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