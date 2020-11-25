#include "../include/app.h"

/* Funciones y métodos */

// Limpiamos clientes desconectados de una lista
void revisarConectados(t_list *lista) {
	for (int i = 0; i < list_size(lista); i++) {
		t_cliente *clienteActual = list_get(lista, i);
		if (recv(clienteActual->socketCliente, NULL, 1, MSG_PEEK | MSG_DONTWAIT) == 0) {
			list_remove(lista, i);
			log_debug(logger, "El cliente [#%d - %s] se desconectó", i, clienteActual->idCliente);
		}
	}
}

// Revisamos si el cliente ya existe en la lista de conectados y limpiar los desconectados
void actualizarClientesConectados(t_cliente *cliente) {
	revisarConectados(clientesConectados);
	revisarConectados(restaurantesConectados);

	bool estaDuplicado(void *actual) {
		t_cliente *clienteActual = actual;
		return string_equals_ignore_case(cliente->idCliente, clienteActual->idCliente);
	};

	if (cliente->esRestaurante) { // TODO: Revisar
		t_cliente *restDuplicado = list_find(restaurantesConectados, &estaDuplicado);
		if (restDuplicado != NULL || list_is_empty(restaurantesConectados)) { list_add(restaurantesConectados, cliente); }
	} else {
		t_cliente *clienteDuplicado = list_find(clientesConectados, &estaDuplicado);
		if (clienteDuplicado != NULL || list_is_empty(clientesConectados)) { list_add(clientesConectados, cliente); }
	}
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

t_cliente *getRestConectado(char *restBuscado) {
	bool restFound(void *actual) {
		t_cliente *restActual = actual;
		return string_equals_ignore_case(restBuscado, restActual->idCliente);
	};
	return list_find(restaurantesConectados, &restFound);
}

t_list *obtenerRestsConectados() {
	t_list *nombresRests = list_create();
	for (int i = 0; i < list_size(restaurantesConectados); i++) {
		t_cliente *currentRest = list_get(restaurantesConectados, i);
		list_add(nombresRests, currentRest->idCliente);
	}
	return nombresRests;
}

/* Hilo de planificación */

void *planificar(void *args) {
	while (1) {
		// Largo plazo
		actualizarQRconQN();
		actualizarQRconQB();
		switch (algoritmoSeleccionado) {
			case FIFO:
				actualizarQEconQR_FIFO();
				break;
			case HRRN:
				actualizarQEconQR_HRRN();
				break;
			case SJF:
				actualizarQEconQR_SJF();
				break;
			default:
				break;
		}
		// Corto plazo
		ejecutarCiclos();
	}
}

/* Hilo de conexiones */

void *atenderConexiones(void *conexionNueva)
{
    pthread_data *t_data = (pthread_data*) conexionNueva;
    int socketCliente = t_data->socketThread;
    free(t_data);

	t_cliente *cliente = malloc(sizeof(t_cliente));
	int socketComanda = ERROR;

	while (1) {
		
    	t_header *header = recibirHeaderPaquete(socketCliente);

		if (header->procesoOrigen == ERROR || header->codigoOperacion == ERROR) {
			logClientDisconnection(socketCliente); // TODO: Mejora en los logs de desconexión
			liberarConexion(socket);
    		pthread_exit(EXIT_SUCCESS);
			return EXIT_FAILURE;
		}

    	switch (header->codigoOperacion) {
			case OBTENER_PROCESO:;
				enviarPaquete(socketCliente, APP, RTA_OBTENER_PROCESO, APP);
				break;
			case ENVIAR_DATACLIENTE:;
				cliente = recibirPayloadPaquete(header, socketCliente);
				cliente->socketCliente = socketCliente;
				logClientInfo(cliente);
				actualizarClientesConectados(cliente);
				break;
        	case CONSULTAR_RESTAURANTES:;
				t_list *rta_cons_rest = _consultar_restaurantes();
				enviarPaquete(socketCliente, APP, RTA_CONSULTAR_RESTAURANTES, rta_cons_rest);
				free(rta_cons_rest);
        		break;
			case SELECCIONAR_RESTAURANTE:;
				t_selecc_rest *tupla_cliente_rest = recibirPayloadPaquete(header, socketCliente);
				t_result *rta_selec_rest = _seleccionar_restaurante(cliente, tupla_cliente_rest);
				enviarPaquete(socketCliente, APP, RTA_SELECCIONAR_RESTAURANTE, rta_selec_rest);
				free(rta_selec_rest);
				break;
			case CONSULTAR_PLATOS:; // Finished with annotations
				char *consulta = recibirPayloadPaquete(header, socketCliente); free(consulta);
				if (list_is_empty(restaurantesConectados)) {
					log_info(logger, "No hay restaurantes conectados, se enviará el RestauranteDefault...");
					enviarPaquete(socketCliente, APP, RTA_CONSULTAR_PLATOS, platosResDefault);		
					logRtaConsultarPlatos(platosResDefault);			
				} else {
					t_cliente *restConectado = getRestConectado(cliente->restSeleccionado); // ¿Qué pasa si no está?
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
			case CREAR_PEDIDO:; // TODO: Mejorar logs
				conexionComanda = conectarseA(COMANDA);
				t_request *reqGuardarPedido;

				if (list_is_empty(restaurantesConectados)) {
					int unRandom = generarId(0, 100);
					enviarPaquete(socketCliente, APP, RTA_CREAR_PEDIDO, unRandom);
					reqGuardarPedido = getTRequest(unRandom, restauranteDefault);
				} else {
					t_cliente *restConsulta = getRestConectado(cliente->restSeleccionado);
					enviarPaquete(restConsulta->socketCliente, APP, CREAR_PEDIDO, NULL);

					printf("Solicitando un nuevo idPedido para el cliente %d a RESTAURANTE...\n", restConsulta->idCliente);
					log_info(logger, "Solicitando un nuevo idPedido para el cliente %d a RESTAURANTE...", restConsulta->idCliente);
					
					t_header *headerRest = recibirHeaderPaquete(restConsulta->socketCliente);
					int newIdPedido = recibirPayloadPaquete(headerRest, restConsulta->socketCliente);
					
					printf("El nuevo idPedido es %d\n", newIdPedido);
					log_info(logger, "El nuevo idPedido es %d", newIdPedido);
					
					reqGuardarPedido = getTRequest(newIdPedido, restConsulta->idCliente);
					free(headerRest);
				}

				enviarPaquete(conexionComanda, APP, GUARDAR_PEDIDO, reqGuardarPedido);
				t_header *headerPedidoGuardado = recibirHeaderPaquete(conexionComanda);
				t_result *payloadPedidoGuardado = recibirPayloadPaquete(headerPedidoGuardado, conexionComanda);
				logTResult(payloadPedidoGuardado);
				enviarPaquete(socketCliente, APP, RTA_CREAR_PEDIDO, payloadPedidoGuardado->hasError ? ERROR : reqGuardarPedido->idPedido);

				free(reqGuardarPedido);
				free(headerPedidoGuardado);
				free(payloadPedidoGuardado);
				liberarConexion(conexionComanda);
				break;	
			case ANIADIR_PLATO:; // Finished with annotations
				conexionComanda = conectarseA(COMANDA);
				t_request *reqAniadir = recibirPayloadPaquete(header, socketCliente);
				logRequest(reqAniadir, header->codigoOperacion);

				// ¿No deberíamos obtener el RESTAURANTE dueño del pedido en lugar del RESTAURANTE seleccionado?
				// Si no fuese asi, podemos obtener el nombre del restaurante de CONSULTAR_PEDIDO
				t_cliente *restConectado = getRestConectado(cliente->restSeleccionado);

				t_header *hAniadir = malloc(sizeof(t_header));
				t_result *resAniadir = malloc(sizeof(t_result));

				// Si es restauranteDefault, enviamos t_request a COMANDA
				if (string_equals_ignore_case(restConectado->idCliente, restauranteDefault)) {
					enviarPaquete(conexionComanda, APP, ANIADIR_PLATO, reqAniadir);
					hAniadir = recibirHeaderPaquete(conexionComanda);
					resAniadir = recibirPayloadPaquete(hAniadir, conexionComanda);
					logTResult(resAniadir);
					enviarPaquete(socketCliente, APP, RTA_ANIADIR_PLATO, resAniadir);
				} else { // Sino, enviamos t_request al RESTAURANTE dueño del pedido
					enviarPaquete(restConectado->socketCliente, APP, ANIADIR_PLATO, reqAniadir);
					hAniadir = recibirHeaderPaquete(restConectado->socketCliente);
					resAniadir = recibirPayloadPaquete(hAniadir, restConectado->socketCliente);
					logTResult(resAniadir);
					// Si el RESTAURANTE dice que falló, informamos directamente al CLIENTE
					if (resAniadir->hasError) {
						enviarPaquete(socketCliente, APP, RTA_ANIADIR_PLATO, resAniadir);
					} else { // Sino, enviamos el t_request a COMANDA e informamos el resultado al CLIENTE
						enviarPaquete(conexionComanda, APP, ANIADIR_PLATO, reqAniadir);
						hAniadir = recibirHeaderPaquete(conexionComanda);
						resAniadir = recibirPayloadPaquete(hAniadir, conexionComanda);
						logTResult(resAniadir);
						enviarPaquete(socketCliente, APP, RTA_ANIADIR_PLATO, resAniadir);
					}
				}

				free(reqAniadir);
				free(restConectado);
				free(hAniadir);
				free(resAniadir);
				liberarConexion(conexionComanda);
				break;	
			case PLATO_LISTO:;
				conexionComanda = conectarseA(COMANDA);

				t_plato_listo *platoListo = recibirPayloadPaquete(header, socketCliente);
				enviarPaquete(conexionComanda, APP, PLATO_LISTO, platoListo);
				t_header *headerPL = recibirHeaderPaquete(conexionComanda);
				t_result *resultPL = recibirPayloadPaquete(headerPL, conexionComanda);
				logTResult(resultPL);

				// Obtener pedido de COMANDA para verificar cantidad de platos listos
				if (!resultPL->hasError) {
					t_request *reqPedidoAVerificar = getTRequest(platoListo->restaurante, platoListo->idPedido);
					enviarPaquete(conexionComanda, APP, OBTENER_PEDIDO, reqPedidoAVerificar);
					t_header *headerPO = recibirHeaderPaquete(conexionComanda);
					t_pedido *pedidoAVerificar = recibirPayloadPaquete(headerPO, conexionComanda);
					
					// Verificar si todos los platos están listos
					t_list *platosPedido = list_create(); list_add_all(platosPedido, pedidoAVerificar->platos);

					bool platoListo(void *actual) {
						t_plato *platoActual = actual;
						return platoActual->cantidadLista == platoActual->cantidadPedida;
					};

					bool todosPlatosListos = list_all_satisfy(pedidoAVerificar->platos, &platoListo);
					
					// int cantidadPlatos = list_size(platosPedido);
					// bool pedidoListo = true;
					// for (int i=0; i<cantidadPlatos; i++) {
					// 	t_plato *platoActual = list_get(platosPedido, i);
					// 	if (platoActual->cantidadLista < platoActual->cantidadPedida) { pedidoListo = false; }
					// }

					if (todosPlatosListos) {
						desbloquearPCB(reqPedidoAVerificar->idPedido);
					}
				}
				
				enviarPaquete(socketCliente, APP, RTA_PLATO_LISTO, resultPL);

				free(platoListo);
				free(headerPL);
				free(resultPL);
				liberarConexion(conexionComanda);
				break;	
			case CONFIRMAR_PEDIDO:;
				t_request *reqConf = recibirPayloadPaquete(header, socketCliente);
				logRequest(reqConf, header->codigoOperacion);

				// 1. Obtener el pedido de COMANDA
				enviarPaquete(conexionComanda, APP, OBTENER_PEDIDO, reqConf);
				t_header *headerCP = recibirHeaderPaquete(conexionComanda);
				t_pedido *pedidoConf = recibirPayloadPaquete(header, conexionComanda);
				logObtenerPedido(pedidoConf);

				bool hasError = false;
				t_pcb *pcb = malloc(sizeof(t_pcb));

				// 2. Si es RDefault, generar PCB y añadirlo para planificar, sino, consultar al restaurante que corresponda
				if (string_equals_ignore_case(cliente->restSeleccionado, restauranteDefault)) {
					// 3.Generar el PCB y dejarlo en planificación
					pcb = crearPcb(cliente, reqConf->idPedido);
					agregarAQN(pcb);
				} else {
					t_cliente *restConectado = getRestConectado(reqConf->nombre);
					enviarPaquete(restConectado->socketCliente, APP, CONFIRMAR_PEDIDO, reqConf);
					t_header *hRestConfP = recibirHeaderPaquete(restConectado->socketCliente);
					t_result *resultConfP = recibirPayloadPaquete(hRestConfP, restConectado->socketCliente);
					if (resultConfP->hasError) {
						hasError = true;
						enviarPaquete(socketCliente, APP, RTA_CONFIRMAR_PEDIDO, resultConfP);
					} else {
						// 3.Generar el PCB y dejarlo en planificación
						pcb = crearPcb(cliente, reqConf->idPedido);
						agregarAQN(pcb);
					}
				}

				// 4. Informar a COMANDA la actualización del estado del pedido
				if (!hasError) {
					enviarPaquete(conexionComanda, APP, CONFIRMAR_PEDIDO, reqConf);
					t_header *hComanda = recibirHeaderPaquete(conexionComanda);
					t_result *resultConfPComanda = recibirPayloadPaquete(hComanda, conexionComanda);
					logTResult(resultConfPComanda);
					// 5. Informar a CLIENTE que el pedido fue confirmado
					enviarPaquete(socketCliente, APP, RTA_CONFIRMAR_PEDIDO, resultConfPComanda);
				}

				break;
			case CONSULTAR_PEDIDO:;
				int idPedidoCons = recibirPayloadPaquete(header, socketCliente);
				conexionComanda = conectarseA(COMANDA);
				enviarPaquete(conexionComanda, APP, CONSULTAR_PEDIDO, idPedidoCons);
				t_header *headerConsCom = recibirHeaderPaquete(conexionComanda);
				t_pedido *pedidoCons = recibirPayloadPaquete(headerConsCom, conexionComanda);
				logConsultarPedido(pedidoCons, idPedidoCons);
				enviarPaquete(socketCliente, APP, RTA_CONSULTAR_PEDIDO, pedidoCons);
				free(headerConsCom);
				free(pedidoCons);
				break;			
        	default:
            	printf("Operación desconocida. Llegó el código: %d. No quieras meter la pata!!!(｀Д´*)\n", header->codigoOperacion);
            	break;
 	   	}
		// free(header);
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