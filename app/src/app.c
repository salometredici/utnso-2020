#include "../include/app.h"

void *atenderConexiones(void *conexionNueva)
{
    pthread_data *t_data = (pthread_data*) conexionNueva;
    int info = t_data->socketThread;
    free(t_data);

	// Acá va la lógica de envío/recepción de paquetes
	while (1) {
		t_buffer *payload;
    	t_header *data = recibirHeaderPaquete(info);

    	if (data->procesoOrigen == ERROR) {
        	close(socket);
        	printf("El cliente se desconecto. Terminando servidor\n");
        	pthread_exit(EXIT_SUCCESS);
        	return EXIT_FAILURE;
    	}

		printf("Me llegaron los siguientes valores: %d %d\n", data->procesoOrigen ,data -> codigoOperacion);

    	switch (data->codigoOperacion) {
        	case CONSULTAR_RESTAURANTES:
            	//no recibe parametros

            	restaurantes = list_create();
            	list_add(restaurantes,"Mc Donalds");
            	list_add(restaurantes,"Burguer King");
            	list_add(restaurantes,"Mostaza");
            	list_add(restaurantes,"Wendys");

            	enviarPaquete(info, APP, RTA_CONSULTAR_RESTAURANTES, getBytesAEnviarListaStrings(restaurantes), restaurantes);
        		break;
        	default:
            	printf("Operacion desconocida. No quieras meter la pata\n");
            	break;
 	   }
	}	

	// Esto sí tiene que estar para finalizar el hilo por ahora cuando le llegue la cadena vacía
    pthread_exit(EXIT_SUCCESS);
    return 0;
}

int main(int argc, char* argv[])
{
	inicializarProceso(APP);
	socketServidor = iniciarServidor();

    conexionComanda = conectarseA(COMANDA);

	int fd;
	while (1) {
		fd = aceptarCliente(socketServidor);
		if (fd != -1) {
			// Creo un nuevo hilo para la conexión aceptada
			pthread_data *t_data = (pthread_data *) malloc(sizeof(*t_data));
			t_data->socketThread = fd;
			pthread_create(&threadConexiones, NULL, (void*)atenderConexiones, t_data);
			pthread_detach(threadConexiones);
			log_info(logger, "Nuevo hilo para atender a Cliente con el socket %d", fd);
		}
	}
	
	liberarConexion(socketServidor);
    finalizarProceso();
    printf("Proceso finalizado");
    return EXIT_SUCCESS;
}