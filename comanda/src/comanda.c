#include "../include/comanda.h"

void *atenderConexiones(void *conexionNueva)
{
    pthread_data *t_data = (pthread_data*) conexionNueva;
    int socket = t_data->socketThread;
    free(t_data);
	
	while(1) {
		t_buffer *payload;
        t_header *data = recibirHeaderPaquete(socket);

		if (data->procesoOrigen == ERROR || data->codigoOperacion == ERROR) {
			log_info(logger, "El cliente %d se desconectó. Terminando el hilo...", socket);
			liberarConexion(socket);
    		pthread_exit(EXIT_SUCCESS);
			return EXIT_FAILURE;
		}

		log_info(logger, "Me llegaron los siguientes valores: %d %d", data->procesoOrigen, data -> codigoOperacion);

		switch (data->codigoOperacion) {
			case OBTENER_RESTAURANTE:
				payload = recibirPayloadPaquete(data, socket);
                log_info(logger, "Me llego: %d %s\n", payload->size ,payload->stream);
			break;

			// Mensajes desde CLIENTE
			case GUARDAR_PEDIDO:
				printf("Operacion recibida: GUARDAR_PEDIDO\n");
				respuesta = "Mensaje de respuesta a GUARDAR_PEDIDO";
				enviarPaquete(socket, COMANDA, RTA_GUARDAR_PEDIDO, respuesta);
				break;
			case GUARDAR_PLATO:
				printf("Operacion recibida: GUARDAR_PLATO\n");
				respuesta = "Mensaje de respuesta a GUARDAR_PLATO";
				enviarPaquete(socket, COMANDA, RTA_GUARDAR_PLATO, respuesta);
				break;
			case CONFIRMAR_PEDIDO:
				printf("Operacion recibida: CONFIRMAR_PEDIDO\n");
				respuesta = "Mensaje de respuesta a CONFIRMAR_PEDIDO";
				enviarPaquete(socket, COMANDA, RTA_CONFIRMAR_PEDIDO, respuesta);
				break;
			case PLATO_LISTO:
				printf("Operacion recibida: PLATO_LISTO\n");
				respuesta = "Mensaje de respuesta a PLATO_LISTO";
				enviarPaquete(socket, COMANDA, RTA_PLATO_LISTO, respuesta);
				break;
			case OBTENER_PEDIDO:
				printf("Operacion recibida: OBTENER_PEDIDO\n");
				respuesta = "Mensaje de respuesta a OBTENER_PEDIDO";
				enviarPaquete(socket, COMANDA, RTA_OBTENER_PEDIDO, respuesta);
				break;
			default:
				printf("Operacion desconocida. No quieras meter la pata!!!\n");
				break;
		}
	}
	
    pthread_exit(EXIT_SUCCESS);
    return 0;
}

int main(int argc, char ** argv){
    
	inicializarProceso(COMANDA);

    socketServidor = iniciarServidor();

    int fd;

    while(1){
        fd = aceptarCliente(socketServidor);
        if (fd != -1) {
			pthread_data *t_data = (pthread_data *) malloc(sizeof(*t_data));
			t_data->socketThread = fd;
			pthread_create(&threadConexiones, NULL, (void*)atenderConexiones, t_data);
			pthread_detach(threadConexiones);
			log_info(logger, "Nuevo hilo para atender a App con el socket %d", fd);
		}
		else		
			pthread_kill(threadConexiones, SIGTERM);
		
    }

    liberarConexion(socketServidor);
    finalizarProceso();
    return EXIT_SUCCESS;
}