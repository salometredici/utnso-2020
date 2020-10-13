     #include "../include/app.h"

void *atenderConexiones(void *conexionNueva)
{
    pthread_data *t_data = (pthread_data*) conexionNueva;
    int socketCliente = t_data->socketThread;
    free(t_data);

	while (1) {

    	t_header *header = recibirHeaderPaquete(socketCliente);

		if (header->procesoOrigen == ERROR || header->codigoOperacion == ERROR) {
			logClientDisconnection(socketCliente);
			liberarConexion(socket);
    		pthread_exit(EXIT_SUCCESS);
			return EXIT_FAILURE;
		}	

    	switch (header->codigoOperacion) {
			case OBTENER_PROCESO:;
				enviarPaquete(socketCliente, APP, RTA_OBTENER_PROCESO, APP);
				break;
        	case CONSULTAR_RESTAURANTES:;
            	restaurantes = list_create();
            	list_add(restaurantes,"McDonalds");
            	list_add(restaurantes,"Burguer King");
            	list_add(restaurantes,"Mostaza");
            	list_add(restaurantes,"Wendys");
            	enviarPaquete(socketCliente, APP, RTA_CONSULTAR_RESTAURANTES, restaurantes);
        		break;
			case SELECCIONAR_RESTAURANTE:; // TODO: Recibe cliente (¿un id o un t_cliente?) y restaurante, retorna Ok/fail
				break;
			case CONSULTAR_PLATOS:;
				char *restConsulta = recibirPayloadPaquete(header, socketCliente); free(restConsulta);
				t_list *platosRest = list_create();
				list_add(platosRest, "Milanesas");
				list_add(platosRest, "Lasagna");
				list_add(platosRest, "Asado");
				enviarPaquete(socketCliente, APP, RTA_CONSULTAR_PLATOS, platosRest);
				free(platosRest);
				break;	
			case CREAR_PEDIDO:;
				int newIdPedido = 88;
				enviarPaquete(socketCliente, APP, RTA_CREAR_PEDIDO, newIdPedido);
				break;	
			case ANIADIR_PLATO:;
				t_request *reqAniadir = recibirPayloadPaquete(header, socketCliente);
				logRequest(reqAniadir, header->codigoOperacion);
				free(reqAniadir);
				t_result *rAP = malloc(sizeof(t_result));
				rAP->msg = "[ANIADIR_PLATO] Ok\n";
				rAP->hasError = false;
				enviarPaquete(socketCliente, APP, RTA_ANIADIR_PLATO, rAP);
				free(rAP);
				break;	
			case PLATO_LISTO:; // TODO: struct que recibe restaurante, idPedido y plato
				break;	
			case CONFIRMAR_PEDIDO:;
				t_request *reqConf = recibirPayloadPaquete(header, socketCliente);
				logRequest(reqConf, header->codigoOperacion);
				free(reqConf);
				t_result *rCP = malloc(sizeof(t_result));
				rCP->msg = "[CONFIRMAR_PEDIDO] Ok\n";
				rCP->hasError = false;
				enviarPaquete(socketCliente, APP, RTA_CONFIRMAR_PEDIDO, rCP);
				free(rCP);
				break;
			case CONSULTAR_PEDIDO:; // TODO: El model del TP incluye un restaurante, que falta agregar a nuestro t_pedido
				break;			
        	default:
            	printf("Operación desconocida. Llegó el código: %d. No quieras meter la pata!!!(｀Д´*)\n", header->codigoOperacion);
            	break;
 	   	}
		free(header);
	}	
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
	printf("SE PLANIFICARA POR FIFO\n");
}
void planificarHRRN() {
	printf("SE PLANIFICARA POR HRNN\n");
}
void planificarSJF() {
	printf("SE PLANIFICARA POR SJF\n");
}

int main(int argc, char* argv[])
{
	inicializarProceso(APP);
	
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
			logNewClientConnection(fd);
		} else {
			pthread_kill(threadConexiones, SIGTERM);
		}
	}
	
	liberarConexion(socketServidor);
    finalizarProceso();
    return EXIT_SUCCESS;
}