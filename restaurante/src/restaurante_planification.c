

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
	pcb->quantum = quantum;

    return pcb;
}

void agregarQueue(int opcion, t_proceso *pcb) {
	bool correspondeAfinidad(void *element){
		t_queue_obj *actual = element;
		return string_equals_ignore_case(actual->afinidad, pcb->plato);
	};
	t_queue_obj *encontrado = list_find(queuesCocineros, &correspondeAfinidad);
	if(encontrado != NULL){
		pthread_mutex_lock(opcion ? &encontrado->mutexQR : &encontrado->mutexQE);
			queue_push(opcion ? encontrado->qR : encontrado->qE, pcb); 
		pthread_mutex_unlock(opcion ? &encontrado->mutexQR : &encontrado->mutexQE);
	} else {
		bool esGeneral(void *element){
			t_queue_obj *actual = element;
			return string_equals_ignore_case(actual->afinidad, "General");
		};
		t_queue_obj *general = list_find(queuesCocineros, &esGeneral);
		pthread_mutex_lock(opcion ? &general->mutexQR : &general->mutexQE);
			queue_push(opcion ? general->qR : general->qE, pcb);
		pthread_mutex_unlock(opcion ? &general->mutexQR : &general->mutexQE);

	} 
}

void agregarQR(t_proceso *pcb){
	agregarQueue(1, pcb);
}

/* Planificación */

// Actualiza los que hayan cumplido su tiempoen block a new
void actualizarQB() {
	//1. revisar queue bloqueados
	if (!queue_is_empty(qB)) {
		for(int i = 0; i < queue_size(qB);i++) {
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
			} else {
				pthread_mutex_lock(&mutexQB);
				queue_push(qB,sigARevisar);
				pthread_mutex_unlock(&mutexQB);
			}
		}
    }	
}

void actualizarQE() {
	// 1. Revisar si algún plato tiene que reposar / hornear / pasar a ready
	void _evaluarPorCocinero(void* element) {
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
						sigARevisar->instruccionActual = pasoActual->paso;
						sigARevisar->qInstruccionActual = pasoActual->qPaso;
						sigARevisar->qEjecutada = 0;
						
						pthread_mutex_lock(&mutexQB);
						queue_push(qB, sigARevisar);
						pthread_mutex_unlock(&mutexQB);
					} else if(string_equals_ignore_case(pasoActual->paso, "HORNEAR")) {
						sigARevisar->estado = ESPERANDO_HORNO;
						sigARevisar->instruccionActual = pasoActual->paso;
						sigARevisar->qInstruccionActual = pasoActual->qPaso;
						sigARevisar->qEjecutada = 0;
						
						pthread_mutex_lock(&mutexEsperaIO);
						queue_push(esperandoIO,sigARevisar);
						pthread_mutex_unlock(&mutexEsperaIO);
					} else {
						sigARevisar->estado = EJECUTANDO;
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
        //free(queue);
    };

    list_iterate(queuesCocineros, _evaluarPorCocinero);

	// 2. Reviso si puedo agregar más PCBs de QB a EXEC de repartidores que ya descansaron (OJO con los repartidores bloq que esperan avisos!)
	
	// 3. Reviso si puedo agregar más PCBs de QR a EXEC
}

void actualizarQRaQE(){
	void _evaluarCocineros(void* element){
		// rompe la compliacion si no se asigna la variable element
		t_queue_obj *queue = element;
		t_queue *qR = queue->qR;
		pthread_mutex_t mutexQR = queue->mutexQR;

		if (!queue_is_empty(qR))  {
			if(queue_size(queue->qE) < queue->instanciasTotales){
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
    	}
        
        //free(queue);
    };

    list_iterate(queuesCocineros, _evaluarCocineros);
}

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
				break;
			}
			
			ejecutarCicloIO();
			//sleep(tiempoRetardoCpu); // lei que más adelante van a agregar esta variable
			currentPcb = queue_pop(queue->qE);
		}
		pthread_mutex_unlock(&queue->mutexQE);
	}

	list_iterate(queuesCocineros, _ejecucionCocineros);
}

void ejecutarCiclosRR() {
	void _ejecucionCocineros(void *element){
		t_queue_obj *queue = element;
		pthread_mutex_lock(&queue->mutexQE);
		t_proceso *currentPcb = queue_pop(queue->qE);

		while (currentPcb != NULL) {
			currentPcb->qEjecutada++;
			currentPcb->qInstruccionActual--;
			currentPcb->quantum--;
			queue_push(queue->qE, currentPcb);

			// si el quantum es cero
			if(!currentPcb->quantum){
				ejecutarCicloIO();
				break;
			}
			
			ejecutarCicloIO();
			//sleep(tiempoRetardoCpu); // lei que más adelante van a agregar esta variable
			currentPcb = queue_pop(queue->qE);
		}
		pthread_mutex_unlock(&queue->mutexQE);
	}

	list_iterate(queuesCocineros, _ejecucionCocineros);
}

void *planificar(void *args) {
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
				// Largo plazo //se pueden reutilizar??
				actualizarQB();
				actualizarQE();
				actualizarQRaQE();		

				// Corto plazo
				ejecutarCiclosRR();
				break;
			default:
				break;
		}
	}
}