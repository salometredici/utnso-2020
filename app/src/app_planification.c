#include "../include/app_planification.h"

// Utils

void logRtaConsultarPlatos(t_list *platosEnviados) {
	printf("Platos enviados:\n");
	log_info(logger, "Platos enviados:");
	for (int i = 0; i < list_size(platosEnviados); i++) {
		printf("\tPlato #%d: %s\n", i, list_get(platosEnviados, i));
		log_info(logger, "\tPlato #%d: %s", i, list_get(platosEnviados, i));
	}
}

// Retorna si un repartidor ya descansó lo suficiente como para volver a READY
bool repartidorDescansado(t_pcb *currentPcb) {
	return currentPcb->qDescansado >= currentPcb->repartidor->tiempoDescanso;
}

// Retorna si un repartidor ya completó una tanda de ejecución y debe pasar a BLOQ para descansar
bool debeDescansarRepartidor(t_pcb *currentPcb) {
	return currentPcb->qRecorrido == currentPcb->repartidor->freqDescanso;
}

// Retorna si hay espacio en qE para ejecutar otro PCB
bool puedeEjecutarAlguno() {
	return queue_size(qE) < gradoMultiprocesamiento;
}

/* Funciones */

double getDistancia(t_posicion *posRepartidor, t_posicion *posRest) {
	int x = posRepartidor->posX - posRest->posX;
	int y = posRepartidor->posY - posRest->posY;
	double distancia = sqrt(pow(x, 2) + pow(y, 2));
	return distancia;
}

t_repartidor *getRepartidorMasCercano(t_posicion *posRest) {
	double distMinima = 0;
	int indexRepartidor = 0;
	for (int i = 0; i < list_size(repartidoresDisponibles); i++) {
		t_repartidor *repActual = list_get(repartidoresDisponibles, i);
		int distARest = getDistancia(repActual->posRepartidor, posRest);
		if (distARest < distMinima) {
			distMinima = distARest;
			indexRepartidor = i;
		}
	}
	t_repartidor *repartidorMasCercano = list_get(repartidoresDisponibles, indexRepartidor);
	
	pthread_mutex_lock(&mutexListaDisponibles);
	list_remove(repartidoresDisponibles, indexRepartidor);
	pthread_mutex_unlock(&mutexListaDisponibles);
	
	pthread_mutex_lock(&mutexListaOcupados);
	list_add(repartidoresOcupados, repartidorMasCercano);	
	pthread_mutex_unlock(&mutexListaOcupados);
	
	return repartidorMasCercano;
}

/* Avisos a COMANDA y CLIENTE */

bool todosPlatosListos(t_pcb *pcb) {
	int conexionComanda = conectarseA(COMANDA);
	
	t_request *req = malloc(sizeof(t_request));
	req->idPedido = pcb->pid;
	req->nombre = pcb->restaurante;

	enviarPaquete(conexionComanda, APP, OBTENER_PEDIDO, req);
	t_header *header = recibirHeaderPaquete(conexionComanda);
	t_pedido *pedido = recibirPayloadPaquete(header, conexionComanda);
	liberarConexion(conexionComanda);

	bool noEstaCompleto(void *actual) {
		t_plato *platoActual = actual;
		return platoActual->cantidadLista < platoActual->cantidadPedida;
	};

	t_plato *platoNoListo = list_find(pedido->platos, &noEstaCompleto);

	return platoNoListo != NULL ? false : true; 
}

void informarPedidoFinalizado(t_pcb *pcb) {
	int conexionComanda = conectarseA(COMANDA);
	t_request *reqPedidoFinalizado = malloc(sizeof(t_request));
	reqPedidoFinalizado->idPedido = pcb->pid;
	reqPedidoFinalizado->nombre = pcb->restaurante;
	enviarPaquete(conexionComanda, APP, FINALIZAR_PEDIDO, reqPedidoFinalizado);
	t_header *header = recibirHeaderPaquete(conexionComanda);
	t_result *result = recibirPayloadPaquete(header, conexionComanda);
	logTResult(result);
	liberarConexion(conexionComanda);
	free(reqPedidoFinalizado);
	free(result);
	free(header);
}

void informarEntregaCliente(t_pcb *pcb) {
	t_request *reqPedidoFinalizado = malloc(sizeof(t_request));
	reqPedidoFinalizado->idPedido = pcb->pid;
	reqPedidoFinalizado->nombre = pcb->restaurante;
	enviarPaquete(pcb->socketCliente, APP, FINALIZAR_PEDIDO, reqPedidoFinalizado);
	t_header *header = recibirHeaderPaquete(conexionComanda);
	t_result *result = recibirPayloadPaquete(header, conexionComanda);
	logTResult(result);
	free(reqPedidoFinalizado);
	free(result);
	free(header);
}

/* Posición del repartidor */

