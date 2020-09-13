#include "../include/app.h"

void *atenderConexiones(void *conexionNueva)
{
    pthread_data *t_data = (pthread_data*) conexionNueva;
    int info = t_data->socketThread;
    free(t_data);
	
	// Acá va la lógica de envío/recepción de paquetes

	// Esto sí tiene que estar para finalizar el hilo por ahora cuando le llegue la cadena vacía
    pthread_exit(EXIT_SUCCESS);
    return 0;
}

int main(int argc, char* argv[])
{
	inicializarProceso(APP);
	socketServidor = iniciarServidor();

    conexionComanda = conectarseA(COMANDA);	
	t_paquete* pedido = crearPaquete(RESTAURANTE,OBTENER_RESTAURANTE, strlen("HOLASOYAPP")+1, "HOLASOYAPP");
	enviarPaquete(pedido,conexionComanda);

	int fd;
	while (1) {
		fd = aceptarCliente(socketServidor);
		if (fd != -1) {
			// Creo un nuevo hilo para la conexión aceptada
			pthread_data *t_data = (pthread_data *) malloc(sizeof(*t_data));
			t_data->socketThread = fd;
			pthread_create(&threadConexiones, NULL, (void*)atenderConexiones, t_data);
			pthread_detach(threadConexiones);
		}
	}
	
	liberarConexion(socketServidor);
    finalizarProceso();
    printf("Proceso finalizado");
    return EXIT_SUCCESS;
}