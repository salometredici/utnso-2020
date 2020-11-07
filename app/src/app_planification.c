#include "../include/app_planification.h"

// Retorna si un repartidor ya descansó lo suficiente como para volver a READY
bool repartidorDescansado(t_pcb *currentPcb) {
	return currentPcb->qDescansado >= currentPcb->repartidor->tiempoDescanso;
}

// Retorna si un repartidor ya completó una tanda de ejecución y debe pasar a BLOCKED para descansar
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
	log_debug(logger, "[PLANIFICATION] Revisando si todos los platos del PCB #%d están listos...", pcb->pid);
	int conexionComanda = conectarseA(COMANDA);
	t_request *req = getTRequest(pcb->pid, pcb->restaurante);
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
	log_debug(logger, "[PLANIFICATION] Informando la finalización del PCB #%d a COMANDA...", pcb->pid);
	int conexionComanda = conectarseA(COMANDA);
	t_request *request = getTRequest(pcb->pid, pcb->restaurante);
	enviarPaquete(conexionComanda, APP, FINALIZAR_PEDIDO, request);
	t_header *header = recibirHeaderPaquete(conexionComanda);
	t_result *result = recibirPayloadPaquete(header, conexionComanda);
	logTResult(result);
	liberarConexion(conexionComanda);
	free(request);
	free(result);
	free(header);
}

void informarEntregaCliente(t_pcb *pcb) {
	log_debug(logger, "[PLANIFICATION] Informando la finalización del PCB #%d al cliente %s...", pcb->pid, pcb->idCliente);
	t_request *request = getTRequest(pcb->pid, pcb->restaurante);
	enviarPaquete(pcb->socketCliente, APP, FINALIZAR_PEDIDO, request);
	t_header *header = recibirHeaderPaquete(conexionComanda);
	t_result *result = recibirPayloadPaquete(header, conexionComanda);
	logTResult(result);
	free(request);
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
	log_debug(logger, "[PLANIFICATION] El repartidor del PCB #%d se encuentra en camino al %s", currentPcb->pid, code == HACIA_CLIENTE ? "cliente" : "restaurante");
	switch (code) {
		case HACIA_CLIENTE:
			nextPos = getNextStep(currentPcb->repartidor->posRepartidor, currentPcb->posCliente);
			break;
		case HACIA_RESTAURANTE:
			nextPos = getNextStep(currentPcb->repartidor->posRepartidor, currentPcb->posRest);
			break;
	}
	log_debug(logger, "[PLANIFICATION] El repartidor del PCB #%d se trasladó de [%d,%d] a [%d,%d]", currentPcb->pid,
																									currentPcb->repartidor->posRepartidor->posX,
																									currentPcb->repartidor->posRepartidor->posY,
																									nextPos->posX,
																									nextPos->posY);
	currentPcb->repartidor->posRepartidor->posX = nextPos->posX;
	currentPcb->repartidor->posRepartidor->posY = nextPos->posY;
	free(nextPos);
}

void liberarRepartidor(t_repartidor *repartidor) {
	pthread_mutex_lock(&mutexListaOcupados);
	list_remove(repartidoresOcupados, repartidor->idRepartidor);
	pthread_mutex_unlock(&mutexListaOcupados);
	pthread_mutex_lock(&mutexListaDisponibles);
	list_add(repartidoresDisponibles, repartidor);
	log_debug(logger, "[PLANIFICATION] Repartidor #d is now available. Total available now: %d", repartidor->idRepartidor, list_size(repartidoresDisponibles));
	pthread_mutex_unlock(&mutexListaDisponibles);
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
	pcb->posCliente = malloc(sizeof(t_posicion));
	pcb->posCliente->posX = cliente->posCliente->posX;
	pcb->posCliente->posY = cliente->posCliente->posY;
	pcb->restaurante = cliente->restSeleccionado;
	pcb->posRest = malloc(sizeof(t_posicion));
	pcb->posRest->posX = cliente->posRest->posX;
	pcb->posRest->posY = cliente->posRest->posY;
	pcb->ultimaEstimacion = estimacionInicial;
	return pcb;
}

void agregarAQN(t_pcb *pcb) {
	log_debug(logger, "[PLANIFICATION] Agregando el PCB %d a NEW por haber sido confirmado...", pcb->pid);
	pthread_mutex_lock(&mutexQN);
	queue_push(qN, pcb);
	log_debug(logger, "[PLANIFICATION] PCB #%d added to NEW queue", pcb->pid);
	pthread_mutex_unlock(&mutexQN);
}

