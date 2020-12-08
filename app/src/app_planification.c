#include "../include/app_planification.h"

// Retorna si un repartidor ya descansó lo suficiente como para volver a READY
bool repartidor_descansado(t_pcb *currentPcb) {
	return currentPcb->qDescansado >= currentPcb->repartidor->tiempoDescanso;
}

// Retorna si un repartidor ya completó una tanda de ejecución y debe pasar a BLOCKED para descansar
bool debe_descansar_repartidor(t_pcb *currentPcb) {
	return currentPcb->qRecorrido == currentPcb->repartidor->freqDescanso;
}

// Retorna si hay espacio en qE para ejecutar otro PCB
bool puede_ejecutar_alguno() {
	return queue_size(qE) < grado_multiprocesamiento;
}

/* Funciones */

double getDistancia(t_posicion *posRepartidor, t_posicion *posRest) {
	int x = posRepartidor->posX - posRest->posX;
	int y = posRepartidor->posY - posRest->posY;
	double distancia = sqrt(pow(x, 2) + pow(y, 2));
	return distancia;
}

t_repartidor *getRepartidorMasCercano(t_posicion *posRest) {
	int indexRepartidor = 0;
	t_repartidor *primer_repartidor = list_get(repartidores_disp, 0);
	int distMinima = ceil(getDistancia(primer_repartidor, posRest));
	for (int i = 0; i < list_size(repartidores_disp); i++) {
		t_repartidor *repActual = list_get(repartidores_disp, i);
		int distARest = ceil(getDistancia(repActual->posRepartidor, posRest));
		if (distARest < distMinima) {
			distMinima = distARest;
			indexRepartidor = i;
		}
	}
	t_repartidor *repartidorMasCercano = list_get(repartidores_disp, indexRepartidor);
	
	pthread_mutex_lock(&mutexListaDisponibles);
	list_remove(repartidores_disp, indexRepartidor);
	pthread_mutex_unlock(&mutexListaDisponibles);
	
	pthread_mutex_lock(&mutexListaOcupados);
	list_add(repartidoresOcupados, repartidorMasCercano);	
	pthread_mutex_unlock(&mutexListaOcupados);
	
	return repartidorMasCercano;
}

/* Avisos a COMANDA y CLIENTE */

t_pedido *get_pedido_from_comanda(t_request *request) {
	int conexionComanda = conectarseA(COMANDA);
	enviarPaquete(conexionComanda, APP, OBTENER_PEDIDO, request);
	t_header *header = recibirHeaderPaquete(conexionComanda);
	t_pedido *pedido = recibirPayloadPaquete(header, conexionComanda);
	liberarConexion(conexionComanda);
	free(header);
	return pedido;
}

bool algun_plato_no_listo(t_list *platos_pedido) {
	bool no_esta_completo(void *actual) {
		t_plato *plato_actual = actual;
		return plato_actual->cantidadLista < plato_actual->cantidadPedida;
	};
	return list_any_satisfy(platos_pedido, &no_esta_completo);
}

bool platos_completos(t_list *platos_pedido) {
	bool esta_completo(void *actual) {
		t_plato *plato_actual = actual;
		return plato_actual->cantidadLista >= plato_actual->cantidadPedida;
	};
	return list_all_satisfy(platos_pedido, &esta_completo);
}

bool todos_platos_listos(t_pcb *pcb) {
	log_checking_all_platos_listos(algoritmo, pcb->pid);

	if (string_equals_ignore_case(pcb->restaurante, rest_default)) {
		return true;
	}

	t_request *request = getTRequest(pcb->pid, pcb->restaurante);
	t_pedido *pedido = get_pedido_from_comanda(request);
	bool todos_completos = platos_completos(pedido->platos);
	free(request);
	free(pedido);
	return todos_completos;
}

