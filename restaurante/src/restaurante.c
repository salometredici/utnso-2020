#include "../include/restaurante.h"

void *atenderConexiones(void *conexionNueva)
{
    pthread_data *t_data = (pthread_data*) conexionNueva;
    int info = t_data->socketThread;
    free(t_data);

	// Por ahora hasta que le agreguemos su propia lógica
	char* leido = readline(">");
	while(strncmp(leido, "", 1) != 0) {
		log_info(logger, "Recibí un mensaje del socket %d: %s", info, leido);
		free(leido);
		leido = readline(">");
	}
	free(leido);

	// Esto sí tiene que estar para finalizar el hilo por ahora cuando le llegue la cadena vacía
    pthread_exit(EXIT_SUCCESS);
    return 0;
}

int main(int argc, char* argv[])
{
	inicializarProceso(RESTAURANTE);
	socketServidor = iniciarServidor();
	// Inicio del bucle que va a generar los diferentes hilos de conexión
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
    return EXIT_SUCCESS;
}