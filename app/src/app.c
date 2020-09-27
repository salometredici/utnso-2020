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
        	printf("El cliente %d se desconectó. Terminando su hilo...\n", info);
        	pthread_exit(EXIT_SUCCESS);
        	return EXIT_FAILURE;
    	}

    	switch (data->codigoOperacion) {
        	case CONSULTAR_RESTAURANTES: // Sin parámetros
				printf("ME LLEGO CONSULTAR_RESTAURANTES \n");
            	restaurantes = list_create();
            	list_add(restaurantes,"Mc Donalds");
            	list_add(restaurantes,"Burguer King");
            	list_add(restaurantes,"Mostaza");
            	list_add(restaurantes,"Wendys");

            	enviarPaquete(info, APP, RTA_CONSULTAR_RESTAURANTES, restaurantes);
        		break;
			case SELECCIONAR_RESTAURANTE:
				printf("ME LLEGO SELECCIONAR RESTAURANTE \n");
				break;
			case CONSULTAR_PLATOS:
				printf("ME LLEGO CONSULTAR PLATOS\n");
				break;	
			case CREAR_PEDIDO:
				printf("ME LLEGO CREAR PEDIDO\n");
				break;	
			case ANIADIR_PLATO:
				printf("ME LLEGO ANIADIR PLATO\n");
				break;	
			case PLATO_LISTO:
				printf("ME LLEGO PLATO LISTO\n");
				break;	
			case CONFIRMAR_PEDIDO:
				printf("ME LLEGO CONFIRMAR PEDIDO\n");
				break;
			case CONSULTAR_PEDIDO:
				printf("ME LLEVO CONSULTAR PEDIDO\n");
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
void cargarConfiguracionApp() {
	appConfig= malloc(sizeof(appConfig_t));
	
	appConfig->gradoMultiprocesamiento = config_get_int_value(config,"GRADO_DE_MULTIPROCESAMIENTO" );
	appConfig->algoritmoPlanificacion = config_get_string_value(config, "ALGORITMO_DE_PLANIFICACION");
	appConfig->alpha = config_get_double_value(config,"ALPHA");
	appConfig->estimacionInicial = config_get_int_value(config,"ESTIMACION_INICIAL");
	appConfig->posicionRepartidores = config_get_array_value(config,"REPARTIDORES");
	appConfig->frecuenciaDescanso = config_get_array_value(config,"FRECUENCIA_DE_DESCANSO");
	appConfig->tiempoDescanso = config_get_array_value(config,"TIEMPO_DE_DESCANSO");
	appConfig->platosDefault =config_get_array_value(config,"PLATOS_DEFAULT");
	appConfig->posicionRestDefaultX = config_get_int_value(config,"POSICION_REST_DEFAULT_X");
	appConfig->posicionRestDefaultY = config_get_int_value(config,"POSICION_REST_DEFAULT_Y");	
	/*
	printf("\n\n grado de multiprogramacion: %d",appConfig->gradoMultiprocesamiento);
	printf("\n algoritmo de planificacion: %s",appConfig->algoritmoPlanificacion);
	printf("\n alpha: %.2f", appConfig->alpha);
	printf("\n estimacion inicial: %d", appConfig->estimacionInicial);
	printf("\n platos default: %s", appConfig->platosDefault[0]);
	printf("\n posicion restaurante default X: %d", appConfig->posicionRestDefaultX);
	printf("\n posicion restaurante default Y: %d\n", appConfig->posicionRestDefaultY);

	int i;
	for (i = 0; appConfig->posicionRepartidores[i] != NULL; i++)
	{
		printf("\n posicion del repartidor %d: %s",i , appConfig->posicionRepartidores[i]);
    }
	printf("\n cantidad de posiciones de repartidores es: %d\n", i);
	
	for (i = 0; appConfig->frecuenciaDescando[i] != NULL; i++)
	{
		printf("\n frecuencia de descanso %d: %s",i, appConfig->frecuenciaDescando[i]);
    }

	for (i = 0; appConfig->tiempoDescanso[i] != NULL; i++)
	{
     	printf("\n tiempo de descanso %d: %s",i, appConfig->tiempoDescanso[i]);
    }
	printf("\n\n");
	*/
	free(appConfig);
}
void inicializarEstructurasYListas() {
	
	listaPedidos = list_create(); // aca se van a ir colocando los PCB
	listaRepartidores = list_create();
	NUEVOS = list_create();
    LISTOS = list_create();
	EJECUTANDO = list_create();
    BLOQUEADOS = list_create();
    FINALIZADOS = list_create();

	cargarPCByListas();

}
void cargarPCByListas() {
// Aca se carga los repartidores a la lista de repartidores
// y los PCB que referencian a cada pedido
}


void planificar() {

	if(strcmp(appConfig->algoritmoPlanificacion,"FIFO") == 0) 
	{
    	planificarFIFO();
    }
	else if(strcmp(appConfig->algoritmoPlanificacion,"HRRN") == 0)
    {
    	planificarHRRN();
    }
    else if(strcmp(appConfig->algoritmoPlanificacion,"SJF") == 0)
    {
    	planificarSJF();
    }
   
}

void planificarFIFO() {
	printf("\n SE PLANIFICARA POR FIFO\n");
}
void planificarHRRN() {
	printf("\n SE PLANIFICARA POR HRNN\n");
}
void planificarSJF() {
	printf("\n SE PLANIFICARA POR SJF\n");
}

int main(int argc, char* argv[])
{
	inicializarProceso(APP); // crea el log y el config de APP
	
	cargarConfiguracionApp();
	
	inicializarEstructurasYListas();

	planificar(); // seguramente habra que tener un hilo para esto. Despues ver donde va esta funcion

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