void informar_pedido_finalizado(t_pcb *pcb) {
	log_app_FinalizarPedido(algoritmo, pcb->pid);
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

void informar_entrega_cliente(t_pcb *pcb) { // Revisar porque hay que dejar un socket de escucha en el cliente
	log_app_entrega_a_cliente(algoritmo, pcb->pid, pcb->idCliente);
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

bool llego_al_rest(t_pcb *pcb) {
	return pcb->posRest->posX == pcb->repartidor->posRepartidor->posX && pcb->posRest->posY == pcb->repartidor->posRepartidor->posY;
}

bool llego_al_cliente(t_pcb *pcb) {
	return pcb->posCliente->posX == pcb->repartidor->posRepartidor->posX && pcb->posCliente->posY == pcb->repartidor->posRepartidor->posY;
}

t_posicion *get_next_step(t_posicion *posRepartidor, t_posicion *posObjetivo) {
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

void update_posicion(t_pcb *current_pcb, tour_code code) {
	current_pcb->qRecorrido++;
	t_posicion *next_pos = malloc(sizeof(t_posicion));
	log_app_repartidor_en_camino(algoritmo, current_pcb->pid, code);
	switch (code) {
		case HACIA_CLIENTE:
			next_pos = get_next_step(current_pcb->repartidor->posRepartidor, current_pcb->posCliente);
			break;
		case HACIA_RESTAURANTE:
			next_pos = get_next_step(current_pcb->repartidor->posRepartidor, current_pcb->posRest);
			break;
	}
	log_app_traslado_repartidor(algoritmo, current_pcb->pid,
								current_pcb->repartidor->posRepartidor->posX,
								current_pcb->repartidor->posRepartidor->posY,
								next_pos->posX, next_pos->posY);
	current_pcb->repartidor->posRepartidor->posX = next_pos->posX;
	current_pcb->repartidor->posRepartidor->posY = next_pos->posY;
	free(next_pos);
}

void liberar_repartidor(t_repartidor *repartidor) {
	pthread_mutex_lock(&mutexListaOcupados);
	list_remove(repartidoresOcupados, repartidor->idRepartidor);
	pthread_mutex_unlock(&mutexListaOcupados);
	pthread_mutex_lock(&mutexListaDisponibles);
	list_add(repartidores_disp, repartidor);
	log_app_repartidor_libre(algoritmo, repartidor->idRepartidor, list_size(repartidores_disp));
	pthread_mutex_unlock(&mutexListaDisponibles);
}

/* Planificación */

t_pcb *crear_pcb(t_cliente *cliente, int idPedido) {
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
	pcb->restaurante = cliente->restSelecc;
	pcb->posRest = malloc(sizeof(t_posicion));
	pcb->posRest->posX = cliente->posRest->posX;
	pcb->posRest->posY = cliente->posRest->posY;
	pcb->ultimaEstimacion = estimacionInicial;
	return pcb;
}

void agregar_a_QN(t_pcb *pcb) {
	log_app_adding_to_new(algoritmo, pcb->pid);
	pthread_mutex_lock(&mutexQN);
	queue_push(qN, pcb);
	log_app_added_to_new(algoritmo, pcb->pid);
	pthread_mutex_unlock(&mutexQN);
}

void update_QR_con_QN() {
	// Mientras haya repartidores disponibles y PCBs en NEW, los asigna y añade a READY
	while (!list_is_empty(repartidores_disp) && !queue_is_empty(qN)) {
		log_app_asignando_repartidores(algoritmo, list_size(repartidores_disp), grado_multiprocesamiento - queue_size(qE));

		pthread_mutex_lock(&mutexQN);
		t_pcb *next_in_line = queue_pop(qN);
		log_app_removed_from_new(algoritmo, next_in_line->pid);
		pthread_mutex_unlock(&mutexQN);
		
		next_in_line->repartidor = getRepartidorMasCercano(next_in_line->posRest);
		log_app_repartidor_asignado(algoritmo, next_in_line->repartidor->idRepartidor, next_in_line->pid);
		next_in_line->estado = ESPERANDO_EJECUCION;

		pthread_mutex_lock(&mutexQR);
		queue_push(qR, next_in_line);
		log_app_added_to_ready(algoritmo, next_in_line->pid);
		pthread_mutex_unlock(&mutexQR);
	}
}

void update_QR_con_QB() {
	// Revisamos si algún PCB de BLOCKED ya descansó y puede pasar a READY
	int sizeQB = queue_size(qB);
	for (int b = 0; b < sizeQB; b++) {
		pthread_mutex_lock(&mutexQB);
		t_pcb *current = queue_pop(qB);
		pthread_mutex_unlock(&mutexQB);
		if (current->estado == REPARTIDOR_DESCANSANDO && repartidor_descansado(current)) {
			log_app_pcb_rest_end(algoritmo, current->pid, current->qDescansado);
			current->estado = ESPERANDO_EJECUCION;
			current->qDescansado = 0;
			pthread_mutex_lock(&mutexQR);
			queue_push(qR, current);
			log_app_blocked_to_ready(algoritmo, current->pid);
			pthread_mutex_unlock(&mutexQR);
		} else {
			pthread_mutex_lock(&mutexQB);
			queue_push(qB, current);
			pthread_mutex_unlock(&mutexQB);
		}
	}
}

void pasar_a_QB(t_pcb *pcb, t_estado estado) {
	pthread_mutex_lock(&mutexQB);
	pcb->qRecorrido = 0;
	pcb->estado = estado;
	pcb->alcanzoRestaurante = estado == ESPERANDO_PLATO ? true : false;
	queue_push(qB, pcb);
	log_app_pasar_a_QB(algoritmo, pcb->pid, pcb->alcanzoRestaurante);
	pthread_mutex_lock(&mutexQB);
}

void desbloquear_PCB(int idPedido) {
	log_app_unblocking_pcb(algoritmo, idPedido);
	pthread_mutex_lock(&mutexQB);
	int qSize = queue_size(qB);
	t_queue *new_QB = queue_create();
	t_pcb *pcb_a_desbloquear = malloc(sizeof(t_pcb));
	for (int i = 0; i < qSize; i++) {
		t_pcb *currentPCB = queue_pop(qB);
		if (currentPCB->pid == idPedido) {
			pcb_a_desbloquear = currentPCB;
			pcb_a_desbloquear->qEsperando = 0;
			pcb_a_desbloquear->qDescansado = 0;
			pcb_a_desbloquear->estado = ESPERANDO_EJECUCION;
		} else {
			queue_push(new_QB, currentPCB);
		}
	}
	qB = new_QB;
	pthread_mutex_unlock(&mutexQB);
	
	pthread_mutex_lock(&mutexQR);
	queue_push(qR, pcb_a_desbloquear);
	log_app_blocked_to_ready(algoritmo, pcb_a_desbloquear->pid);
	pthread_mutex_unlock(&mutexQR);

	free(pcb_a_desbloquear);
}

void update_QDescansado() {
	if (!queue_is_empty(qB)) {
		log_app_updating_QB_times(algoritmo);
		pthread_mutex_lock(&mutexQB);
		int sizeQB = queue_size(qB);
		for (int i = 0; i < sizeQB; i++) {
			t_pcb *current = queue_pop(qB);
			current->qDescansado++;
			queue_push(qB, current);
		}
		pthread_mutex_unlock(&mutexQB);
		log_app_QB_times_increased(algoritmo);
	}
}

/* FIFO */

void update_QE_con_QR_FIFO() {
	// Reviso si puedo agregar más PCBs de QR a EXEC
	while (!queue_is_empty(qR) && puede_ejecutar_alguno()) {
		log_app_ready_to_exec(algoritmo, grado_multiprocesamiento, queue_size(qE));
		pthread_mutex_lock(&mutexQR);
		t_pcb *current = queue_pop(qR);
		log_app_removed_from_ready(algoritmo, current->pid);
		pthread_mutex_unlock(&mutexQR);
		pthread_mutex_lock(&mutexQE);
		current->estado = current->alcanzoRestaurante ? EN_CAMINO_A_CLIENTE : EN_CAMINO_A_RESTAURANTE;
		queue_push(qE, current);
		log_app_added_to_exec(algoritmo, current->pid);
		pthread_mutex_unlock(&mutexQE);
	}
}

/* HRRN */

double obtener_tasa_rta(t_pcb *pcb) {
	return (pcb->qEsperando + pcb->repartidor->freqDescanso)/pcb->repartidor->freqDescanso;
}

void update_tiempos_espera() {
	log_app_increasing_tiempos_espera();
	pthread_mutex_lock(&mutexQR);
	int sizeQR = queue_size(qR);
	for (int i = 0; i < sizeQR; i++) {
		t_pcb *current_pcb = queue_pop(qR);
		current_pcb->qEsperando++;
		queue_push(qR, current_pcb);
	}
	pthread_mutex_unlock(&mutexQR);
	log_app_tiempos_espera_increased();
}

t_pcb *prox_to_exec_HRRN() {
	log_app_next_pcb_HRRN();
	pthread_mutex_lock(&mutexQR);
	int tasa_rta_max = 0;
	int qSize = queue_size(qR);
	t_queue *new_QR = queue_create(); 
	t_pcb *next_PCB_to_exec = malloc(sizeof(t_pcb));
	for (int i = 0; i < qSize; i++) {
		t_pcb *current_PCB = queue_pop(qR);
		int tasa_rta_actual = obtener_tasa_rta(current_PCB);
		if (tasa_rta_actual > tasa_rta_max) {
			if (next_PCB_to_exec != NULL) { queue_push(new_QR, next_PCB_to_exec); }
			next_PCB_to_exec = current_PCB;
			tasa_rta_max = tasa_rta_actual;
		} else {
			queue_push(new_QR, current_PCB);
		}
	}
	qR = new_QR;
	pthread_mutex_unlock(&mutexQR);
	log_app_removed_from_ready(algoritmo, next_PCB_to_exec->pid);
	return next_PCB_to_exec;
}

void update_QE_con_QR_HRRN() {
	// Reviso si puedo agregar más PCBs de QR a EXEC
	while (!queue_is_empty(qR) && puede_ejecutar_alguno()) {
		log_app_ready_to_exec(algoritmo, grado_multiprocesamiento, queue_size(qE));
		t_pcb *current = prox_to_exec_HRRN();
		pthread_mutex_lock(&mutexQE);
		current->estado = current->alcanzoRestaurante ? EN_CAMINO_A_CLIENTE : EN_CAMINO_A_RESTAURANTE;
		queue_push(qE, current);
		log_app_added_to_exec(algoritmo, current->pid);
		pthread_mutex_unlock(&mutexQE);
	}
}

/* SJF */

void update_estimacion(t_pcb *pcb) {
	double nueva_estimacion = pcb->qRecorrido*alpha + (1 - alpha)*pcb->ultimaEstimacion;
	pcb->ultimaEstimacion = nueva_estimacion;
}

t_pcb *prox_a_ejecutar_SJF() {
	log_app_next_pcb_SJF();
	pthread_mutex_lock(&mutexQR);
	
	t_pcb *primer_pcb = queue_peek(qR);
	double est_minima = primer_pcb->ultimaEstimacion;
	int qSize = queue_size(qR);
	t_queue *new_QR = queue_create();
	t_pcb *next_pcb_to_exec = malloc(sizeof(t_pcb));

	for (int i = 0; i < qSize; i++) {
		t_pcb *current_pcb = queue_pop(qR);
		if (current_pcb->ultimaEstimacion < est_minima) {
			if (next_pcb_to_exec != NULL) { queue_push(new_QR, next_pcb_to_exec); }
			next_pcb_to_exec = current_pcb;
			est_minima = current_pcb->ultimaEstimacion;
		} else {
			queue_push(new_QR, current_pcb);
		}
	}
	qR = new_QR;
	
	pthread_mutex_unlock(&mutexQR);
	log_app_removed_from_ready(algoritmo, next_pcb_to_exec->pid);
	return next_pcb_to_exec;
}

void update_QE_con_QR_SJF() {
	// Reviso si puedo agregar más PCBs de QR a EXEC
	while (!queue_is_empty(qR) && puede_ejecutar_alguno()) {
		log_app_ready_to_exec(algoritmo, grado_multiprocesamiento, queue_size(qE));
		t_pcb *current = prox_a_ejecutar_SJF();
		pthread_mutex_lock(&mutexQE);
		current->estado = current->alcanzoRestaurante ? EN_CAMINO_A_CLIENTE : EN_CAMINO_A_RESTAURANTE;
		queue_push(qE, current);
		log_app_added_to_exec(algoritmo, current->pid);
		pthread_mutex_unlock(&mutexQE);
	}
}

/* Ejecución */

void ejecutar_ciclos() // Para FIFO, HRRN y SJF
{
	pthread_mutex_lock(&mutexQE);
	t_pcb *current_pcb = queue_pop(qE);
	bool get_next_pcb = false;

	while (current_pcb != NULL) {
		log_app_running_exec_cycle(algoritmo);
		if (debe_descansar_repartidor(current_pcb)) {

			// 1. Si debe descansar, pasa a BLOQUEADO
			pasar_a_QB(current_pcb, REPARTIDOR_DESCANSANDO);
			if (algoritmoSeleccionado == SJF) { update_estimacion(current_pcb); }
			log_app_repartidor_cansado(algoritmo, current_pcb->pid);
			get_next_pcb = true;

		} else if (current_pcb->estado == EN_CAMINO_A_RESTAURANTE && llego_al_rest(current_pcb)) {
			
			// 2. Si llegó al restaurante y tiene todos los platos listos, sigue hacia el cliente, sino, a BLOQUEADO
			log_app_pcb_llego_al_rest(algoritmo, current_pcb->pid);
			if (todos_platos_listos(current_pcb)) {
				current_pcb->estado = EN_CAMINO_A_CLIENTE;
				current_pcb->alcanzoRestaurante = true;
				log_app_continua_hacia_cliente(algoritmo, current_pcb->pid);
				//queue_push(qE, current_pcb);
			} else {
				pasar_a_QB(current_pcb, ESPERANDO_PLATO);
				if (algoritmoSeleccionado == SJF) { update_estimacion(current_pcb); }
				log_app_platos_pendientes(algoritmo, current_pcb->pid);
				get_next_pcb = true;				
			}

		} else if (current_pcb->estado == EN_CAMINO_A_CLIENTE && llego_al_cliente(current_pcb)) {

			// 3. Si llegó al cliente, se da por concluido el pedido
			log_app_pcb_llego_al_cliente(algoritmo, current_pcb->pid, current_pcb->idCliente);
			informar_pedido_finalizado(current_pcb);
			//informar_entrega_cliente(current_pcb); // Definir cómo recibe el cliente este mensaje
			if (algoritmoSeleccionado == SJF) update_estimacion(current_pcb);
			log_app_pcb_entregado_al_cliente(algoritmo, current_pcb->pid, current_pcb->idCliente, current_pcb->repartidor->idRepartidor);
			queue_push(current_pcb, qF);
			liberar_repartidor(current_pcb);
			get_next_pcb = true;			

		} else {

			// 4. Si sigue ejecutando, debe actualizar su posición de acuerdo a donde esté viajando
			update_posicion(current_pcb, current_pcb->alcanzoRestaurante ? HACIA_CLIENTE : HACIA_RESTAURANTE);
			//queue_push(qE, current_pcb);

		}
		if (get_next_pcb) { current_pcb = queue_pop(qE); }// Continúa el ciclo con el siguiente PCB
		if (algoritmoSeleccionado == HRRN) { update_tiempos_espera(); }
		update_QDescansado();
        sleep(tiempoRetardoCpu);
	}

	pthread_mutex_unlock(&mutexQE);
}