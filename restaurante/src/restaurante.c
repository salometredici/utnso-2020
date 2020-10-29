#include "../include/restaurante.h"

// Limpiar clientes desconectados
void revisarConectados(t_list *lista) {
    for (int i = 0; i < list_size(lista); i++) {
        t_cliente *clienteActual = list_get(lista, i);
        if (recv(clienteActual->socketCliente, NULL, 1, MSG_PEEK | MSG_DONTWAIT) == 0) {
            list_remove(lista, i);
            log_debug(logger, "El cliente [#%d - %s] se desconectó", i, clienteActual->idCliente);
        }
    }
}

// Preguntar al cliente su nombre y posición, asociandolo a un socket
t_cliente *getCliente(int socketCliente) {
	t_header *header = recibirHeaderPaquete(socketCliente);
	t_cliente *cliente = recibirPayloadPaquete(header, socketCliente);
	cliente->socketCliente = socketCliente;
	free(header);
}
// Revisar si el cliente ya existe en la lista de conectados y limpiar los desconectados
void actualizarClientesConectados(t_cliente *cliente) {
    revisarConectados(clientesConectados);

    bool estaDuplicado(void *actual) {
        t_cliente *clienteActual = actual;
        return string_equals_ignore_case(cliente->idCliente, clienteActual->idCliente);
    };

	t_cliente *cliDuplicado = list_find(clientesConectados, &estaDuplicado);
	if (cliDuplicado != NULL) { list_add(clientesConectados, cliente); }
}

void *atenderConexiones(void *conexionNueva)
{
    pthread_data *t_data = (pthread_data*) conexionNueva;
    int socketCliente = t_data->socketThread;
    free(t_data);

	int socketSindicato = ERROR;
	t_cliente *cliente = getCliente(socketCliente);
	actualizarClientesConectados(cliente);

	while (1) {

		t_header *header = recibirHeaderPaquete(socketCliente);

		if (header->procesoOrigen == ERROR || header->codigoOperacion == ERROR) {
			logClientDisconnection(socketCliente);
			liberarConexion(socket);
    		pthread_exit(EXIT_SUCCESS);
			return EXIT_FAILURE;
		}

		socketSindicato = conectarseA(SINDICATO);

		switch (header->codigoOperacion) {
			case OBTENER_PROCESO:;
				enviarPaquete(socketCliente, RESTAURANTE, RTA_OBTENER_PROCESO, RESTAURANTE);
				break;
			case CONSULTAR_PLATOS:;
				char *restConsulta = recibirPayloadPaquete(header, socketCliente);
				logConsultaPlatos(restConsulta);
				t_list *platosRest = list_create();
				list_add(platosRest, "Milanesas");
				list_add(platosRest, "Lasagna");
				list_add(platosRest, "Asado");
				enviarPaquete(socketCliente, RESTAURANTE, RTA_CONSULTAR_PLATOS, platosRest);
				free(platosRest);
				break;
			case CREAR_PEDIDO:;
				int newIdPedido = 77;
				enviarPaquete(socketCliente, RESTAURANTE, RTA_CREAR_PEDIDO, newIdPedido);
				break;
			case ANIADIR_PLATO:;
				t_request *reqAniadir = recibirPayloadPaquete(header, socketCliente);
				logRequest(reqAniadir, header->codigoOperacion);
				free(reqAniadir);

				t_result *resAP = malloc(sizeof(t_result));
				resAP->hasError = false;
				resAP->msg = "[ANIADIR_PLATO] OK";
				enviarPaquete(socketCliente, RESTAURANTE, RTA_ANIADIR_PLATO, resAP);
				free(resAP);
				break;
			case CONFIRMAR_PEDIDO:;
				t_request *reqConf = recibirPayloadPaquete(header, socketCliente);
				logRequest(reqConf, header->codigoOperacion);
				// free(reqConf);

				// Sólo recibe el ID del pedido
				// 1. Obtener el Pedido desde Sindicato

				reqConf->nombre = nombreRestaurante;
				enviarPaquete(conexionSindicato, RESTAURANTE, OBTENER_PEDIDO, reqConf);
				t_header *hRConf = recibirHeaderPaquete(conexionSindicato);
				t_pedido *pedidoConf = recibirPayloadPaquete(header, conexionSindicato);
				mostrarListaPlatos(pedidoConf->platos);
				free(hRConf);

				// 2. Generar PCB de cada plato y dejarlo en el ciclo de planificación
				// Obtener receta de Sindicato para saber trazabilidad al momento de ejecución
				// El número de pedido se deberá guardar dentro del PCB
				
				// por cada plato pedir la receta y generar pcb
				for (int i = 0;i<list_size(pedidoConf->platos);i++){
					//conseguir receta de sindicato //MENSAJE OBTENER RECETA

					//crear pcb con nombre, receta, id pedido, etc
					//y lo metes en cola de new que no es necesaria lo metemos directo en ready
				}



				// 3. Informar a quien lo invocó que su pedido fue confirmado

				t_result *resCP = malloc(sizeof(t_result));
				resCP->hasError = false;
				resCP->msg = "[CONFIRMAR_PEDIDO] OK";
				enviarPaquete(socketCliente, SINDICATO, RTA_CONFIRMAR_PEDIDO, resCP);
				free(resCP);
				break;
			case CONSULTAR_PEDIDO: // TODO: El model del TP incluye un restaurante, que falta agregar a nuestro t_pedido
				break;
			default:
				printf("Operación desconocida. Llegó el código: %d. No quieras meter la pata!!!(｀Д´*)\n", header->codigoOperacion);
				break;
		}
		free(header);
	}

	liberarConexion(socketSindicato); // si no es error, lo liberamo'
    pthread_exit(EXIT_SUCCESS);
    return 0;
}



