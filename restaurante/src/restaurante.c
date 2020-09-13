#include "../include/restaurante.h"
void iterator(char* value)	{
		printf("%s\n", value);
	}
void *atenderConexiones(void *conexionNueva)
{
    pthread_data *t_data = (pthread_data*) conexionNueva;
    int info = t_data->socketThread;
    free(t_data);
	
	t_list* lista;
	while(1) {
		//int cod_op = recibir_operacion(info);
		t_paquete* data = recibirHeaderPaquete(info);

		if(data->procesoOrigen == -1){
			close(socket);
			printf("El cliente se desconecto. Terminando servidor\n");
    		pthread_exit(EXIT_SUCCESS);
			return EXIT_FAILURE;
		}

		switch(data->codigoOperacion) {
			case RTA_OBTENER_RESTAURANTE:
				data = recibirPayloadPaquete(data, info);
				printf("Me llego: %s\n", data->buffer->stream);
				break;
			case -1:
				log_error(logger, "el cliente se desconecto. Terminando servidor");
				return EXIT_FAILURE;
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
	inicializarProceso(RESTAURANTE);
	socketServidor = iniciarServidor();
	conexionSindicato = conectarseA(SINDICATO);
	nombre = config_get_string_value(config, "NOMBRE_RESTAURANTE");

	// obtener metadata del restaurante al modulo sindicato
	t_paquete* pedido = crearPaquete(RESTAURANTE,OBTENER_RESTAURANTE, strlen(nombre)+1, nombre);
	enviarPaquete(pedido,conexionSindicato);

	t_paquete* data = recibirHeaderPaquete(conexionSindicato);
	printf("Me llegaron los siguientes valores: %d %d\n", data->procesoOrigen,data->codigoOperacion);
	data = recibirPayloadPaquete(data, conexionSindicato);
	printf("Me llego: %d %s\n", data->buffer->size ,data->buffer->stream);
  
	// creacion de las distintas colas de planificacion

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