void actualizarQRconQN() {
	// Mientras haya repartidores disponibles y PCBs en NEW, los asigna y añade a READY
	while (!list_is_empty(repartidoresDisponibles) && !queue_is_empty(qN)) {
		log_debug(logger, "[PLANIFICATION] Asignando repartidores para pasar PCBs a READY...");
		log_debug(logger, "[PLANIFICATION] Repartidores disponibles: %d, Espacio en EXEC: %d", list_size(repartidoresDisponibles), queue_size(qE) - gradoMultiprocesamiento);

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
	// Revisamos si algún PCB de BLOCKED ya descansó y puede pasar a READY
	int sizeQB = queue_size(qB);
	for (int b = 0; b < sizeQB; b++) {
		pthread_mutex_lock(&mutexQB);
		t_pcb *current = queue_pop(qB);
		pthread_mutex_unlock(&mutexQB);
		if (current->estado == REPARTIDOR_DESCANSANDO && repartidorDescansado(current)) {
			log_debug(logger, "[PLANIFICATION] PCB #%d finished its timeout. Cycles resting: %d", current->pid, current->qDescansado);
			current->estado = ESPERANDO_EJECUCION;
			current->qDescansado = 0;
			pthread_mutex_lock(&mutexQR);
			queue_push(qR, current);
			log_debug(logger, "[PLANIFICATION] PCB #%d has returned from BLOCKED state to READY", current->pid);
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
	pcb->qRecorrido = 0;
	pcb->estado = estado;
	pcb->alcanzoRestaurante = estado == ESPERANDO_PLATO ? true : false;
	queue_push(qB, pcb);
	log_debug(logger, "[PLANIFICATION] PCB #%d added to BLOCKED queue in %s state", pcb->pid, pcb->alcanzoRestaurante ? "ESPERANDO_PLATO" : "REPARTIDOR_DESCANSANDO"); // TODO: Mejorar log
	pthread_mutex_lock(&mutexQB);
}

void desbloquearPCB(int idPedido) {
	log_debug(logger, "[PLANIFICATION] Desbloqueando al PCB #%d...", idPedido);
	pthread_mutex_lock(&mutexQB);
	int qSize = queue_size(qB);
	t_queue *newQB = queue_create();
	t_pcb *pcbADesbloquear = malloc(sizeof(t_pcb));
	for (int i = 0; i < qSize; i++) {
		t_pcb *currentPCB = queue_pop(qB);
		if (currentPCB->pid == idPedido) {
			pcbADesbloquear = currentPCB;
			pcbADesbloquear->qEsperando = 0;
			pcbADesbloquear->qDescansado = 0;
			pcbADesbloquear->estado = ESPERANDO_EJECUCION;
		} else {
			queue_push(newQB, currentPCB);
		}
	}
	qB = newQB;
	pthread_mutex_unlock(&mutexQB);
	
	pthread_mutex_lock(&mutexQR);
	queue_push(qR, pcbADesbloquear);
	log_debug(logger, "[PLANIFICATION] PCB #%d has returned from BLOCKED state to READY", pcbADesbloquear->pid);
	pthread_mutex_unlock(&mutexQR);

	free(pcbADesbloquear);
}

void actualizarQDescansado() {
	log_debug(logger, "[PLANIFICATION] Incrementando tiempos de descanso en BLOCKED...");
	pthread_mutex_lock(&mutexQB);
	int sizeQB = queue_size(qB);
	for (int i = 0; i < sizeQB; i++) {
		t_pcb *current = queue_pop(qB);
		current->qDescansado++;
		queue_push(qB, current);
	}
	pthread_mutex_unlock(&mutexQB);
	log_debug(logger, "[PLANIFICATION] Tiempos de descanso incrementados en BLOCKED");
}

/* FIFO */

void actualizarQEconQR_FIFO() {
	// Reviso si puedo agregar más PCBs de QR a EXEC
	while (!queue_is_empty(qR) && puedeEjecutarAlguno()) {
		log_debug(logger, "[PLANIFICATION] Agregando PCBs de READY a EXEC...");
		log_debug(logger, "[PLANIFICATION] Espacio en EXEC: %d", queue_size(qE) - gradoMultiprocesamiento);
		pthread_mutex_lock(&mutexQR);
		t_pcb *current = queue_pop(qR);
		log_debug(logger, "[PLANIFICATION] PCB #%d removed from READY queue", current->pid);
		pthread_mutex_unlock(&mutexQR);
		pthread_mutex_lock(&mutexQE);
		current->estado = current->alcanzoRestaurante ? EN_CAMINO_A_CLIENTE : EN_CAMINO_A_RESTAURANTE;
		queue_push(qE, current);
		log_debug(logger, "[PLANIFICATION] PCB #%d added to EXEC queue", current->pid);
		pthread_mutex_unlock(&mutexQE);
	}
}

/* HRRN */

double obtenerTasaRta(t_pcb *pcb) {
	return (pcb->qEsperando + pcb->repartidor->freqDescanso)/pcb->repartidor->freqDescanso;
}

void actualizarTiemposEspera() {
	log_debug(logger, "[PLANIFICATION] Incrementando tiempos de espera en READY...");
	pthread_mutex_lock(&mutexQR);
	int sizeQR = queue_size(qR);
	for (int i = 0; i < sizeQR; i++) {
		t_pcb *currentPcb = queue_pop(qR);
		currentPcb->qEsperando++;
		queue_push(qR, currentPcb);
	}
	pthread_mutex_unlock(&mutexQR);
	log_debug(logger, "[PLANIFICATION] Tiempos de espera incrementados en READY");
}

t_pcb *proximoAEjecutarHRRN() {
	log_debug(logger, "[PLANIFICATION - HRRN] Obteniendo el próximo PCB a pasar a EXEC...");
	pthread_mutex_lock(&mutexQR);
	int tasaRtaMax = 0;
	int qSize = queue_size(qR);
	t_queue *newQR = queue_create(); 
	t_pcb *nextPCBtoExec = malloc(sizeof(t_pcb));
	for (int i = 0; i < qSize; i++) {
		t_pcb *currentPCB = queue_pop(qR);
		int tasaRtaActual = obtenerTasaRta(currentPCB);
		if (tasaRtaActual > tasaRtaMax) {
			if (nextPCBtoExec != NULL) { queue_push(newQR, nextPCBtoExec); }
			nextPCBtoExec = currentPCB;
			tasaRtaMax = tasaRtaActual;
		} else {
			queue_push(newQR, currentPCB);
		}
	}
	qR = newQR;
	pthread_mutex_unlock(&mutexQR);
	log_debug(logger, "[PLANIFICATION] PCB #%d removed from READY queue", nextPCBtoExec->pid);
	return nextPCBtoExec;
}

void actualizarQEconQR_HRRN() {
	// Reviso si puedo agregar más PCBs de QR a EXEC
	while (!queue_is_empty(qR) && puedeEjecutarAlguno()) {
		log_debug(logger, "[PLANIFICATION] Agregando PCBs de READY a EXEC...");
		log_debug(logger, "[PLANIFICATION] Espacio en EXEC: %d", queue_size(qE) - gradoMultiprocesamiento);
		t_pcb *current = proximoAEjecutarHRRN();
		pthread_mutex_lock(&mutexQE);
		current->estado = current->alcanzoRestaurante ? EN_CAMINO_A_CLIENTE : EN_CAMINO_A_RESTAURANTE;
		queue_push(qE, current);
		log_debug(logger, "[PLANIFICATION] PCB #%d added to EXEC queue", current->pid);
		pthread_mutex_unlock(&mutexQE);
	}
}

/* SJF */

void actualizarEstimacion(t_pcb *pcb) {
	double nuevaEstimacion = pcb->qRecorrido*alpha + (1 - alpha)*pcb->ultimaEstimacion;
	pcb->ultimaEstimacion = nuevaEstimacion;
}

t_pcb *proximoAEjecutarSJF() {
	log_debug(logger, "[PLANIFICATION - SJF] Obteniendo el próximo PCB a pasar a EXEC...");
	pthread_mutex_lock(&mutexQR);
	t_pcb *primerPCB = queue_peek(qR);
	double estMinima = primerPCB->ultimaEstimacion;
	int qSize = queue_size(qR);
	t_queue *newQR = queue_create();
	t_pcb *nextPCBtoExec = malloc(sizeof(t_pcb));
	for (int i = 0; i < qSize; i++) {
		t_pcb *currentPCB = queue_pop(qR);
		if (currentPCB->ultimaEstimacion < estMinima) {
			if (nextPCBtoExec != NULL) { queue_push(newQR, nextPCBtoExec); }
			nextPCBtoExec = currentPCB;
			estMinima = currentPCB->ultimaEstimacion;
		} else {
			queue_push(newQR, currentPCB);
		}
	}
	qR = newQR;
	pthread_mutex_unlock(&mutexQR);
	log_debug(logger, "[PLANIFICATION] PCB #%d removed from READY queue", nextPCBtoExec->pid);
	return nextPCBtoExec;
}

void actualizarQEconQR_SJF() {
	// Reviso si puedo agregar más PCBs de QR a EXEC
	while (!queue_is_empty(qR) && puedeEjecutarAlguno()) {
		log_debug(logger, "[PLANIFICATION] Agregando PCBs de READY a EXEC...");
		log_debug(logger, "[PLANIFICATION] Espacio en EXEC: %d", queue_size(qE) - gradoMultiprocesamiento);
		t_pcb *current = proximoAEjecutarSJF();
		pthread_mutex_lock(&mutexQE);
		current->estado = current->alcanzoRestaurante ? EN_CAMINO_A_CLIENTE : EN_CAMINO_A_RESTAURANTE;
		queue_push(qE, current);
		log_debug(logger, "[PLANIFICATION] PCB #%d added to EXEC queue", current->pid);
		pthread_mutex_unlock(&mutexQE);
	}
}

/* Ejecución */

void ejecutarCiclos() // Para FIFO, HRRN y SJF
{
	pthread_mutex_lock(&mutexQE);
	t_pcb *currentPcb = queue_pop(qE);

	while (currentPcb != NULL) {
		log_debug(logger, "[PLANIFICATION] Running execution...");
		// 1. Si debe descansar, pasa a BLOQUEADO
		if (debeDescansarRepartidor(currentPcb)) {
			pasarAQB(currentPcb, REPARTIDOR_DESCANSANDO);
			if (algoritmoSeleccionado == SJF) { actualizarEstimacion(currentPcb); }
			log_debug(logger, "El repartidor del PCB #%d debía descansar, el PCB pasó al estado BLOCKED", currentPcb->pid);
		} else if (currentPcb->estado == EN_CAMINO_A_RESTAURANTE && llegoAlRestaurante(currentPcb)) {
			// 2. Si llegó al restaurante y tiene todos los platos listos, sigue hacia el cliente, sino, a BLOQUEADO
			log_debug(logger, "[PLANIFICATION] El PCB #%d llegó al restaurante");
			if (todosPlatosListos(currentPcb)) {
				currentPcb->estado = EN_CAMINO_A_CLIENTE;
				currentPcb->alcanzoRestaurante = true;
				log_debug(logger, "[PLANIFICATION] El PCB #%d continúa su viaje hacia el cliente...", currentPcb->pid);
			} else {
				pasarAQB(currentPcb, ESPERANDO_PLATO);
				if (algoritmoSeleccionado == SJF) { actualizarEstimacion(currentPcb); }
				log_debug(logger, "[PLANIFICATION] El PCB #%d tiene platos pendientes, pasa a BLOCKED", currentPcb->pid);
			}
		} else if (currentPcb->estado == EN_CAMINO_A_CLIENTE && llegoAlCliente(currentPcb)) {
			// 3. Si llegó al cliente, se da por concluido el pedido
			log_debug(logger, "[PLANIFICATION] El PCB #%d llegó al cliente, finalizando pedido...", currentPcb->pid);
			informarPedidoFinalizado(currentPcb);
			informarEntregaCliente(currentPcb);
			if (algoritmoSeleccionado == SJF) actualizarEstimacion(currentPcb);
			log_debug(logger, "[PLANIFICATION] El PCB #%d fue entregado a %s por el repartidor #%d, pasa a FINALIZADO", currentPcb->pid, currentPcb->idCliente, currentPcb->repartidor->idRepartidor);
			queue_push(currentPcb, qF);
			liberarRepartidor(currentPcb);
		} else {
			// 4. Si sigue ejecutando, debe actualizar su posición de acuerdo a donde esté viajando
			actualizarPosicion(currentPcb, currentPcb->alcanzoRestaurante ? HACIA_CLIENTE : HACIA_RESTAURANTE);
		}
		currentPcb = queue_pop(qE); // Continúa el ciclo con el siguiente PCB
		if (algoritmoSeleccionado == HRRN) { actualizarTiemposEspera(); }
		actualizarQDescansado();
        sleep(tiempoRetardoCpu);
	}

	pthread_mutex_unlock(&mutexQE);
}