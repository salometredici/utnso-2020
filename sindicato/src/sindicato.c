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
			logConsoleInput(comandoLeido);

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
					printf("Comando no válido. Escriba 'AIUDA' para ver los formatos aceptados ◔_◔\n");
					break;
			}

			free(mensaje);
			free(parametros);
			free(comandoLeido);
			if (opcion == OPT_BAI) { break; }
		}
		comandoLeido = readline("(=^.^=)~>");
	}

    pthread_exit(EXIT_SUCCESS);
    return 0;
}

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
			case OBTENER_PROCESO:;
				enviarPaquete(socketCliente, SINDICATO, RTA_OBTENER_PROCESO, SINDICATO);
				break;
			case OBTENER_RESTAURANTE:;
				char *nombreRestaurante = recibirPayloadPaquete(header, socketCliente);
				logMetadataRequest(nombreRestaurante);

				t_md *md = malloc(sizeof(t_md));

				t_list *afinidades = list_create();
				list_add(afinidades, "Milanesas");
				md->afinidades = afinidades;

				t_list *platosMd = list_create();
				t_md_receta *r1 = malloc(sizeof(t_md_receta)); t_md_receta *r2 = malloc(sizeof(t_md_receta));
				r1->plato = "Milanesas"; r1->precio = 150;
				r2->plato = "Pizza"; r2->precio = 220;
				list_add(platosMd, r1); list_add(platosMd, r2);
				md->platos = platosMd;

				md->posX = 5; md->posY = 7;
				md->cantidadCocineros = 5; md->cantidadHornos = 2; md->cantidadPedidos = 1;

				enviarPaquete(socketCliente, SINDICATO, RTA_OBTENER_RESTAURANTE, md);
				free(r1); free(r2); free(platosMd);
				free(afinidades);
				free(md);
				break;
			case CONSULTAR_PLATOS:;
				char *restConsulta = recibirPayloadPaquete(header, socketCliente);
				logConsultaPlatos(restConsulta);
				free(restConsulta);

				// TODO:
				// 1. Verificar si R existe en FS, buscando en dir Restaurantes si existe un subdir con R - Si no existe informarlo
				// 2. Obtener los platos que puede preparar R del archivo info.AFIP
				// 3. Responder indicando los platos que puede preparar R
				
				t_list *platosRest = list_create();
				list_add(platosRest, "Ensalada");
				list_add(platosRest, "Lasagna");
				list_add(platosRest, "Helado");
				enviarPaquete(socketCliente, SINDICATO, RTA_CONSULTAR_PLATOS, platosRest);
				free(platosRest);
				break;
			case GUARDAR_PEDIDO:;
				t_request *reqGuardarPedido = recibirPayloadPaquete(header, socketCliente);
				logRequest(reqGuardarPedido, header->codigoOperacion);
				free(reqGuardarPedido);

				// TODO:
				// 1. Verificar si R existe en FS... etc.
				// 2. Verificar cuál fue el último pedido y crear un nuevo archivo Pedido y ContPedidos++, de ser el 1ero, crear el archivo Pedido1
				// 3. Responder el mensaje con Ok/fail
				
				t_result *resGP = malloc(sizeof(t_result));
				resGP->msg = "[GUARDAR_PEDIDO] OK";
				resGP->hasError = false;
				enviarPaquete(socketCliente, SINDICATO, RTA_GUARDAR_PEDIDO, resGP);
				free(resGP);		
				break;
			case GUARDAR_PLATO:;
				t_req_plato *reqGuardarPlato = recibirPayloadPaquete(header, socketCliente);
				logRequestPlato(reqGuardarPlato);
				free(reqGuardarPlato);

				// TODO:
				// 1. Verificar si R existe en FS... etc.
				// 2. Verificar si el Pedido existe en FS, buscando en dir de R si existe el Pedido - Si no existe informarlo
				// 3. Verificar que el Pedido esté en estado "Pendiente" - En caso contrario informar situación
				// 4. Verificar si Pl existe en el archivo. CantActual + CantEnviada - Si no existe agregar Pl a lista de Pls y anexar Cant + aumentar precio total del Pedido
				// 5. Responder el mensaje con Ok/fail
				
				t_result *resGPlato = malloc(sizeof(t_result));
				resGPlato->msg = "[GUARDAR_PLATO] OK";
				resGPlato->hasError = false;
				enviarPaquete(socketCliente, SINDICATO, RTA_GUARDAR_PLATO, resGPlato);
				free(resGPlato);
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
				t_request *reqObt = recibirPayloadPaquete(header, socketCliente);
				logRequest(reqObt, header->codigoOperacion);
				free(reqObt);

				// TODO:
				// 1. Verificar si R existe en FS... etc.
				// 2. Verificar si el Pedido existe en FS, buscando en dir de R si existe el Pedido - Si no existe informarlo
				// 3. Responder indicando si se pudo realizar junto con la información del pedido de ser así

				t_pedido *pedido = malloc(sizeof(t_pedido)); t_list *platos = list_create();
				t_plato *milanesa = malloc(sizeof(t_plato)); t_plato *empanadas = malloc(sizeof(t_plato)); t_plato *ensalada = malloc(sizeof(t_plato));

				milanesa->plato = "Milanesa"; milanesa->precio = 200; milanesa->cantidadPedida = 2; milanesa->cantidadLista = 1;
				empanadas->plato = "Empanadas"; empanadas->precio = 880; empanadas->cantidadPedida = 12; empanadas->cantidadLista = 6;
				ensalada->plato = "Ensalada"; ensalada->precio = 120; ensalada->cantidadPedida = 1; ensalada->cantidadLista = 0;
				list_add(platos, milanesa); list_add(platos, empanadas); list_add(platos, ensalada);

				pedido->restaurante = string_new(); pedido->estado = PENDIENTE;
				pedido->platos = platos; pedido->precioTotal = calcularPrecioTotal(platos);

				enviarPaquete(socketCliente, SINDICATO, RTA_OBTENER_PEDIDO, pedido);
				free(pedido); free(milanesa); free(empanadas); free(ensalada);
				break;
			case PLATO_LISTO: // TODO
				break;
			case TERMINAR_PEDIDO: // TODO: Recibe idPedido y restaurante, retorna Ok/fail
				break;
			case OBTENER_RECETA: // TODO: Recibe plato y retorna receta
				// t_list *instrucciones = list_create();

				// char* ej[] = { "PREPARAR", "SERVIR" };
				// for(int i = 0; i < 4; i++){
				// 	t_instrucciones_receta *instruccion = malloc(sizeof(t_instrucciones_receta));
				// 	instruccion->paso = ej[i];
				// 	instruccion->qPaso = 2;
				// 	list_add(instrucciones, instruccion);
				// }
				// enviarPaquete(socketCliente, SINDICATO, RTA_OBTENER_RECETA, instrucciones);
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

int main(int argc, char **argv)
{
    inicializarProceso(SINDICATO);
	socketServidor = iniciarServidor();

	init();

	// Inicio del hilo de la consola y su lectura
	pthread_create(&threadConsola, NULL, (void *) threadLecturaConsola, NULL);
    pthread_detach(threadConsola);

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

    finalizarProceso();
    return EXIT_SUCCESS;
}