// PCB

t_proceso *crearPcb(t_cliente *cliente, int idPedido, t_receta *receta) {
    t_proceso *pcb = malloc(sizeof(t_proceso));
    pcb->pid = idPedido;
    pcb->idCliente = cliente->idCliente;
    pcb->estado = ESPERANDO_EJECUCION;
	pcb->plato= receta->plato;
	pcb->pasosReceta=receta->instrucciones;
	t_instrucciones_receta *pasoActual = list_get(pcb->pasosReceta,0);
	pcb->instruccionActual = pasoActual->paso;
    pcb->qInstruccionActual = pasoActual->qPaso;
    pcb->posInstruccionActual = 0;
    pcb->qEjecutada = 0;

    return pcb;
}

void agregarQueue(int opcion, t_proceso *pcb){
	bool correspondeAfinidad(void *element){
		t_queue_obj *actual = element;
		return string_equals_ignore_case(actual->afinidad, pcb->plato);
	};
	t_queue_obj *encontrado = list_find(queuesCocineros, &correspondeAfinidad);
	if(encontrado != NULL){
		pthread_mutex_lock(opcion ? &encontrado->mutexQR : &encontrado->mutexQR);
			queue_push(opcion ? encontrado->qR : encontrado->qE, pcb); 
		pthread_mutex_unlock(opcion ? &encontrado->mutexQR : &encontrado->mutexQR);
	} else {
		bool esGeneral(void *element){
			t_queue_obj *actual = element;
			return string_equals_ignore_case(actual->afinidad, "General");
		};
		t_queue_obj *general = list_find(queuesCocineros, &esGeneral);
		pthread_mutex_lock(opcion ? &general->mutexQR : &general->mutexQR);
			queue_push(opcion ? general->qR : general->qE, pcb);
		pthread_mutex_unlock(opcion ? &general->mutexQR : &general->mutexQR);

	} 
}

void agregarQR(t_proceso *pcb){
	agregarQueue(1, pcb);
}

/* Planificación FIFO */

// Actualiza los que hayan cumplido su tiempoen block a new
void actualizarQB() {
	//1. revisar queue bloqueados
	if (!queue_is_empty(qB)) {
		for(int i = 0; i < queue_size(qB);i++){
			pthread_mutex_lock(&mutexQB);
			t_proceso *sigARevisar = queue_pop(qB); // Lo sacamos de la cola de Bloqueado
			pthread_mutex_unlock(&mutexQB);
			if( sigARevisar->estado == REPOSANDO && sigARevisar->qInstruccionActual == 0 ){
				sigARevisar->estado = ESPERANDO_EJECUCION;
				sigARevisar->posInstruccionActual++;

				t_instrucciones_receta *pasoActual = list_get(sigARevisar->pasosReceta,sigARevisar->posInstruccionActual);
				sigARevisar->instruccionActual = pasoActual->paso;
				sigARevisar->qInstruccionActual = pasoActual->qPaso;
				sigARevisar->qEjecutada = 0;
				
				agregarQueue(1, sigARevisar); //agrego a ready slds
			}
			else{
				pthread_mutex_lock(&mutexQB);
				queue_push(qB,sigARevisar);
				pthread_mutex_unlock(&mutexQB);
			}
		}
    }	
}

