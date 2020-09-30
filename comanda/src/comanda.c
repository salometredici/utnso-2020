#include "../include/comanda.h"

void *atenderConexiones(void *conexionNueva)
{
    pthread_data *t_data = (pthread_data*) conexionNueva;
    int socketCliente = t_data->socketThread;
    free(t_data);
	
	while(1) {
        t_header *data = recibirHeaderPaquete(socketCliente);

		if (data->procesoOrigen == ERROR || data->codigoOperacion == ERROR) {
			logClientDisconnection(socketCliente);
			liberarConexion(socket);
    		pthread_exit(EXIT_SUCCESS);
			return EXIT_FAILURE;
		}

		switch (data->codigoOperacion) {
			case OBTENER_RESTAURANTE:;
				t_posicion *posicion = recibirPayloadPaquete(data, socketCliente);
                log_info(logger, "Me llego: %d %s\n", posicion->posX, posicion->posY);
			break;

			// Mensajes desde CLIENTE
			case GUARDAR_PEDIDO:;
				t_req_pedido *request = recibirPayloadPaquete(data, socketCliente);
				logRequestPedido(request);

				respuesta = "Mensaje de respuesta a GUARDAR_PEDIDO";
				//devuelve ok/fail
				enviarPaquete(socketCliente, COMANDA, RTA_GUARDAR_PEDIDO, respuesta);
				break;
			case GUARDAR_PLATO:;
				t_req_plato *plato = recibirPayloadPaquete(data, socketCliente);
				logRequestPlato(plato);

				respuesta = "Mensaje de respuesta a GUARDAR_PLATO";
				//devuelve ok/fail
				enviarPaquete(socketCliente, COMANDA, RTA_GUARDAR_PLATO, respuesta);
				break;
			case CONFIRMAR_PEDIDO:;
				t_req_pedido *requestPedido = recibirPayloadPaquete(data, socketCliente);
				logRequestPedido(requestPedido);
				
				respuesta = "Mensaje de respuesta a CONFIRMAR_PEDIDO";
				//devuelve ok/fail
				enviarPaquete(socketCliente, COMANDA, RTA_CONFIRMAR_PEDIDO, respuesta);
				break;
			case PLATO_LISTO:
				// Esperando chequedo del plato
				//payload = recibirPayloadPaquete(data, socketCliente);

				//respuesta = "Mensaje de respuesta a PLATO_LISTO";
				//enviarPaquete(socketCliente, COMANDA, RTA_PLATO_LISTO, respuesta);
				break;
			case OBTENER_PEDIDO:;
				char *nombreRestaurante = recibirPayloadPaquete(data, socketCliente);
				logMetadataRequest(nombreRestaurante);

				t_posicion *posicionRestaurante = malloc(sizeof(t_posicion));
				posicionRestaurante->posX = 25; posicionRestaurante->posY = 45; // Ejemplo de envío de una rta con un struct t_posicion

				enviarPaquete(socketCliente, COMANDA, RTA_OBTENER_RESTAURANTE, posicionRestaurante);
				
				break;

			case FINALIZAR_PEDIDO:;
				t_req_pedido *requestF = recibirPayloadPaquete(data, socketCliente);
				logRequestPedido(requestF);
				
				respuesta = "Mensaje de respuesta a FINALIZAR_PEDIDO";
				//devuelve ok/fail
				enviarPaquete(socketCliente, COMANDA, RTA_FINALIZAR_PEDIDO, respuesta);
				break;
			default:
				printf("Operacion desconocida. No quieras meter la pata!!!\n");
				break;
		}
	}
	
    pthread_exit(EXIT_SUCCESS);
    return 0;
}

int main(int argc, char ** argv) {
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