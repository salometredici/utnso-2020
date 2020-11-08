#include "../include/comanda.h"

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
				enviarPaquete(socketCliente, COMANDA, RTA_OBTENER_PROCESO, COMANDA);
				break;
			case GUARDAR_PEDIDO:;
				t_request *requestgp = recibirPayloadPaquete(header, socketCliente);				
				t_result *resultgp = _guardar_pedido(requestgp->nombre, requestgp->idPedido);
				free(requestgp);
				enviarPaquete(socketCliente, COMANDA, RTA_GUARDAR_PEDIDO, resultgp);
				free(resultgp);
				break;
			case GUARDAR_PLATO:;
				t_req_plato *requestgpl = recibirPayloadPaquete(header, socketCliente);
				t_result *resultgpl = _guardar_plato(requestgpl->restaurante, requestgpl->idPedido, requestgpl->plato, requestgpl->cantidadPlato);				
				free(requestgpl);
				enviarPaquete(socketCliente, COMANDA, RTA_GUARDAR_PLATO, resultgpl);
				free(resultgpl);
				break;
			case CONFIRMAR_PEDIDO:;
				t_request *reqConf = recibirPayloadPaquete(header, socketCliente);
				logRequest(reqConf, header->codigoOperacion);
				free(reqConf);
				t_result *rCP = malloc(sizeof(t_result));
				rCP->msg = "[CONFIRMAR_PEDIDO] Ok";
				rCP->hasError = false;
				enviarPaquete(socketCliente, COMANDA, RTA_CONFIRMAR_PEDIDO, rCP);
				free(rCP);
				break;
			case PLATO_LISTO:; // TODO: struct que recibe restaurante, idPedido y plato
				break;
			case OBTENER_PEDIDO:;
				/*t_request *reqObt = recibirPayloadPaquete(header, socketCliente);
				logRequest(reqObt, header->codigoOperacion);
				free(reqObt);

				t_pedido *pedido = malloc(sizeof(t_pedido)); t_list *platos = list_create();
				t_plato *milanesa = malloc(sizeof(t_plato)); t_plato *empanadas = malloc(sizeof(t_plato)); t_plato *ensalada = malloc(sizeof(t_plato));

				milanesa->plato = "Milanesa"; milanesa->precio = 200; milanesa->cantidadPedida = 2; milanesa->cantidadLista = 1;
				empanadas->plato = "Empanadas"; empanadas->precio = 880; empanadas->cantidadPedida = 12; empanadas->cantidadLista = 6;
				ensalada->plato = "Ensalada"; ensalada->precio = 120; ensalada->cantidadPedida = 1; ensalada->cantidadLista = 0;
				list_add(platos, milanesa); list_add(platos, empanadas); list_add(platos, ensalada);

				pedido->estado = PENDIENTE; pedido->platos = platos; pedido->precioTotal = calcularPrecioTotal(platos);

				enviarPaquete(socketCliente, COMANDA, RTA_OBTENER_PEDIDO, pedido);
				free(pedido); free(milanesa); free(empanadas); free(ensalada);*/
				break;
			case FINALIZAR_PEDIDO:;
				/*t_request *reqFin = recibirPayloadPaquete(header, socketCliente);
				logRequest(reqFin, header->codigoOperacion);
				free(reqFin);
				t_result *rFP = malloc(sizeof(t_result));
				rFP->msg = "[FINALIZAR_PEDIDO] Ok";
				rFP->hasError = false;
				enviarPaquete(socketCliente, COMANDA, RTA_FINALIZAR_PEDIDO, rFP);
	   			free(rFP);*/
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

int main(int argc, char **argv) {
	
	inicializarProceso(COMANDA);
    socketServidor = iniciarServidor();
	
	init_comanda();

    int fd;
    while (1) {
        fd = aceptarCliente(socketServidor);
        if (fd != -1) {
			pthread_data *t_data = (pthread_data *) malloc(sizeof(*t_data));
			t_data->socketThread = fd;
			pthread_create(&threadConexiones, NULL, (void*)atenderConexiones, t_data);
			pthread_detach(threadConexiones);
			logNewClientConnection(fd);
		} else {
			pthread_kill(threadConexiones, SIGTERM);
		}
    }

    liberarConexion(socketServidor);
    finalizarProceso();
    return EXIT_SUCCESS;
}