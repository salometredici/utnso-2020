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

		if (header->procesoOrigen == ERROR) { //TODO: Manejar desconexiones de sockets
			printf("El cliente %d se desconectó. Finalizando el hilo...╮(╯_╰)╭\n", info);
			liberarConexion(info);
    		pthread_exit(EXIT_SUCCESS);
			return EXIT_FAILURE;
		}		

		switch (header->codigoOperacion) {
			case OBTENER_RESTAURANTE: // Params: Nombre del restaurante
				payload = recibirPayloadPaquete(header, info);
				char *nombreRestaurante = payload->stream;

				t_posicion *posicionRestaurante = malloc(sizeof(t_posicion));
				posicionRestaurante->posX = 25; posicionRestaurante->posY = 45; // Ejemplo de envío de una rta con un struct t_posicion

				enviarPaquete(info, SINDICATO, RTA_OBTENER_RESTAURANTE, posicionRestaurante);
				break;
			case CONSULTAR_PLATOS: // Params: Nombre del restaurante
				payload = recibirPayloadPaquete(header, info);
				char *restPlatos = payload->stream;

				// TODO:
				// 1. Verificar si R existe en FS, buscando en dir Restaurantes si existe un subdir con R - Si no existe informarlo
				// 2. Obtener los platos que puede preparar R del archivo info.AFIP
				// 3. Responder indicando los platos que puede preparar R
				printf("Restaurante: %s\n", restPlatos);
				t_list *platos = list_create(); // Va a retornar una lista de todos los platos que puede preparar el restaurante, como enum o como string?
				list_add(platos, "Milanesas");
				list_add(platos, "Lasagna");
				list_add(platos, "Asado");

				enviarPaquete(info, SINDICATO, RTA_CONSULTAR_PLATOS, platos);				
				break;
			case GUARDAR_PEDIDO: // Params: Nombre del restaurante + Id del Pedido (¿para qué?) - Nota: Si no le dan utilidad al IdPedido no usar t_req_pedido
				payload = recibirPayloadPaquete(header, info);
				t_req_pedido *reqPedido = payload->stream;

				// TODO:
				// 1. Verificar si R existe en FS... etc.
				// 2. Verificar cuál fue el último pedido y crear un nuevo archivo Pedido y ContPedidos++, de ser el 1ero, crear el archivo Pedido1
				// 3. Responder el mensaje con Ok/Fail
				printf("Datos del pedido a guardar:\n");
				log_info(logger, "Id pedido: %d, Restaurante: %s", reqPedido->idPedido, reqPedido->restaurante);
				char *msjGuardarPedido = "[GUARDAR_PEDIDO] Ok";

				enviarPaquete(info, SINDICATO, RTA_GUARDAR_PEDIDO, msjGuardarPedido);
				break;
			case GUARDAR_PLATO:
				payload = recibirPayloadPaquete(header, info);
				t_req_plato *reqPlato = payload->stream;

				// TODO:
				// 1. Verificar si R existe en FS... etc.
				// 2. Verificar si el Pedido existe en FS, buscando en dir de R si existe el Pedido - Si no existe informarlo
				// 3. Verificar que el Pedido esté en estado "Pendiente" - En caso contrario informar situación
				// 4. Verificar si Pl existe en el archivo. CantActual + CantEnviada - Si no existe agregar Pl a lista de Pls y anexar Cant + aumentar precio total del Pedido
				// 5. Responder el mensaje con Ok/Fail
				printf("Datos del plato a guardar:\n");
				log_info(logger, "Restaurante: %s, IdPedido: %d", reqPlato->restaurante, reqPlato->idPedido);
				log_info(logger, "Plato: %s, CantPlato: %d", reqPlato->plato, reqPlato->cantidadPlato);
				char *msjGuardarPlato = "[GUARDAR_PLATO] Ok";

				enviarPaquete(info, SINDICATO, RTA_GUARDAR_PLATO, msjGuardarPlato);
				break;
			case CONFIRMAR_PEDIDO: // Params: Nombre del restaurante + idPedido
				payload = recibirPayloadPaquete(header, info);
				t_req_pedido *reqConfPedido = payload->stream;

				// TODO:
				// 1. Verificar si R existe en FS... etc.
				// 2. Verificar si el Pedido existe en FS, buscando en dir de R si existe el Pedido - Si no existe informarlo
				// 3. Verificar que el Pedido esté en estado "Pendiente" - En caso contrario informar situación
				// 4. Cambiar el estado del Pedido de "Pendiente" a "Confirmado" - Truncar el archivo de ser necesario
				// 5. Responder el mensaje con Ok/Fail
				printf("Datos del pedido a confirmar:\n");
				log_info(logger, "Id pedido: %d, Restaurante: %s", reqConfPedido->idPedido, reqConfPedido->restaurante);
				char *msjConfPedido = "[CONFIRMAR_PEDIDO] Ok";

				enviarPaquete(info, SINDICATO, RTA_CONFIRMAR_PEDIDO, msjConfPedido);
				break;
			case OBTENER_PEDIDO:
				payload = recibirPayloadPaquete(header, info);
				t_req_pedido *reqObtenerPedido = payload->stream;

				// TODO:
				// 1. Verificar si R existe en FS... etc.
				// 2. Verificar si el Pedido existe en FS, buscando en dir de R si existe el Pedido - Si no existe informarlo
				// 3. Responder indicando si se pudo realizar junto con la información del pedido de ser así
				printf("Pedido a obtener:\n");
				log_info(logger, "Id pedido: %d, Restaurante: %s", reqConfPedido->idPedido, reqConfPedido->restaurante);
				


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