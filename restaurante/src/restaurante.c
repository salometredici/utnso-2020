#include "../include/restaurante.h"

void iterator(char *value)	{
	printf("%s\n", value);
}

void *atenderConexiones(void *conexionNueva)
{
    pthread_data *t_data = (pthread_data*) conexionNueva;
    int info = t_data->socketThread;
    free(t_data);
	
	t_list *lista;
	while(1) {

		t_buffer *payload; // Ver cómo liberar este payload después
		t_header *header = recibirHeaderPaquete(info);

		if (header->procesoOrigen == ERROR) { // Ver cómo manejar esta desconexión
			log_error("El cliente %d se desconectó. Finalizando hilo...╮(╯_╰)╭\n", info);
			liberarConexion(info);
    		pthread_exit(EXIT_SUCCESS);
			return EXIT_FAILURE;
		}

		switch (header->codigoOperacion) { // Mensajes y/o rtas que puede manejar Restaurante
			case CONSULTAR_PLATOS:
				//TODO
				break;
			case CREAR_PEDIDO:
				//TODO
				break;
			case ANIADIR_PLATO:
				//TODO
				break;
			case CONFIRMAR_PEDIDO:
				//TODO
				break;
			case CONSULTAR_PEDIDO:
				//TODO
				break;
			case RTA_OBTENER_RESTAURANTE:
				payload = recibirPayloadPaquete(header, info);
				printf("Me llegó un payload de tamaño %d, Nombre del restaurante: %s\n", payload->size , payload->stream);
				break;
			case ERROR:
				log_error(logger, "El cliente %d se desconectó. Finalizando el hilo...╮(╯_╰)╭\n", info);
				return EXIT_FAILURE;
			default:
				printf("Operación desconocida. Valor recibido: %d. No quieras meter la pata!\n", header->codigoOperacion);
				break;
		}
	}

    pthread_exit(EXIT_SUCCESS);
    return 0;
}

int main(int argc, char* argv[])
{
	inicializarProceso(RESTAURANTE);
	socketServidor = iniciarServidor();
	conexionSindicato = conectarseA(SINDICATO);
	nombreRestaurante = obtenerNombreRestaurante();

	// Obtener metadata del restaurante consultando a Sindicato
	enviarPaquete(conexionSindicato, RESTAURANTE, OBTENER_RESTAURANTE, strlen(nombreRestaurante) + 1, nombreRestaurante); // Ver si se puede simplificar o generalizar esto

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