#include "../include/restaurante.h"

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
	if (cliDuplicado != NULL) { list_add(clientesConectados, cliente); }
}

void crearProceso(t_cliente *cliente, int idPedido, char *plato){
	enviarPaquete(conexionSindicato, RESTAURANTE, OBTENER_RECETA, plato);
	t_header *hRConf = recibirHeaderPaquete(conexionSindicato);
	t_list *instrucciones = recibirPayloadPaquete(hRConf, conexionSindicato);

	t_receta *receta = malloc(sizeof(t_receta));
	receta->plato = plato;
	receta->instrucciones = instrucciones;

	t_proceso *proceso = crearPcb(cliente, idPedido, receta);

	agregarQueue(1, proceso); // agregar a queue de ready 
}

void *atenderConexiones(void *conexionNueva)
{
    pthread_data *t_data = (pthread_data*) conexionNueva;
    int socketCliente = t_data->socketThread;
    free(t_data);

	int socketSindicato = ERROR;
	t_cliente *cliente = getCliente(socketCliente);
	actualizarClientesConectados(cliente);

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
				char *restConsulta = recibirPayloadPaquete(header, socketCliente); free(restConsulta);
				// Se consultan los platos de este restaurante a Sindicato
				enviarPaquete(conexionSindicato, RESTAURANTE, CONSULTAR_PLATOS, nombreRestaurante);
				t_header *hConsulta = recibirHeaderPaquete(conexionSindicato);
				t_list *platosRest = recibirPayloadPaquete(hConsulta, conexionSindicato);
				free(hConsulta);
				// Se contesta con los platos obtenidos
				enviarPaquete(socketCliente, RESTAURANTE, RTA_CONSULTAR_PLATOS, platosRest);
				free(platosRest);
				break;
			case CREAR_PEDIDO:;
				int newIdPedido = 77;
				enviarPaquete(socketCliente, RESTAURANTE, RTA_CREAR_PEDIDO, newIdPedido);
				break;
			case ANIADIR_PLATO:;
				// tiene que tener un plato y un id pedido
				t_request *reqAniadir = recibirPayloadPaquete(header, socketCliente);
				logRequest(reqAniadir, header->codigoOperacion);
				free(reqAniadir);

				t_req_plato *reqPlato= malloc(sizeof(t_req_plato));

				reqPlato->idPedido = reqAniadir->idPedido;
				reqPlato->restaurante = cliente->idCliente;
				reqPlato->plato = reqAniadir->nombre;
				reqPlato->cantidadPlato = 1;

				enviarPaquete(socketSindicato, RESTAURANTE, GUARDAR_PLATO, reqPlato);
				t_header *hrRtaGuardarPlato = recibirHeaderPaquete(conexionSindicato);
				t_result *reqRtaGuardarPlato = recibirPayloadPaquete(header, socketCliente);
				logRequest(hrRtaGuardarPlato, header->codigoOperacion);
				free(hrRtaGuardarPlato);
				if(reqRtaGuardarPlato->hasError){
					crearProceso(cliente, reqPlato->idPedido, reqPlato->plato);
				}

				t_result *resAP = malloc(sizeof(t_result));
				resAP->hasError = reqRtaGuardarPlato->hasError;
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
				
				// por cada plato pedir la receta y generar pcb
				for (int i = 0;i<list_size(pedidoConf->platos);i++){
					//conseguir receta de sindicato //MENSAJE OBTENER RECETA
					char *platoActual = list_get(pedidoConf->platos,i);
					crearProceso(cliente, reqConf->idPedido, platoActual);
				}

				// 3. Informar a quien lo invocó que su pedido fue confirmado

				t_result *resCP = malloc(sizeof(t_result));
				resCP->hasError = false;
				resCP->msg = "[CONFIRMAR_PEDIDO] OK";
				enviarPaquete(socketCliente, SINDICATO, RTA_CONFIRMAR_PEDIDO, resCP);
				free(resCP);
				break;
			case CONSULTAR_PEDIDO: // TODO: El model del TP incluye un restaurante, que falta agregar a nuestro t_pedido
				// t_request *reqConf = recibirPayloadPaquete(header, socketCliente);
				// logRequest(reqConf, header->codigoOperacion);
				// //reqConf->idPedido;
				// //devuelve:
				// //{Restaurante, estado,[{comida1, cantTotal1, cantLista1}...]}
				// t_pedido *pedido = malloc(sizeof(t_pedido));
				// pedido->restaurante = nombreRestaurante;
				// pedido->estado = "";
				
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

int main(int argc, char *argv[])
{
	inicializarProceso(RESTAURANTE);
	conexion = iniciarServidor();//conectarseA(RESTAURANTE);
	initRestaurante();
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