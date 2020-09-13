#include "../include/comanda.h"

void *atenderConexiones(void *conexionNueva)
{
    pthread_data *t_data = (pthread_data*) conexionNueva;
    int socket = t_data->socketThread;
    free(t_data);
	
	while(1) {
		t_paquete* data = recibirHeaderPaquete(socket);

		if(data->procesoOrigen == -1){
			close(socket);
			printf("El cliente se desconecto. Terminando servidor\n");
    		pthread_exit(EXIT_SUCCESS);
			return EXIT_FAILURE;
		}

		printf("Me llegaron los siguientes valores: %d %d\n", data->procesoOrigen ,data -> codigoOperacion);

		switch(data->codigoOperacion) {
			case OBTENER_RESTAURANTE:
				data = recibirPayloadPaquete(data, socket);
				printf("Me llego: %d %s\n", data->buffer->size ,data->buffer->stream);							
			break;
			default:
				printf("Operacion desconocida. No quieras meter la pata\n");
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
		}
		else		
			pthread_kill(threadConexiones, SIGTERM);
		
    }

    liberarConexion(socketServidor);
    finalizarProceso();
    return EXIT_SUCCESS;
}