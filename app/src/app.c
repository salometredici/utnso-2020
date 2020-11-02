#include "../include/app.h"

void *planificar(void *args) {
	while (1) {
		switch (algoritmoSeleccionado) {
			case FIFO:
				// Largo plazo
				actualizarQRconQN();
				actualizarQRconQB();
				actualizarQEconQR_FIFO();
				// Corto plazo
				ejecutarCiclosFIFO();
				break;
			case HRRN:
				// Largo plazo
				actualizarQRconQN();
				actualizarQRconQB();
				actualizarQEconQR_HRRN();
				// Corto plazo
				ejecutarCiclosHRRN();
				break;
			case SJF:
				break;
			default:
				break;
		}
	}
}

// Preguntar al cliente su nombre y posición, asociandolo a un socket
t_cliente *getCliente(int socketCliente) {
	t_header *header = recibirHeaderPaquete(socketCliente);
	t_cliente *cliente = recibirPayloadPaquete(header, socketCliente);
	cliente->socketCliente = socketCliente;
	free(header);
}

// Limpiar clientes desconectados
void revisarConectados(t_list *lista) {
	for (int i = 0; i < list_size(lista); i++) {
		t_cliente *clienteActual = list_get(lista, i);
		if (recv(clienteActual->socketCliente, NULL, 1, MSG_PEEK | MSG_DONTWAIT) == 0) {
			list_remove(lista, i);
			log_debug(logger, "El cliente [#%d - %s] se desconectó", i, clienteActual->idCliente);
		}
	}
}

// Revisar si el cliente ya existe en la lista de conectados y limpiar los desconectados
void actualizarClientesConectados(t_cliente *cliente) {
	revisarConectados(clientesConectados);
	revisarConectados(restaurantesConectados);

	bool estaDuplicado(void *actual) {
		t_cliente *clienteActual = actual;
		return string_equals_ignore_case(cliente->idCliente, clienteActual->idCliente);
	};

	if (cliente->esRestaurante) {
		t_cliente *restDuplicado = list_find(restaurantesConectados, &estaDuplicado);
		if (restDuplicado != NULL) { list_add(restaurantesConectados, cliente); }
	} else {
		t_cliente *clienteDuplicado = list_find(clientesConectados, &estaDuplicado);
		if (clienteDuplicado != NULL) {	list_add(clientesConectados, cliente); }
	}
}

t_cliente *getRestConectado(char *restBuscado) {
	bool restFound(void *actual) {
		t_cliente *restActual = actual;
		return string_equals_ignore_case(restBuscado, restActual->idCliente);
	};

	return list_find(restaurantesConectados, &restFound);
}

int generarId(int min, int max) {
	srand(time(NULL));
	
	int nuevoId = (rand() % (max - min + 1)) + min;
	
	bool idExistente(void *actual) {
		int idActual = actual;
		return idActual == nuevoId;
	}
	
	if (list_find(idsGenerados, &idExistente) == NULL) {
		list_add(idsGenerados, nuevoId);
		return nuevoId;
	} else {
		return generarId(min, max);
	}
};

t_list *obtenerRestsConectados() {
	t_list *nombresRests = list_create();
	for (int i = 0; i < list_size(restaurantesConectados); i++) {
		t_cliente *currentRest = list_get(restaurantesConectados, i);
		list_add(nombresRests, currentRest->idCliente);
	}
	return nombresRests;
}

