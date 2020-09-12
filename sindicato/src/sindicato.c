#include "../include/sindicato.h"

void mostrarComandosValidos() {
    printf("-------------------Comandos Válidos-------------------\n");
	printf("Ejemplo: AIUDA\n");
	printf("Ejemplo: CLEAR\n");
	printf("Ejemplo: BAI\n");
    printf("Formato: [MENSAJE] [PARAMETROS]\n");
	printf("Ejemplo: CREAR_RESTAURANTE [NOMBRE] [CANTIDAD_COCINEROS] [POSICION] [AFINIDAD_COCINEROS] [PLATOS] [PRECIO_PLATOS] [CANTIDAD_HORNOS]\n");
	printf("Ejemplo: CREAR_RECETA [NOMBRE] [PASOS] [TIEMPO_PASOS] \n");
    printf("------------------------------------------------------\n");
}

void* threadLecturaConsola(void * args) {
    printf("Iniciando la consola ...\n");
	printf("Para ver los comandos válidos, ingrese 'AIUDA'.\n");

	char *comandoOriginal;
	char **parametros;
	char *mensaje;
	int opcion;
    char *comandoLeido = readline("(=^.^=)~>");

    while (1) {
        add_history(comandoLeido);
        comandoOriginal = malloc(sizeof(char) *strlen(comandoLeido)+1);
        strcpy(comandoOriginal, comandoLeido);
        string_to_upper(comandoLeido);
        string_trim(&comandoLeido);

		// Parámetros
        parametros = string_split(comandoLeido, " ");
		mensaje = parametros[0];
        free(comandoOriginal);
		log_debug(logger, "Comando ingresado: %s", comandoLeido); // Por ahora, para ver lo que toma

        opcion = stringAKey(mensaje, CONSOLENKEYS);
		switch (opcion) {
			case OPT_CREAR_RESTAURANTE:
				printf("Crear Restaurante: Se deberá crear una nueva carpeta restaurante, con su respectivo info.AFIP explicado anteriormente\n");
				break;
			case OPT_CREAR_RECETA:
				printf("Crear Receta: Se deberá crear un nuevo archivo de receta siguiendo los lineamientos de lo detallado anteriormente.\n");
				break;
			case OPT_AIUDA:
				mostrarComandosValidos();
				break;
			case OPT_BAI:
				printf("adiosss (๑♡3♡๑)!\n");
				break;
			case OPT_CLEAR:
				limpiarPantalla();
				break;
			case ERROR:
			default:
				printf("Comando no válido. Escriba 'AIUDA' para ver el formato aceptado.\n");
				break;
		}

		free(mensaje);
        free(parametros);
        free(comandoLeido);
        if (opcion == OPT_BAI) { break; }

		comandoLeido = readline("(=^.^=)~>");
	}

    pthread_exit(EXIT_SUCCESS);
    return 0;
}

void iterator(char* value)	{
	printf("%s\n", value);
}

void *atenderConexiones(void *conexionNueva) {
    pthread_data *t_data = (pthread_data*) conexionNueva;
    int info = t_data->socketThread;
    free(t_data);
	
	// 
	t_list* lista;
	while(1) {
		//int cod_op = recibir_operacion(info);
		t_paquete* data = recibirHeaderPaquete(info);
		printf("Me llegaron los siguientes valores: %d %d\n", data->procesoOrigen,data->codigoOperacion);

		switch(data->codigoOperacion) {
			case OBTENER_RESTAURANTE:
				data = recibirPayloadPaquete(data, info);
				printf("Me llego: %d %s\n", data->buffer->size ,data->buffer->stream);

				char* mock = "RESPUESTA_NOMBRE_RESTAURANTE";
				t_paquete* pedido = crearPaquete(SINDICATO,RTA_OBTENER_RESTAURANTE, strlen(mock)+1, mock);
				enviarPaquete(pedido, info);
				break;
			case -1:
				log_error(logger, "el cliente se desconecto. Terminando servidor");
				return EXIT_FAILURE;
			default:
				printf("Operacion desconocida. No quieras meter la pata");
				break;
		}
	}

	// finalizar el hilo
    pthread_exit(EXIT_SUCCESS);
    return 0;
}

int main(int argc, char ** argv){
    inicializarProceso(SINDICATO);

	// Inicio del hilo de la consola y su lectura
	pthread_create(&threadConsola, NULL, (void *) threadLecturaConsola, NULL);
    pthread_detach(threadConsola);

	socketServidor = iniciarServidor();
	int fd;
	while (1) {
		//TODO: Lógica del Cliente
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

    finalizarProceso();
    return EXIT_SUCCESS;
}