bool llegoAlRestaurante(t_pcb *pcb) {
	return pcb->posRest->posX == pcb->repartidor->posRepartidor->posX && pcb->posRest->posY == pcb->repartidor->posRepartidor->posY;
}

bool llegoAlCliente(t_pcb *pcb) {
	return pcb->posCliente->posX == pcb->repartidor->posRepartidor->posX && pcb->posCliente->posY == pcb->repartidor->posRepartidor->posY;
}

t_posicion *getNextStep(t_posicion *posRepartidor, t_posicion *posObjetivo) {
	t_posicion *nextPos = malloc(sizeof(t_posicion));
	// Primero avanzamos/retrocedemos en el eje X y después en el eje Y
	if (posRepartidor->posX != posObjetivo->posX) {
		nextPos->posY = posRepartidor->posY; // No cambia en Y
		nextPos->posX = posRepartidor->posX < posObjetivo->posX ? 
						posRepartidor->posX + 1 :
						posRepartidor->posX - 1;
	} else {
		nextPos->posX = posRepartidor->posX; // No cambia en X
		nextPos->posY = posRepartidor->posY < posObjetivo->posY ?
						posRepartidor->posY + 1 :
						posRepartidor - 1;
	}
	return nextPos;
}

void actualizarPosicion(t_pcb *currentPcb, tour_code code) {
	currentPcb->qRecorrido++;
	t_posicion *nextPos = malloc(sizeof(t_posicion));
	switch (code) {
		case HACIA_CLIENTE:
			nextPos = getNextStep(currentPcb->repartidor->posRepartidor, currentPcb->posCliente);
			break;
		case HACIA_RESTAURANTE:
			nextPos = getNextStep(currentPcb->repartidor->posRepartidor, currentPcb->posRest);
			break;
	}
	currentPcb->repartidor->posRepartidor->posX = nextPos->posX;
	currentPcb->repartidor->posRepartidor->posY = nextPos->posY;
	free(nextPos);
}

void liberarRepartidor(t_repartidor *repartidor) {
	list_remove(repartidoresOcupados, repartidor->idRepartidor);
	list_add(repartidoresDisponibles, repartidor);
}

/* Planificación */

t_pcb *crearPcb(t_cliente *cliente, int idPedido) {
	t_pcb *pcb = malloc(sizeof(t_pcb));
	pcb->pid = idPedido;
	pcb->qRecorrido = 0;
	pcb->qDescansado = 0;
	pcb->qEsperando = 0;
	pcb->estado = SIN_ASIGNAR;
	pcb->alcanzoRestaurante = false;
	pcb->repartidor = malloc(sizeof(t_repartidor));
	pcb->idCliente = cliente->idCliente;
	pcb->socketCliente = cliente->socketCliente;
	 // validar si es restaurante o no
	pcb->posCliente = malloc(sizeof(t_posicion));
	pcb->posCliente->posX = cliente->posCliente->posX;
	pcb->posCliente->posY = cliente->posCliente->posY;
	pcb->restaurante = cliente->restauranteSeleccionado;
	pcb->posRest = malloc(sizeof(t_posicion));
	pcb->posRest->posX = cliente->posRestaurante->posX;
	pcb->posRest->posY = cliente->posRestaurante->posY;
	pcb->ultimaEstimacion = estimacionInicial;
	return pcb;
}

void agregarAQN(t_pcb *pcb) {
	pthread_mutex_lock(&mutexQN);
	queue_push(qN, pcb);
	log_debug(logger, "[PLANIFICATION] PCB #%d added to NEW queue", pcb->pid);
	pthread_mutex_unlock(&mutexQN);
}

void actualizarQRconQN() {
	// Mientras haya repartidores disponibles y PCBs en NEW, los asigna y añade a READY
	while (!list_is_empty(repartidoresDisponibles) && !queue_is_empty(qN)) {		
		pthread_mutex_lock(&mutexQN);
		t_pcb *nextInLine = queue_pop(qN);
		log_debug(logger, "[PLANIFICATION] PCB #%d removed from NEW queue", nextInLine->pid);
		pthread_mutex_unlock(&mutexQN);
		
		nextInLine->repartidor = getRepartidorMasCercano(nextInLine->posRest);
		log_debug(logger, "[PLANIFICATION] Repartidor #d assigned to PCB #%d", nextInLine->repartidor->idRepartidor, nextInLine->pid);
		nextInLine->estado = ESPERANDO_EJECUCION;

		pthread_mutex_lock(&mutexQR);
		queue_push(qR, nextInLine);
		log_debug(logger, "[PLANIFICATION] PCB #%d added to READY queue", nextInLine->pid);
		pthread_mutex_unlock(&mutexQR);
	}
}

