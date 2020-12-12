#include "../include/restaurante.h"

// Limpiar clientes desconectados
void revisarConectados(t_list *lista) {
	if(lista != NULL){
		int size = list_size(lista);
		for (int i = 0; i < size; i++) {
			t_cliente *clienteActual = list_get(lista, i);
			if (recv(clienteActual->socketCliente, NULL, 1, MSG_PEEK | MSG_DONTWAIT) == 0) {
				list_remove(lista, i);
				log_debug(logger, "El cliente [#%d - %s] se desconectó", i, clienteActual->idCliente);
			}
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

// Revisar si el cliente ya existe en la lista de conectados y limpiar los desconectados
void actualizarClientesConectados(t_cliente *cliente) {
    revisarConectados(clientesConectados);

    bool estaDuplicado(void *actual) {
        t_cliente *clienteActual = actual;
        return string_equals_ignore_case(cliente->idCliente, clienteActual->idCliente);
    };

	t_cliente *cliDuplicado = list_find(clientesConectados, &estaDuplicado);
	if (cliDuplicado == NULL) { list_add(clientesConectados, cliente); }
}

void crearProceso(t_cliente *cli, int idPedido, char *plato, int cantPedida){
	int conexionSindicato = conectarseA(SINDICATO);
	
	enviarPaquete(conexionSindicato, RESTAURANTE, ENVIAR_NOMBRE, nombreRestaurante);

	enviarPaquete(conexionSindicato, RESTAURANTE, OBTENER_RECETA, plato);
	t_header *hRConf = recibirHeaderPaquete(conexionSindicato);
	t_receta *receta = recibirPayloadPaquete(hRConf, conexionSindicato);
	liberarConexion(conexionSindicato);

	// t_receta *receta = malloc(sizeof(t_receta));
	// receta->plato = plato;
	// receta->instrucciones = instrucciones;
	for(int i = 0; i < cantPedida; i++) {
		t_proceso *proceso = crearPcb(cli, idPedido, receta);
		aReadyPorAfinidad(proceso); // agregar a queue de ready 
	}
}

void *atenderConexiones(void *conexionNueva)
{
    pthread_data *t_data = (pthread_data*) conexionNueva;
    int socketCliente = t_data->socketThread;
    free(t_data);
	t_cliente *cliente = malloc(sizeof(t_cliente));

	while (1) {
		t_header *header = recibirHeaderPaquete(socketCliente);

		if (header->procesoOrigen == ERROR || header->codigoOperacion == ERROR) {
			log_common_client_disconnection(socketCliente);
			liberarConexion(socketCliente);
    		pthread_exit(EXIT_SUCCESS);
			return EXIT_FAILURE;
		}

		switch (header->codigoOperacion) {
			case ENVIAR_DATACLIENTE:;
				cliente = recibirPayloadPaquete(header, socketCliente);
				cliente->socketCliente = socketCliente;
				actualizarClientesConectados(cliente);
				log_rta_EnviarDataCliente(cliente);
				break;
			case OBTENER_PROCESO:;
				enviarPaquete(socketCliente, RESTAURANTE, RTA_OBTENER_PROCESO, RESTAURANTE);
				break;
			case CONSULTAR_PLATOS:;
				char *restConsulta = recibirPayloadPaquete(header, socketCliente); free(restConsulta);
				// Se consultan los platos de este restaurante a Sindicato
				conexionSindicato = conectarseA(SINDICATO);
				enviarPaquete(conexionSindicato, RESTAURANTE, CONSULTAR_PLATOS, nombreRestaurante);
				t_header *hConsulta = recibirHeaderPaquete(conexionSindicato);
				t_list *platosRest = recibirPayloadPaquete(hConsulta, conexionSindicato);
				liberarConexion(conexionSindicato);
				free(hConsulta);
				// Se contesta con los platos obtenidos
				enviarPaquete(socketCliente, RESTAURANTE, RTA_CONSULTAR_PLATOS, platosRest); 
				
				//free(platosRest);
				break;
			case CREAR_PEDIDO:;
				pthread_mutex_lock(&mutexQPedidos);
				cantidadPedidos++;
				pthread_mutex_unlock(&mutexQPedidos);
				t_request *reqcrearPedido = malloc(sizeof(t_request));
				reqcrearPedido->idPedido = cantidadPedidos;
				reqcrearPedido->nombre = nombreRestaurante;

				conexionSindicato = conectarseA(SINDICATO);
				enviarPaquete(conexionSindicato, RESTAURANTE, GUARDAR_PEDIDO, reqcrearPedido);
				t_header *hrRtaGuardarPedido = recibirHeaderPaquete(conexionSindicato);
				t_result *reqRtaGuardarPedido = recibirPayloadPaquete(hrRtaGuardarPedido, conexionSindicato);
				liberarConexion(conexionSindicato);
				
				enviarPaquete(socketCliente, RESTAURANTE, RTA_CREAR_PEDIDO, reqRtaGuardarPedido->hasError? ERROR : cantidadPedidos);
				free(hrRtaGuardarPedido);
				free(reqRtaGuardarPedido);
				free(reqcrearPedido);
				break;
			case ANIADIR_PLATO:;
				// tiene que tener un plato y un id pedido
				t_request *reqAniadir = recibirPayloadPaquete(header, socketCliente);
				logRequest(reqAniadir, header->codigoOperacion);

				t_req_plato *reqPlato= malloc(sizeof(t_req_plato));
				reqPlato->idPedido = reqAniadir->idPedido;
				reqPlato->restaurante = nombreRestaurante;
				reqPlato->plato = reqAniadir->nombre;
				reqPlato->cantidadPlato = 1;

				conexionSindicato = conectarseA(SINDICATO);
				enviarPaquete(conexionSindicato, RESTAURANTE, GUARDAR_PLATO, reqPlato);
				t_header *hrRtaGuardarPlato = recibirHeaderPaquete(conexionSindicato);
				t_result *reqRtaGuardarPlato = recibirPayloadPaquete(hrRtaGuardarPlato, conexionSindicato);
				liberarConexion(conexionSindicato);
				free(hrRtaGuardarPlato);
				// if(reqRtaGuardarPlato->hasError){
				// 	crearProceso(cliente, reqPlato->idPedido, reqPlato->plato);
				// }
				// ver si solo se puede hacer si el pedido no esta confirmado 
				
				enviarPaquete(socketCliente, RESTAURANTE, RTA_ANIADIR_PLATO, reqRtaGuardarPlato);
				break;
			case CONFIRMAR_PEDIDO:;
				t_request *reqConf2 = recibirPayloadPaquete(header, socketCliente);
				logRequest(reqConf2, header->codigoOperacion);
				reqConf2->nombre = nombreRestaurante;
				t_result *resultadoGral = malloc(sizeof(t_result));

				conexionSindicato = conectarseA(SINDICATO);
				enviarPaquete(conexionSindicato, RESTAURANTE, CONFIRMAR_PEDIDO, reqConf2);
				t_header *hconfirmarpedido = recibirHeaderPaquete(conexionSindicato);
				resultadoGral = recibirPayloadPaquete(hconfirmarpedido, conexionSindicato);
				liberarConexion(conexionSindicato);

				if(!resultadoGral->hasError) {
					conexionSindicato = conectarseA(SINDICATO);
					enviarPaquete(conexionSindicato, RESTAURANTE, OBTENER_PEDIDO, reqConf2);
					t_header *hRConf2 = recibirHeaderPaquete(conexionSindicato);
					t_pedido *pedidoConf2 = recibirPayloadPaquete(hRConf2, conexionSindicato);
					liberarConexion(conexionSindicato);
					
					int cantDePlatos = list_size(pedidoConf2->platos);
					for (int i = 0; i < cantDePlatos; i++){
						//conseguir receta de sindicato //MENSAJE OBTENER RECETA
						t_plato *platoActual = malloc(sizeof(t_plato));
						// t_plato *current = list_get(pedidoConf2->platos,i);
						platoActual = list_get(pedidoConf2->platos,i); //current->plato;
						crearProceso(cliente, reqConf2->idPedido, platoActual->plato, platoActual->cantidadPedida);
					}
					log_rta_ObtenerPedido(pedidoConf2,reqConf2);

					resultadoGral->hasError = false;
					resultadoGral->msg = "[CONFIRMAR_PEDIDO] OK";
				}
					
				// 3. Informar a quien lo invocó que su pedido fue confirmado
				enviarPaquete(socketCliente, RESTAURANTE, RTA_CONFIRMAR_PEDIDO, resultadoGral);
				free(resultadoGral);
				break;
			case CONSULTAR_PEDIDO:; // TODO: El model del TP incluye un restaurante, que falta agregar a nuestro t_pedido
				t_request *reqConsultarPedido = malloc(sizeof(t_request));
				int idConsultPedido = recibirPayloadPaquete(header, socketCliente);
				log_ConsultarPedido(idConsultPedido);
				reqConsultarPedido->nombre = nombreRestaurante;
				reqConsultarPedido->idPedido = idConsultPedido;
				
				conexionSindicato = conectarseA(SINDICATO);
				enviarPaquete(conexionSindicato, RESTAURANTE, OBTENER_PEDIDO , reqConsultarPedido);
				t_header *hRConsultarP = recibirHeaderPaquete(conexionSindicato);
				t_pedido *pedidoConsultar = recibirPayloadPaquete(hRConsultarP, conexionSindicato);
				liberarConexion(conexionSindicato);
				pedidoConsultar->restaurante = nombreRestaurante;
				enviarPaquete(socketCliente, RESTAURANTE, RTA_CONSULTAR_PEDIDO, pedidoConsultar);
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

void *planificar(void *arg) {
	int queuePosition = (int)arg;
	// TODO: Semáforo
	t_queue_obj *currentCPU = list_get(queuesCocineros, queuePosition);
	log_planif_step("Hilo afinidad", currentCPU->afinidad);
	while (1) {
		switch (algoritmoSeleccionado) {
			case FIFO:
				// Largo plazo
				actualizarQB(currentCPU);
				actualizarQRaQE(currentCPU);
				ejecutarCicloIO(currentCPU);
				
				// Corto plazo
				ejecutarCiclosFIFO(currentCPU);
				break;
			case RR:
				// Largo plazo //se pueden reutilizar??
				actualizarQB(currentCPU);
				actualizarQRaQE(currentCPU);		
				ejecutarCicloIO(currentCPU);

				// Corto plazo
				ejecutarCiclosRR(currentCPU);
				break;
			default:
				break;
		}
	}
}

startPlanificacion(){
	int size = list_size(queuesCocineros);
	for(int i = 0; i < size; i++) {
		pthread_t threadPlanificacionAfinidad;
		pthread_create(&threadPlanificacionAfinidad, NULL, (void*)planificar, i);
	}
}

int main(int argc, char *argv[]) {
	inicializarProceso(RESTAURANTE);
	conexion = iniciarServidor();//conectarseA(RESTAURANTE);
	initRestaurante();
	startPlanificacion();

	// Inicio del bucle que va a generar los diferentes hilos de conexión
	int fd;
	while (1) {
		fd = aceptarCliente(conexion);
		if (fd != -1) {
			// Creo un nuevo hilo para la conexión aceptada
			pthread_data *t_data = (pthread_data *) malloc(sizeof(*t_data));
			t_data->socketThread = fd;
			pthread_create(&threadConexiones, NULL, (void*)atenderConexiones, t_data);
			pthread_detach(threadConexiones);
			log_new_client_connection(fd);
		} else {
			pthread_kill(threadConexiones, SIGTERM);
		}
	}
	
	//pthread_kill(threadPlanificacion, SIGTERM); ver si matamos todas las planificaciones
	liberarConexion(conexion);
    finalizarProceso();
    return EXIT_SUCCESS;
}