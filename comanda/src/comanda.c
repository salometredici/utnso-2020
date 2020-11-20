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
				t_request *request_gp = recibirPayloadPaquete(header, socketCliente);				
				t_result *result_gp = _guardar_pedido(request_gp);
				free(request_gp);
				enviarPaquete(socketCliente, COMANDA, RTA_GUARDAR_PEDIDO, result_gp);
				free(result_gp);
				break;
			case GUARDAR_PLATO:;
				t_req_plato *request_gpl = recibirPayloadPaquete(header, socketCliente);
				t_result *result_gpl = _guardar_plato(request_gpl);				
				free(request_gpl);
				enviarPaquete(socketCliente, COMANDA, RTA_GUARDAR_PLATO, result_gpl);
				free(result_gpl);
				break;
			case OBTENER_PEDIDO:;
				t_request *request_obp = recibirPayloadPaquete(header, socketCliente);
				logRequest(request_obp, header->codigoOperacion);
				t_pedido *pedido = _obtener_pedido(request_obp);
				enviarPaquete(socketCliente, COMANDA, RTA_OBTENER_PEDIDO, pedido);
				free(request_obp);
				free(pedido); // creo que tengo que liberar lo que esta adentro
				break;
			case CONFIRMAR_PEDIDO:;
				t_request *request_conf = recibirPayloadPaquete(header, socketCliente);
				logRequest(request_conf, header->codigoOperacion);
				t_result *result_conf = _confirmar_pedido(request_conf);
				free(request_conf);
				enviarPaquete(socketCliente, COMANDA, RTA_CONFIRMAR_PEDIDO, result_conf);
				free(result_conf);
				break;
			case PLATO_LISTO:;
				t_plato_listo *request_pl = recibirPayloadPaquete(header, socketCliente);
				// TODO: Log?
				t_result *result_pl = _plato_listo(request_pl->restaurante, request_pl->idPedido, request_pl->plato);
				free(request_pl);
				enviarPaquete(socketCliente, COMANDA, RTA_PLATO_LISTO, result_pl);
				free(result_pl);
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