void actualizarQRconQB() {
	// Reviso si puedo agregar más PCBs de QB a READY de repartidores que ya descansaron
	int sizeQB = queue_size(qB);
	for (int b = 0; b < sizeQB; b++) {
		pthread_mutex_lock(&mutexQB);
		t_pcb *current = queue_pop(qB);
		pthread_mutex_unlock(&mutexQB);
		if (current->estado == REPARTIDOR_DESCANSANDO && repartidorDescansado(current)) {
			pthread_mutex_lock(&mutexQR);
			current->estado = ESPERANDO_EJECUCION;
			queue_push(qR, current);
			pthread_mutex_unlock(&mutexQR);
		} else {
			pthread_mutex_lock(&mutexQB);
			queue_push(qB, current);
			pthread_mutex_unlock(&mutexQB);
		}
	}
}

void pasarAQB(t_pcb *pcb, t_estado estado) {
	pthread_mutex_lock(&mutexQB);
	pcb->estado = estado;
	pcb->alcanzoRestaurante = estado == ESPERANDO_PLATO ? true : false;
	pcb->qRecorrido = 0;
	queue_push(qB, pcb);
	pthread_mutex_lock(&mutexQB);
	// Logguear algo...
}

// Actualizar el estado de PCB en QB
void desbloquearPCB(int idPedido) {
	pthread_mutex_lock(&mutexQB);
	int qIndex = 0;
	int qSize = queue_size(qB);
	t_pcb *pcbADesbloquear = malloc(sizeof(t_pcb));

	t_queue *newQB = queue_create();

	for (int i = 0; i < qSize; i++) {
		t_pcb *currentPCB = queue_pop(qB);
		if (currentPCB->pid == idPedido) {
			qIndex = i;
			pcbADesbloquear = currentPCB;
			pcbADesbloquear->qDescansado = 0;
			pcbADesbloquear->estado = ESPERANDO_EJECUCION;
		} else {
			queue_push(newQB, currentPCB);
		}
	}

	qB = newQB; // Ahora qB tiene un elemento menos
	pthread_mutex_unlock(&mutexQB);
	// Agregamos el PCB desbloqueado a la QR
	pthread_mutex_lock(&mutexQR);
	queue_push(qR, pcbADesbloquear);
	pthread_mutex_unlock(&mutexQR);
}

/* FIFO */

void actualizarQEconQR_FIFO() {
	// Reviso si puedo agregar más PCBs de QR a EXEC
	while (puedeEjecutarAlguno() && !queue_is_empty(qR)) {
		pthread_mutex_lock(&mutexQR);
		t_pcb *current = queue_pop(qR);
		pthread_mutex_unlock(&mutexQR);
		pthread_mutex_lock(&mutexQE);
		current->estado = current->alcanzoRestaurante ? EN_CAMINO_A_CLIENTE : EN_CAMINO_A_RESTAURANTE;
		queue_push(qE, current);
		pthread_mutex_unlock(&mutexQE);
	}
}

void actualizarQDescanso() {
	pthread_mutex_lock(&mutexQB);
	for (int i = 0; i < queue_size(qB); i++) {
		t_pcb *current = queue_pop(qB);
		current->qDescansado++;
		queue_push(qB, current);
	}
	pthread_mutex_unlock(&mutexQB);
}

/* HRRN */

double obtenerTasaRta(t_pcb *pcb) {
	return (pcb->qEsperando + pcb->repartidor->freqDescanso)/pcb->repartidor->freqDescanso;
}

void actualizarTiemposEspera() {
	pthread_mutex_lock(&mutexQR);
	int sizeQR = queue_size(qR);
	for (int i = 0; i < sizeQR; i++) {
		t_pcb *currentPcb = queue_pop(qR);
		currentPcb->qEsperando++;
		queue_push(qR, currentPcb);
	}
	pthread_mutex_unlock(&mutexQR);
}

t_pcb *proximoAEjecutarHRRN() {
	pthread_mutex_lock(&mutexQR);
	int qIndex = 0;
	int qSize = queue_size(qR);
	int tasaRtaMax = 0;
	t_pcb *nextPCBtoExec = malloc(sizeof(t_pcb));

	t_queue *newQR = queue_create();

	for (int i = 0; i < qSize; i++) {
		t_pcb *currentPCB = queue_pop(qR);
		int tasaRtaActual = obtenerTasaRta(currentPCB);
		if (tasaRtaActual >= tasaRtaMax) {
			qIndex = i;
			if (nextPCBtoExec != NULL) queue_push(newQR, nextPCBtoExec);
			nextPCBtoExec = currentPCB;
			tasaRtaMax = tasaRtaActual;
		} else {
			queue_push(newQR, currentPCB);
		}
	}

	qR = newQR; // Ahora qR tiene un elemento menos

	pthread_mutex_unlock(&mutexQR);
}