void actualizarQE() {
	// 1. Revisar si algún plato tiene que reposar / hornear / pasar a ready
	void _evaluarCocineros(void* element) {
        t_queue_obj *queue = element;
		t_queue *qE = queue->qE;
		pthread_mutex_t mutexQE = queue->mutexQE;

		// ver si lo cambia sobre la referencia cuando lo pruebo 
		//o tengo que usar directo el element
		if (!queue_is_empty(qE)) {
			for(int i = 0; i < queue_size(qE);i++){
				pthread_mutex_lock(&mutexQE);
				t_proceso *sigARevisar = queue_pop(qE); // Lo sacamos de la cola de Bloqueado
				pthread_mutex_unlock(&mutexQE);
				if( sigARevisar->qInstruccionActual == 0 ){
					sigARevisar->posInstruccionActual++;
					t_instrucciones_receta *pasoActual = list_get(sigARevisar->pasosReceta,sigARevisar->posInstruccionActual);
					
					if(string_equals_ignore_case(pasoActual->paso, "REPOSAR")) {
						sigARevisar->estado = REPOSANDO;
						t_instrucciones_receta *pasoActual = list_get(sigARevisar->pasosReceta,sigARevisar->posInstruccionActual);
						sigARevisar->instruccionActual = pasoActual->paso;
						sigARevisar->qInstruccionActual = pasoActual->qPaso;
						sigARevisar->qEjecutada = 0;
						
						pthread_mutex_lock(&mutexQB);
						queue_push(qB,sigARevisar);
						pthread_mutex_unlock(&mutexQB);
					} else if(string_equals_ignore_case(pasoActual->paso, "HORNEAR")) {
						sigARevisar->estado = ESPERANDO_HORNO;
						t_instrucciones_receta *pasoActual = list_get(sigARevisar->pasosReceta,sigARevisar->posInstruccionActual);
						sigARevisar->instruccionActual = pasoActual->paso;
						sigARevisar->qInstruccionActual = pasoActual->qPaso;
						sigARevisar->qEjecutada = 0;
						
						pthread_mutex_lock(&mutexEsperaIO);
						queue_push(esperandoIO,sigARevisar);
						pthread_mutex_unlock(&mutexEsperaIO);
					} else {
						sigARevisar->estado = EJECUTANDO;
						t_instrucciones_receta *pasoActual = list_get(sigARevisar->pasosReceta,sigARevisar->posInstruccionActual);
						sigARevisar->instruccionActual = pasoActual->paso;
						sigARevisar->qInstruccionActual = pasoActual->qPaso;
						sigARevisar->qEjecutada = 0;
						agregarQueue(0, sigARevisar);// vas a ejecutando again
					}
				}
				else {
					pthread_mutex_lock(&mutexQE);
					queue_push(qE,sigARevisar);
					pthread_mutex_unlock(&mutexQE);
				}
			}
    	}
        
        free(queue);
    };

    list_iterate(queuesCocineros, _evaluarCocineros);

	// 2. Reviso si puedo agregar más PCBs de QB a EXEC de repartidores que ya descansaron (OJO con los repartidores bloq que esperan avisos!)
	
	// 3. Reviso si puedo agregar más PCBs de QR a EXEC
}

void actualizarQRaQE(){
	void _evaluarCocineros(void* element){
		// rompe la compliacion si no se asigna la variable element
		t_queue_obj *queue = element;
		t_queue *qR = queue->qR;
		pthread_mutex_t mutexQR = queue->mutexQR;

		if (!queue_is_empty(qR) && queue_size(queue->qE) < queue->instanciasTotales)  {
			int disponibles = queue->instanciasTotales-queue_size(queue->qE);
			for(int i = 0; i < disponibles; i++) {
				pthread_mutex_lock(&mutexQR);
				t_proceso *sigARevisar = queue_pop(qR); // Lo sacamos de la cola de ready
				pthread_mutex_unlock(&mutexQR);
				
				pthread_mutex_lock(&queue->mutexQE);
				queue_push(queue->qE,sigARevisar);
				pthread_mutex_unlock(&queue->mutexQE);
			}
    	}
        
        free(queue);
    };

    list_iterate(queuesCocineros, _evaluarCocineros);
}
// bool debeBloquearse(t_proceso *pcb){
// 	t_instrucciones_receta *pasoSiguiente = list_get(sigARevisar->pasosReceta,sigARevisar->posInstruccionActual+1);
	
