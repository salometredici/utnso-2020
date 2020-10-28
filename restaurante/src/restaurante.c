#include "../include/restaurante.h"




void *atenderConexiones(void *conexionNueva)
{
    pthread_data *t_data = (pthread_data*) conexionNueva;
    int socketCliente = t_data->socketThread;
    free(t_data);

	int socketSindicato = ERROR;

	while (1) {

		t_header *header = recibirHeaderPaquete(socketCliente);

		if (header->procesoOrigen == ERROR || header->codigoOperacion == ERROR) {
			logClientDisconnection(socketCliente);
			liberarConexion(socket);
    		pthread_exit(EXIT_SUCCESS);
			return EXIT_FAILURE;
		}

		socketSindicato = conectarseA(SINDICATO);

		switch (header->codigoOperacion) {
			case OBTENER_PROCESO:;
				enviarPaquete(socketCliente, RESTAURANTE, RTA_OBTENER_PROCESO, RESTAURANTE);
				break;
			case CONSULTAR_PLATOS:;
				char *restConsulta = recibirPayloadPaquete(header, socketCliente);
				logConsultaPlatos(restConsulta);
				t_list *platosRest = list_create();
				list_add(platosRest, "Milanesas");
				list_add(platosRest, "Lasagna");
				list_add(platosRest, "Asado");
				enviarPaquete(socketCliente, RESTAURANTE, RTA_CONSULTAR_PLATOS, platosRest);
				free(platosRest);
				break;
			case CREAR_PEDIDO:;
				int newIdPedido = 77;
				enviarPaquete(socketCliente, RESTAURANTE, RTA_CREAR_PEDIDO, newIdPedido);
				break;
			case ANIADIR_PLATO:;
				t_request *reqAniadir = recibirPayloadPaquete(header, socketCliente);
				logRequest(reqAniadir, header->codigoOperacion);
				free(reqAniadir);

				t_result *resAP = malloc(sizeof(t_result));
				resAP->hasError = false;
				resAP->msg = "[ANIADIR_PLATO] OK";
				enviarPaquete(socketCliente, RESTAURANTE, RTA_ANIADIR_PLATO, resAP);
				free(resAP);
				break;
			case CONFIRMAR_PEDIDO:;
				t_request *reqConf = recibirPayloadPaquete(header, socketCliente);
				logRequest(reqConf, header->codigoOperacion);
				// free(reqConf);

				// Sólo recibe el ID del pedido
				// 1. Obtener el Pedido desde Sindicato

				reqConf->nombre = nombreRestaurante;
				enviarPaquete(conexionSindicato, RESTAURANTE, OBTENER_PEDIDO, reqConf);
				t_header *hRConf = recibirHeaderPaquete(conexionSindicato);
				t_pedido *pedidoConf = recibirPayloadPaquete(header, conexionSindicato);
				mostrarListaPlatos(pedidoConf->platos);
				free(hRConf);

				// 2. Generar PCB de cada plato y dejarlo en el ciclo de planificación
				// Obtener receta de Sindicato para saber trazabilidad al momento de ejecución
				// El número de pedido se deberá guardar dentro del PCB

				



				// 3. Informar a quien lo invocó que su pedido fue confirmado

				t_result *resCP = malloc(sizeof(t_result));
				resCP->hasError = false;
				resCP->msg = "[CONFIRMAR_PEDIDO] OK";
				enviarPaquete(socketCliente, SINDICATO, RTA_CONFIRMAR_PEDIDO, resCP);
				free(resCP);
				break;
			case CONSULTAR_PEDIDO: // TODO: El model del TP incluye un restaurante, que falta agregar a nuestro t_pedido
				break;
			default:
				printf("Operación desconocida. Llegó el código: %d. No quieras meter la pata!!!(｀Д´*)\n", header->codigoOperacion);
				break;
		}
		free(header);
	}

	liberarConexion(socketSindicato); // si no es error, lo liberamo'
    pthread_exit(EXIT_SUCCESS);
    return 0;
}





/* Planificación FIFO */

// Si hay repartidores disponibles y PCBs en NEW, los asigna y añade a READY
void actualizarQR() {
	void _evaluarCocineros(void* element){
		t_queue_obj *queue = malloc(sizeof(t_queue_obj));
		queue = element;
		while (queue->instanciasTotales!=0 && !queue_is_empty(queue->qN) ) { 
			//con los mutex es suficiente o tengo que chequear la cantidad de hornos?
			pthread_mutex_lock(&mutexQN);
			t_proceso *nextInLine = queue_pop(queue->qN); // Lo sacamos de la cola de NEW
			pthread_mutex_unlock(&mutexQN);
			
			
			nextInLine->horno = "horno??";
			nextInLine->estado = "ESPERANDO_EJECUCION";// necesario?

			pthread_mutex_lock(&mutexQR);
			queue_push(queue->qR, nextInLine); // Pasa a la cola de READY
			pthread_mutex_unlock(&mutexQR);
			queue->instanciasTotales-=1;
		}
		free(queue);
	};

	list_iterate(queuesCocineros, _evaluarCocineros);
}

void *planificar(void *args) { // FIFO por ahora
	// TODO: Semáforo
	while (1) {
		switch (algoritmoSeleccionado) {
			case FIFO:
				// Largo plazo
				actualizarQR();
				//actualizarQE();
				
				// Corto plazo
				//ejecutarCiclosFIFO();
				break;
			case RR:
				break;
			default:
				break;
		}
	}
}

void obtenerNombreServidor() {
	enviarPaquete(conexion, RESTAURANTE, OBTENER_PROCESO, NULL);
	t_header *rtaProceso = recibirHeaderPaquete(conexion);
	procesoConectado = intToPCode(recibirPayloadPaquete(rtaProceso, conexion));
	logConnectionCliente(procesoConectado);
	free(rtaProceso);
}
int main(int argc, char *argv[])
{
	inicializarProceso(RESTAURANTE);
	conexion = iniciarServidor();//conectarseA(RESTAURANTE);
	initRestaurante();
	//obtenerNombreServidor();//??
	pthread_create(&threadPlanificacion, NULL, (void*)planificar, NULL);

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
			logNewClientConnection(fd);
		} else {
			pthread_kill(threadConexiones, SIGTERM);
		}
	}
	
	pthread_kill(threadPlanificacion, SIGTERM);
	liberarConexion(conexion);
    finalizarProceso();
    return EXIT_SUCCESS;
}