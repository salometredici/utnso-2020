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
			case GUARDAR_PEDIDO:;
				t_req_pedido *reqGuardarPedido = recibirPayloadPaquete(header, socketCliente);
				logRequestPedido(reqGuardarPedido);
				free(reqGuardarPedido);
				// TODO: t_result
				char *rtaGuardarPedido = "[GUARDAR_PEDIDO] Ok\n";
				enviarPaquete(socketCliente, COMANDA, RTA_GUARDAR_PEDIDO, rtaGuardarPedido);
				break;
			case GUARDAR_PLATO:;
				t_req_plato *reqGuardarPlato = recibirPayloadPaquete(header, socketCliente);
				logRequestPlato(reqGuardarPlato);
				free(reqGuardarPlato);
				// TODO: t_result
				char *rtaGuardarPlato = "[GUARDAR_PLATO] Ok\n";
				enviarPaquete(socketCliente, COMANDA, RTA_GUARDAR_PLATO, rtaGuardarPlato);
				break;
			case CONFIRMAR_PEDIDO:;
				t_req_pedido *reqConf = recibirPayloadPaquete(header, socketCliente);
				logRequestPedido(reqConf);
				free(reqConf);
				// TODO: t_result
				char *rtaConfPedido = "[CONFIRMAR_PEDIDO] Ok\n";
				enviarPaquete(socketCliente, COMANDA, RTA_CONFIRMAR_PEDIDO, rtaConfPedido);
				break;
			case PLATO_LISTO:; // TODO: struct que recibe restaurante, idPedido y plato
				break;
			case OBTENER_PEDIDO:;
				t_req_pedido *reqObt = recibirPayloadPaquete(header, socketCliente);
				logRequestPedido(reqObt);
				free(reqObt);

				t_pedido *pedido = malloc(sizeof(t_pedido)); t_list *platos = list_create();
				t_plato *milanesa = malloc(sizeof(t_plato)); t_plato *empanadas = malloc(sizeof(t_plato)); t_plato *ensalada = malloc(sizeof(t_plato));

				milanesa->plato = "Milanesa"; milanesa->precio = 200; milanesa->cantidadPedida = 2; milanesa->cantidadLista = 1;
				empanadas->plato = "Empanadas"; empanadas->precio = 880; empanadas->cantidadPedida = 12; empanadas->cantidadLista = 6;
				ensalada->plato = "Ensalada"; ensalada->precio = 120; ensalada->cantidadPedida = 1; ensalada->cantidadLista = 0;
				list_add(platos, milanesa); list_add(platos, empanadas); list_add(platos, ensalada);

				pedido->estado = PENDIENTE; pedido->platos = platos; pedido->precioTotal = calcularPrecioTotal(platos);

				enviarPaquete(socketCliente, COMANDA, RTA_OBTENER_PEDIDO, pedido);
				free(pedido); free(milanesa); free(empanadas); free(ensalada);
				break;
			case FINALIZAR_PEDIDO:;
				t_req_pedido *reqFin = recibirPayloadPaquete(header, socketCliente);
				logRequestPedido(reqFin);
				free(reqFin);
				// TODO: t_result
				char *rtaFinalizarPedido = "[FINALIZAR_PEDIDO] Ok\n";
				enviarPaquete(socketCliente, COMANDA, RTA_FINALIZAR_PEDIDO, rtaFinalizarPedido);
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

    int fd;
    while(1) {
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