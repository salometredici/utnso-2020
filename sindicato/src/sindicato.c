#include "../include/sindicato.h"

void mostrarComandosValidos() {
    printf("-------------------Comandos Válidos-------------------\n");
    printf("Formato: [MENSAJE] [PARAMETROS]\n");
	printf("Ejemplo: CrearRestaurante [NOMBRE] [CANTIDAD_COCINEROS] [POSICION] [AFINIDAD_COCINEROS] [PLATOS] [PRECIO_PLATOS] [CANTIDAD_HORNOS]\n");
	printf("Ejemplo: CrearReceta [NOMBRE] [PASOS] [TIEMPO_PASOS] \n");
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
        if (!comandoLeido || string_equals_ignore_case(comandoLeido, "")) {
            continue;
        } else {
            add_history(comandoLeido);
            comandoOriginal = malloc(sizeof(char) *strlen(comandoLeido)+1);
            strcpy(comandoOriginal, comandoLeido);
            string_to_upper(comandoLeido);
            string_trim(&comandoLeido);
            parametros = string_split(comandoLeido, " ");
			mensaje = parametros[0];
            free(comandoOriginal);

            opcion = stringAProceso(mensaje); 
			switch (opcion) {
				case CREARRESTAURANTE:
					printf("Crear Restaurante: Se deberá crear una nueva carpeta restaurante, con su respectivo info.AFIP explicado anteriormente\n");
					break;
				case CREARRECETA:
					printf("Crear Receta: Se deberá crear un nuevo archivo de receta siguiendo los lineamientos de lo detallado anteriormente.\n");
					break;
				case AIUDA:
					mostrarComandosValidos();
					break;
				case BAI:
					printf("adiosss (๑♡3♡๑)!\n");
					break;
				case CLEAR:
					limpiarPantalla();
				case ERROR:
					printf("Comando no válido. Escriba 'AIUDA' para ver el formato aceptado.\n");
					break;
			}

			free(mensaje);
            free(parametros);
            if (opcion == BAI) { break; }

            free(comandoLeido);
			comandoLeido = readline("(=^.^=)~>");
        }
	}

	free(comandoLeido);
    pthread_exit(EXIT_SUCCESS);
    return 0;
}

void iterator(char* value)	{
		printf("%s\n", value);
	}
void *atenderConexiones(void *conexionNueva)
{
    pthread_data *t_data = (pthread_data*) conexionNueva;
    int info = t_data->socketThread;
    free(t_data);
	
	// 
	t_list* lista;
	while(1) {
		//int cod_op = recibir_operacion(info);
		t_paquete_v2* data = recibir_header_paquete(info);
		printf("Me llegaron los siguientes valores: %d %d\n", data->proceso_origen,data->codigo_operacion);

		switch(data->codigo_operacion) {
			case OBTENER_RESTAURANTE:
				printf("Me llego: %s\n", data->buffer->stream);
				//mandarle a resto sus cosas 
				//verificar q exista
				//obtener datos del archivo info.AFIP
				void * mock[5];
				mock[0] = "lista de cocineros"; 
				mock[1] = "posx";
				mock[2] = "posy";
				mock[3] = "recetas";
				mock[4] = "5"; //cant hornos
				int size = sizeof(char*)*5;
				t_paquete_v2* pedido = crear_paquete_v2(SINDICATO,RTA_OBTENER_RESTAURANTE,size, *mock);
				enviar_paquete_v2(pedido,info);
				break;
			case MENSAJE:
				recibir_mensaje(info);
				break;
			case PAQUETE:
				lista = recibir_paquete(info);
				printf("Me llegaron los siguientes valores:\n");
				list_iterate(lista, (void*) iterator);
				break;
			case -1:
				log_error(logger, "el cliente se desconecto. Terminando servidor");
				return EXIT_FAILURE;
			default:
				log_warning(logger, "Operacion desconocida. No quieras meter la pata");
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