// 		actualizarQE();
// 	if(pcb->qInstruccionActual == 0 && (string_equals_ignore_case(pasoActual->paso, "REPOSAR") || string_equals_ignore_case(pasoActual->paso, "HORNEAR"))){
// 		actualizarQE();}
// 		else
// }

// t proceso *evaluarPCB(t_proceso *pcb){
// 	t_instrucciones_receta *pasoSiguiente = list_get(sigARevisar->pasosReceta,sigARevisar->posInstruccionActual+1);
// 	// si llego q instrucciones igual a cero hay que avanzar
// 	if(pcb->qInstruccionActual == 0 && (string_equals_ignore_case(pasoActual->paso, "REPOSAR") || string_equals_ignore_case(pasoActual->paso, "HORNEAR"))){
// 	actualizarQE(pcb);
// 	}
// 	else {
// 		return pcb
// 	}

void ejecutarCicloIO(){
	if (!queue_is_empty(ejecutandoIO)) {
		//nos fijamos si alguno termino su rafaga
		for(int i = 0; i < queue_size(ejecutandoIO);i++){
			pthread_mutex_lock(&mutexEjecutaIO);
			t_proceso *sigARevisar = queue_pop(ejecutandoIO); // Lo sacamos de la cola de Bloqueado
			pthread_mutex_unlock(&mutexEjecutaIO);
			if( sigARevisar->qInstruccionActual == 0 ){
				agregarQueue(1, sigARevisar); //agrego a ready slds
			} else {
				sigARevisar->qEjecutada++;
				sigARevisar->qInstruccionActual--;
			}
		}
	}
	if(queue_size(ejecutandoIO) < instanciasTotalesIO){
		if(!queue_is_empty(esperandoIO)){
			// Lo pasamos de espera a ejecutando
			pthread_mutex_lock(&mutexEsperaIO);
				t_proceso *proceso = queue_pop(esperandoIO); 
			pthread_mutex_unlock(&mutexEsperaIO);
				
			pthread_mutex_lock(&mutexEjecutaIO);
				queue_push(ejecutandoIO,proceso);
			pthread_mutex_unlock(&mutexEjecutaIO);
		}
	}
}

void ejecutarCiclosFIFO() {
	void _ejecucionCocineros(void *element){
		t_queue_obj *queue = element;
		pthread_mutex_lock(&queue->mutexQE);
		t_proceso *currentPcb = queue_pop(queue->qE);

		while (currentPcb != NULL) {
			currentPcb->qEjecutada++;
			currentPcb->qInstruccionActual--;
			queue_push(queue->qE, currentPcb);

			//ver que se puede mejorar
			if(!currentPcb->qInstruccionActual){
				ejecutarCicloIO();
				break;// o continue
			}
			
			// revisar si hay lugar para meter otro 
			ejecutarCicloIO();
			//sleep(tiempoRetardoCpu); // lei que más adelante van a agregar esta variable
			currentPcb = queue_pop(queue->qE);
		}
		pthread_mutex_unlock(&queue->mutexQE);
	}

	list_iterate(queuesCocineros, _ejecucionCocineros);
}


void *planificar(void *args) { // FIFO por ahora
	// TODO: Semáforo
	while (1) {
		switch (algoritmoSeleccionado) {
			case FIFO:
				// Largo plazo
				actualizarQB();
				actualizarQE();
				actualizarQRaQE();
				
				// Corto plazo
				ejecutarCiclosFIFO();
				break;
			case RR:
				// Largo plazo			

				// Corto plazo
				break;
			default:
				break;
		}
	}
}

int main(int argc, char *argv[])
{
	inicializarProceso(RESTAURANTE);
	conexion = iniciarServidor();//conectarseA(RESTAURANTE);
	initRestaurante();
	pthread_create(&threadPlanificacion, NULL, (void*)planificar, NULL);

	// Inicio del bucle que va a generar los diferentes hilos de conexión
	int fd;
	while (1) {
		fd = aceptarCliente(conexion);
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
	
	pthread_kill(threadPlanificacion, SIGTERM);
	liberarConexion(conexion);
    finalizarProceso();
    return EXIT_SUCCESS;
}