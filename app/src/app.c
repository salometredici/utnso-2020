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

int main(int argc, char* argv[])
{
	inicializarProceso(APP);
	initApp();

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