void *atenderConexiones(void *conexionNueva)
{
    pthread_data *t_data = (pthread_data*) conexionNueva;
    int socketCliente = t_data->socketThread;
    free(t_data);

	int socketComanda = ERROR;
	t_cliente *cliente = getCliente(socketCliente);
	actualizarClientesConectados(cliente);

	while (1) {
    	t_header *header = recibirHeaderPaquete(socketCliente);

		if (header->procesoOrigen == ERROR || header->codigoOperacion == ERROR) {
			logClientDisconnection(socketCliente); // Mejorar
			liberarConexion(socket);
    		pthread_exit(EXIT_SUCCESS);
			return EXIT_FAILURE;
		}

    	switch (header->codigoOperacion) {
			case OBTENER_PROCESO:;
				enviarPaquete(socketCliente, APP, RTA_OBTENER_PROCESO, APP);
				break;
        	case CONSULTAR_RESTAURANTES:; // Finished
				if (list_is_empty(restaurantesConectados)) {
					t_list *rest = list_create(); list_add(rest, restauranteDefault);
					enviarPaquete(socketCliente, APP, RTA_CONSULTAR_RESTAURANTES, rest);
					free(rest);
				} else {
            		enviarPaquete(socketCliente, APP, RTA_CONSULTAR_RESTAURANTES, obtenerRestsConectados());
				}
        		break;
			case SELECCIONAR_RESTAURANTE:; // Finished
				t_selecc_rest *seleccRest = recibirPayloadPaquete(header, socketCliente);
				cliente->restauranteSeleccionado = seleccRest->restauranteSeleccionado;
				logSeleccionarRestaurante(seleccRest);
				t_result *resSelecc = malloc(sizeof(t_result));
				resSelecc->msg = "[SELECCIONAR_RESTAURANTE] Ok\n";
				resSelecc->hasError = false;
				enviarPaquete(socketCliente, APP, RTA_SELECCIONAR_RESTAURANTE, resSelecc);
				free(resSelecc);
				break;
			case CONSULTAR_PLATOS:; // Finished with annotations
				char *consulta = recibirPayloadPaquete(header, socketCliente); free(consulta);

				if (list_is_empty(restaurantesConectados)) {
					log_info(logger, "No hay restaurantes conectados, se enviará el RestauranteDefault...");
					enviarPaquete(socketCliente, APP, RTA_CONSULTAR_PLATOS, platosResDefault);		
					logRtaConsultarPlatos(platosResDefault);			
				} else {
					t_cliente *restConectado = getRestConectado(cliente->restauranteSeleccionado); // ¿Qué pasa si no está?
					enviarPaquete(restConectado->socketCliente, APP, CONSULTAR_PLATOS, string_new());
					log_info(logger, "Consultando los platos del restaurante %s...", restConectado->idCliente);
					t_header *headerRest = recibirHeaderPaquete(restConectado->socketCliente);
					t_list *platosRest = recibirPayloadPaquete(headerRest, restConectado->socketCliente);
					enviarPaquete(socketCliente, APP, RTA_CONSULTAR_PLATOS, platosRest);
					logRtaConsultarPlatos(platosRest);
					free(platosRest);
					free(headerRest);
				}
				break;	
			case CREAR_PEDIDO:; // Finished
				conexionComanda = conectarseA(COMANDA);
				t_request *restAGuardar = malloc(sizeof(t_request));

				if (list_is_empty(restaurantesConectados)) {
					int unRandom = generarId(0, 100);
					enviarPaquete(socketCliente, APP, RTA_CREAR_PEDIDO, unRandom);

					restAGuardar->idPedido = unRandom;
					restAGuardar->nombre = restauranteDefault;
				} else {
					t_cliente *restConsulta = getRestConectado(cliente->restauranteSeleccionado);
					enviarPaquete(restConsulta->socketCliente, APP, CREAR_PEDIDO, NULL);
					printf("Solicitando un nuevo idPedido para el cliente %d a RESTAURANTE...\n", restConsulta->idCliente);
					log_info(logger, "Solicitando un nuevo idPedido para el cliente %d a RESTAURANTE...", restConsulta->idCliente);
					t_header *headerRest = recibirHeaderPaquete(restConsulta->socketCliente);
					int newIdPedido = recibirPayloadPaquete(headerRest, restConsulta->socketCliente);
					printf("El nuevo idPedido es %d\n", newIdPedido);
					log_info(logger, "El nuevo idPedido es %d", newIdPedido);
					free(headerRest);

					restAGuardar->idPedido = newIdPedido;
					restAGuardar->nombre = restConsulta->idCliente;
				}

				enviarPaquete(conexionComanda, APP, GUARDAR_PEDIDO, restAGuardar);
				t_header *headerPedidoGuardado = recibirHeaderPaquete(conexionComanda);
				t_result *payloadPedidoGuardado = recibirPayloadPaquete(headerPedidoGuardado, conexionComanda);
				logTResult(payloadPedidoGuardado);
				enviarPaquete(socketCliente, APP, RTA_CREAR_PEDIDO, payloadPedidoGuardado->hasError ? ERROR : restAGuardar->idPedido);

				free(restAGuardar);
				free(headerPedidoGuardado);
				free(payloadPedidoGuardado);
				liberarConexion(conexionComanda);
				break;	
			case ANIADIR_PLATO:; // Finished with annotations
				conexionComanda = conectarseA(COMANDA);
				t_request *reqAniadir = recibirPayloadPaquete(header, socketCliente);
				logRequest(reqAniadir, header->codigoOperacion);

				// ¿No deberíamos obtener el RESTAURANTE dueño del pedido en lugar del RESTAURANTE seleccionado?
				t_cliente *restConectado = getRestConectado(cliente->restauranteSeleccionado);

				t_header *headerCom = malloc(sizeof(t_header));
				t_result *resAniadir = malloc(sizeof(t_result));

				// Si es restauranteDefault, enviamos t_request a COMANDA
				if (string_equals_ignore_case(restConectado->idCliente, restauranteDefault)) {
					enviarPaquete(conexionComanda, APP, ANIADIR_PLATO, reqAniadir);
					headerCom = recibirHeaderPaquete(conexionComanda);
					resAniadir = recibirPayloadPaquete(headerCom, conexionComanda);
					logTResult(resAniadir);
					enviarPaquete(socketCliente, APP, RTA_ANIADIR_PLATO, resAniadir);
				} else { // Sino, enviamos t_request al RESTAURANTE dueño del pedido
					enviarPaquete(restConectado->socketCliente, APP, ANIADIR_PLATO, reqAniadir);
					headerCom = recibirHeaderPaquete(restConectado->socketCliente);
					resAniadir = recibirPayloadPaquete(headerCom, restConectado->socketCliente);
					logTResult(resAniadir);
					// Si el RESTAURANTE dice que falló, informamos directamente al CLIENTE
					if (resAniadir->hasError) {
						enviarPaquete(socketCliente, APP, RTA_ANIADIR_PLATO, resAniadir);
					} else { // Sino, enviamos el t_request a COMANDA e informamos el resultado al CLIENTE
						enviarPaquete(conexionComanda, APP, ANIADIR_PLATO, reqAniadir);
						headerCom = recibirHeaderPaquete(conexionComanda);
						resAniadir = recibirPayloadPaquete(headerCom, conexionComanda);
						logTResult(resAniadir);
						enviarPaquete(socketCliente, APP, RTA_ANIADIR_PLATO, resAniadir);
					}
				}

				free(reqAniadir);
				free(restConectado);
				free(headerCom);
				free(resAniadir);
				liberarConexion(conexionComanda);
				break;	
			case PLATO_LISTO:; // In progress
				// REVISAR CON VALIDACION DE ESTADO PEDIDO
				conexionComanda = conectarseA(COMANDA);

				t_plato_listo *platoListo = recibirPayloadPaquete(header, socketCliente);
				enviarPaquete(conexionComanda, APP, PLATO_LISTO, platoListo);
				t_header *headerPL = recibirHeaderPaquete(conexionComanda);
				t_result *resPL = recibirPayloadPaquete(headerPL, conexionComanda);
				logTResult(resPL);

				// Verifico la respuesta de COMANDA
				if (resPL->hasError == false) { // Obtener pedido de COMANDA para verificar cantidad de paltos listos
					t_request *reqPedidoAVerificar = malloc(sizeof(t_request));
					reqPedidoAVerificar->nombre = platoListo->restaurante;
					reqPedidoAVerificar->idPedido = platoListo->idPedido;
					enviarPaquete(conexionComanda, APP, OBTENER_PEDIDO, reqPedidoAVerificar);
					t_header *headerPO = recibirHeaderPaquete(conexionComanda);
					t_pedido *pedidoAVerificar = recibirPayloadPaquete(headerPO, conexionComanda);
					
					// Verificar si todos los platos están listos
					t_list *platosPedido = list_create();
					list_add_all(platosPedido, pedidoAVerificar->platos);
					int cantidadPlatos = list_size(platosPedido);
					bool pedidoListo = true;
					for (int i=0; i<cantidadPlatos; i++) {
						t_plato *platoActual = list_get(platosPedido, i);
						if (platoActual->cantidadLista < platoActual->cantidadPedida) { pedidoListo = false; }
					}

					if (pedidoListo) {
						// Avisar al repartidor que ya puede buscar el pedido
						// ToDo
					}
				}
				
				enviarPaquete(socketCliente, APP, RTA_PLATO_LISTO, resPL);

				free(platoListo);
				free(headerPL);
				free(resPL);
				liberarConexion(conexionComanda);
				break;	
			case CONFIRMAR_PEDIDO:;
				t_request *reqConf = recibirPayloadPaquete(header, socketCliente);
				logRequest(reqConf, header->codigoOperacion);

				// Obtener el pedido de COMANDA
				enviarPaquete(conexionComanda, APP, OBTENER_PEDIDO, reqConf);
				t_header *headerCP = recibirHeaderPaquete(conexionComanda);
				t_pedido *pedidoConf = recibirPayloadPaquete(header, conexionComanda);
				logObtenerPedido(pedidoConf);

				// if pedidoConf != FINALIZADO

				t_pcb *pcb = malloc(sizeof(t_pcb));
				bool hasError = false;

				// Si es RDefault, generar PCB y añadirlo para planificar, sino, consultar al restaurante
				if (string_equals_ignore_case(cliente->restauranteSeleccionado, restauranteDefault)) {
					pcb = crearPcb(cliente, reqConf->idPedido);
					agregarAQN(pcb);
				} else {
					t_cliente *restConectado = getRestConectado(reqConf->nombre);
					enviarPaquete(restConectado->socketCliente, APP, CONFIRMAR_PEDIDO, reqConf);
					t_header *hRest = recibirHeaderPaquete(restConectado->socketCliente);
					t_result *rCP = recibirPayloadPaquete(hRest, restConectado->socketCliente);
					if (rCP->hasError) {
						hasError = true;
						enviarPaquete(socketCliente, APP, RTA_CONFIRMAR_PEDIDO, rCP);
					} else {
						pcb = crearPcb(cliente, reqConf->idPedido);
						agregarAQN(pcb);
					}
				}

				if (!hasError) {
					enviarPaquete(conexionComanda, APP, CONFIRMAR_PEDIDO, reqConf);
					t_header *hCom = recibirHeaderPaquete(conexionComanda);
					t_result *rCPC = recibirPayloadPaquete(hCom, conexionComanda);
					logTResult(rCPC);
					enviarPaquete(socketCliente, APP, RTA_CONFIRMAR_PEDIDO, rCPC);
				}

				break;
			case CONSULTAR_PEDIDO:; /* !!! Comanda no devuelve el precio, no? */
				int idPedidoCons = recibirPayloadPaquete(header, socketCliente);
				// Se consulta a COMANDA para obtener el pedido
				enviarPaquete(conexionComanda, APP, CONSULTAR_PEDIDO, idPedidoCons);
				t_header *headerCons = recibirHeaderPaquete(conexionComanda);
				t_pedido *pedidoCons = recibirPayloadPaquete(headerCons, conexionComanda);
				enviarPaquete(socketCliente, APP, RTA_CONSULTAR_PEDIDO, pedidoCons);
				logConsultarPedido(pedidoCons, idPedidoCons);
				free(pedidoCons);
				free(headerCons);
				break;			
        	default:
            	printf("Operación desconocida. Llegó el código: %d. No quieras meter la pata!!!(｀Д´*)\n", header->codigoOperacion);
            	break;
 	   	}
		free(header);
	}
	// TODO: Liberar conexion a comanda y sacar al cliente de la lista de conectados
    pthread_exit(EXIT_SUCCESS);
    return 0;
}

int main(int argc, char* argv[])
{
	inicializarProceso(APP);
	socketServidor = iniciarServidor();
	initApp();

	pthread_create(&threadPlanificacion, NULL, (void*)planificar, NULL);

	int fd;
	while (1) {
		fd = aceptarCliente(socketServidor);
		if (fd != ERROR) {
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
	
	pthread_kill(threadPlanificacion, SIGTERM);
	liberarConexion(socketServidor);
    finalizarProceso();
    return EXIT_SUCCESS;
}