void actualizarQEconQR_HRRN() {
	// Reviso si puedo agregar más PCBs de QR a EXEC
	while (puedeEjecutarAlguno() && !queue_is_empty(qR)) {
		t_pcb *current = proximoAEjecutarHRRN();
		pthread_mutex_lock(&mutexQE);
		current->estado = current->alcanzoRestaurante ? EN_CAMINO_A_CLIENTE : EN_CAMINO_A_RESTAURANTE;
		queue_push(qE, current);
		pthread_mutex_unlock(&mutexQE);
	}
}

/* SJF */

void actualizarEstimacion(t_pcb *pcb) {
	double nuevaEstimacion = pcb->ultimaEstimacion*alpha + pcb->qRecorrido*(1 - alpha);
	pcb->ultimaEstimacion = nuevaEstimacion;
}

t_pcb *proximoAEjecutarSJF() {
	pthread_mutex_lock(&mutexQR);
	
	t_pcb *primerPCB = queue_peek(qR);
	double estimacionMin = primerPCB->ultimaEstimacion;

	t_pcb *nextPCBtoExec = malloc(sizeof(t_pcb));
	t_queue *newQR = queue_create();

	int qSize = queue_size(qR);

	for (int i = 0; i < qSize; i++) {
		t_pcb *currentPCB = queue_pop(qR);
		if (currentPCB->ultimaEstimacion < estimacionMin) {
			if (nextPCBtoExec != NULL) queue_push(newQR, nextPCBtoExec); // Verifico si nextPCBtoExec tiene un PCB
			nextPCBtoExec = currentPCB;
			estimacionMin = currentPCB->ultimaEstimacion;
		} else {
			queue_push(newQR, currentPCB);
		}
	}

	qR = newQR;

	pthread_mutex_unlock(&mutexQR);
	return nextPCBtoExec;
}

void actualizarQEconQR_SJF() {
	// Reviso si puedo agregar más PCBs de QR a EXEC
	while (puedeEjecutarAlguno() && !queue_is_empty(qR)) {
		t_pcb *current = proximoAEjecutarSJF();
		pthread_mutex_lock(&mutexQE);
		current->estado = current->alcanzoRestaurante ? EN_CAMINO_A_CLIENTE : EN_CAMINO_A_RESTAURANTE;
		queue_push(qE, current);
		pthread_mutex_unlock(&mutexQE);
	}
}


/* Ejecución */

void ejecutarCiclos() // Para FIFO, HRRN y SJF
{
	pthread_mutex_lock(&mutexQE);
	t_pcb *currentPcb = queue_pop(qE);

	while (currentPcb != NULL) {
		// 1. Si debe descansar, pasa a BLOQUEADO
		if (debeDescansarRepartidor(currentPcb)) {
			pasarAQB(currentPcb, REPARTIDOR_DESCANSANDO);
			if (algoritmoSeleccionado == SJF) actualizarEstimacion(currentPcb);
			// currentPcb = queue_pop(qE); // Continúa el ciclo con el siguiente PCB
		} else if (currentPcb->estado == EN_CAMINO_A_RESTAURANTE && llegoAlRestaurante(currentPcb)) {
			// 2. Si llegó al restaurante y tiene todos los platos listos, sigue hacia el cliente, sino, a BLOQUEADO
			if (todosPlatosListos(currentPcb)) {
				currentPcb->estado = EN_CAMINO_A_CLIENTE;
				currentPcb->alcanzoRestaurante = true;
			} else {
				pasarAQB(currentPcb, ESPERANDO_PLATO);
				if (algoritmoSeleccionado == SJF) actualizarEstimacion(currentPcb);
				// currentPcb = queue_pop(qE); // Continúa el ciclo con el siguiente PCB
			}
		} else if (currentPcb->estado == EN_CAMINO_A_CLIENTE && llegoAlCliente(currentPcb)) {
			// 3. Si llegó al cliente, se da por concluido el pedido
			informarPedidoFinalizado(currentPcb);
			informarEntregaCliente(currentPcb);
			if (algoritmoSeleccionado == SJF) actualizarEstimacion(currentPcb);
			queue_push(currentPcb, qF);
			// currentPcb = queue_pop(qE); // Continúa el ciclo con el siguiente PCB
		} else {
			// 4. Si sigue ejecutando, debe actualizar su posición de acuerdo a donde esté viajando
			actualizarPosicion(currentPcb, currentPcb->alcanzoRestaurante ? HACIA_CLIENTE : HACIA_RESTAURANTE);
		}
		currentPcb = queue_pop(qE); // Continúa el ciclo con el siguiente PCB
		if (algoritmoSeleccionado == HRRN) { actualizarTiemposEspera(); }
		actualizarQDescanso();
        sleep(tiempoRetardoCpu);
	}

	pthread_mutex_unlock(&mutexQE);
}