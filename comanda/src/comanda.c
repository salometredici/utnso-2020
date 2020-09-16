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
			log_info(logger, "El cliente %d se desconectó. Terminando el hilo...\n", socket);
			liberarConexion(socket);
    		pthread_exit(EXIT_SUCCESS);
			return EXIT_FAILURE;
		}

		log_info(logger, "Me llegaron los siguientes valores: %d %d\n", data->procesoOrigen, data -> codigoOperacion);

		switch (data->codigoOperacion) {
			case OBTENER_RESTAURANTE:
				payload = recibirPayloadPaquete(data, socket);
                log_info(logger, "Me llego: %d %s\n", payload->size ,payload->stream);
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