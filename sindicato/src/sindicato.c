#include "../include/sindicato.h"

void mostrarComandosValidos() {
    printf("-------------------Comandos Válidos-------------------\n");
	printf("Ejemplo: AIUDA\n");
	printf("Ejemplo: CLEAR\n");
	printf("Ejemplo: BAI\n");
    printf("Formato: [MENSAJE] [PARAMETROS]\n");
	printf("Ejemplo: CREAR_RESTAURANTE [NOMBRE] [CANTIDAD_COCINEROS] [POSICION] [AFINIDAD_COCINEROS] [PLATOS] [PRECIO_PLATOS] [CANTIDAD_HORNOS]\n");
	printf("Ejemplo: CREAR_RECETA [NOMBRE] [PASOS] [TIEMPO_PASOS] \n");
    printf("------------------------------------------------------\n");
}

void* threadLecturaConsola(void * args) {
    printf("Iniciando la consola ...\n");
	printf("Para ver los comandos válidos, ingrese 'AIUDA'.\n");

	char *comandoOriginal;
	char **parametros;
	char *mensaje;
	int opcion;
    char *comandoLeido = readline("(=^.^=)~>");

    while (1) {
        add_history(comandoLeido);
        comandoOriginal = malloc(sizeof(char) * strlen(comandoLeido) + 1);
        strcpy(comandoOriginal, comandoLeido);
        string_to_upper(comandoLeido);
        string_trim(&comandoLeido);

		// Parámetros
        parametros = string_split(comandoLeido, " ");
		mensaje = parametros[0];
        free(comandoOriginal);
		log_debug(logger, "Comando ingresado: %s", comandoLeido); // Por ahora, para ver lo que toma

        opcion = sindicatoOptionToKey(mensaje);
		switch (opcion) {
			case OPT_CREAR_RESTAURANTE:
				printf("Crear Restaurante: Se deberá crear una nueva carpeta restaurante, con su respectivo info.AFIP explicado anteriormente\n");
				break;
			case OPT_CREAR_RECETA:
				printf("Crear Receta: Se deberá crear un nuevo archivo de receta siguiendo los lineamientos de lo detallado anteriormente.\n");
				break;
			case OPT_AIUDA:
				mostrarComandosValidos();
				break;
			case OPT_BAI:
				printf("adiosss (๑♡3♡๑)!\n");
				break;
			case OPT_CLEAR:
				limpiarPantalla();
				break;
			case ERROR:
			default:
				printf("Comando no válido. Escriba 'AIUDA' para ver el formato aceptado.\n");
				break;
		}

		free(mensaje);
        free(parametros);
        free(comandoLeido);
        if (opcion == OPT_BAI) { break; }

		comandoLeido = readline("(=^.^=)~>");
	}

    pthread_exit(EXIT_SUCCESS);
    return 0;
}

void iterator(char* value)	{
	printf("%s\n", value);
}

void *atenderConexiones(void *conexionNueva) {
    pthread_data *t_data = (pthread_data*) conexionNueva;
    int info = t_data->socketThread;
    free(t_data);

	t_list *lista; // Revisar un poco más cómo utilizar las listas en los paquetes
	while (1) {

		t_buffer *payload;
		t_header *header = recibirHeaderPaquete(info);
		log_info(logger, "Me llegó procesoOrigen: %s, mensaje: %s\n",
					getStringKeyValue(header->procesoOrigen, PROCNKEYS),
					getStringKeyValue(header->codigoOperacion, COMMANDNKEYS));

		if (header->procesoOrigen == ERROR) { //TODO: Manejar desconexiones de sockets
			printf("El cliente %d se desconectó. Finalizando el hilo...╮(╯_╰)╭\n", info);
			liberarConexion(info);
    		pthread_exit(EXIT_SUCCESS);
			return EXIT_FAILURE;
		}		

		switch (header->codigoOperacion) {
			case OBTENER_RESTAURANTE:
				printf("OBTENER RESTAURANTE\n");
				// Recibe como parámetro el nombre del restaurante
				payload = recibirPayloadPaquete(header, info);
				printf("Me llegó un payload de tamaño %d, Nombre del restaurante: %s\n", payload->size , payload->stream);

				t_posicion *posicionRestaurante = malloc(sizeof(t_posicion));
				posicionRestaurante->posX = 25; posicionRestaurante->posY = 45; // Ejemplo de envío de una rta con un struct t_posicion
				enviarPaquete(info, SINDICATO, RTA_OBTENER_RESTAURANTE, posicionRestaurante);

				break;
			case CONSULTAR_PLATOS:
				printf("CONSULTAR PLATOS\n");
				// Recibe como parámetro el nombre del restaurante
				payload = recibirPayloadPaquete(header, info);
				printf("Me llegó un payload de tamaño %d, Nombre del restaurante: %s\n", payload->size , payload->stream);

				
				break;
			case GUARDAR_PEDIDO:
				printf("GUARDAR PEDIDO\n");				
				// Recibe como parámetro un t_req_pedido (a revisar si actualizan las definiciones)
				payload = recibirPayloadPaquete(header, info);
				printf("Me llegó un payload de tamaño %d\n", payload->size);
				t_req_pedido *p = malloc(sizeof(t_req_pedido));
				p = payload->stream;
				printf("datos del pedido a guardar:\n");
				printf("id del pedido: %d, nombre del restaurante: %s", p->idPedido, p->restaurante);
				
				
				// char *we;
				// sprintf(we, "Pedido %d guardado con éxito", payload->idPedido);
				// enviarPaquete(info, SINDICATO, RTA_GUARDAR_PEDIDO, res);
				// int sizeOfResult = strlen("Pedido %d guardado con éxito", payload->idPedido + 1);
				// t_result *res = malloc(sizeof(bool) + sizeOfResult);

				break;
			case GUARDAR_PLATO:
				break;
			case CONFIRMAR_PEDIDO:
				break;
			case OBTENER_PEDIDO:
				break;
			case PLATO_LISTO:
				break;
			case TERMINAR_PEDIDO:
				break;
			case OBTENER_RECETA:
				break;
			case ERROR:
				log_error(logger, "El cliente %d se desconectó. Finalizando el hilo...╮(╯_╰)╭\n", info);
				return EXIT_FAILURE;
			default:
				printf("Operación desconocida. Llegó el código: %d. No quieras meter la pata!!!(｀Д´*)\n", header->codigoOperacion);
				break;
		}
	}

    pthread_exit(EXIT_SUCCESS);
    return 0;
}

int main(int argc, char ** argv){
    inicializarProceso(SINDICATO);

	// Inicio del hilo de la consola y su lectura
	pthread_create(&threadConsola, NULL, (void *) threadLecturaConsola, NULL);
    pthread_detach(threadConsola);

	socketServidor = iniciarServidor();
	int fd;
	while (1) {
		//TODO: Lógica del Cliente
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

    finalizarProceso();
    return EXIT_SUCCESS;
}