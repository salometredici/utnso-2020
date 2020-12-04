#include "../include/restaurante_planification.h"

// PCB
t_proceso *crearPcb(t_cliente *cliente, int idPedido, t_receta *receta) {
    t_proceso *pcb = malloc(sizeof(t_proceso));
    pcb->pid = idPedido;
    //pcb->idCliente = cliente->idCliente;
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
		log_planif_step("agregado a Queue", encontrado->afinidad);
	} else {
		bool esGeneral(void *element){
			t_queue_obj *actual = element;
			return string_equals_ignore_case(actual->afinidad, "General");
		};
		t_queue_obj *general = list_find(queuesCocineros, &esGeneral);
		pthread_mutex_lock(opcion ? &general->mutexQR : &general->mutexQE);
			queue_push(opcion ? general->qR : general->qE, pcb);
		pthread_mutex_unlock(opcion ? &general->mutexQR : &general->mutexQE);
		log_planif_step("agregado a Queue", "GENERAL");
	} 
}


/* Planificación */

// Actualiza los que hayan cumplido su tiempoen block a new
void actualizarQB(t_queue_obj *currentCPU) {
	//1. revisar queue bloqueados
	if (!queue_is_empty(currentCPU->qB)) {
		for(int i = 0; i < queue_size(currentCPU->qB);i++) {
			pthread_mutex_lock(&currentCPU->mutexQB);
			t_proceso *sigARevisar = queue_pop(currentCPU->qB); // Lo sacamos de la cola de Bloqueado
			pthread_mutex_unlock(&currentCPU->mutexQB);
			sigARevisar->qEjecutada++;
			sigARevisar->qInstruccionActual--;
			log_planif_step(sigARevisar->plato, sigARevisar->instruccionActual);
			//log_planif_step("Revisando bloqueados", NULL);
			if(/*sigARevisar->estado == REPOSANDO &&*/ sigARevisar->qInstruccionActual == 0){
				sigARevisar->estado = ESPERANDO_EJECUCION;
				sigARevisar->posInstruccionActual++;

				t_instrucciones_receta *sigPaso = list_get(sigARevisar->pasosReceta,sigARevisar->posInstruccionActual);
				// if(sigARevisar) {
				// 	sigARevisar->estado = DONE;
				// 	pthread_mutex_lock(&mutexQF);
				// 	queue_push(qF,sigARevisar);
				// 	pthread_mutex_unlock(&mutexQF);
				// 	log_planif_step("Pasó a done", NULL);
				// } else {
					sigARevisar->instruccionActual = sigPaso->paso;
					sigARevisar->qInstruccionActual = sigPaso->qPaso;
					sigARevisar->qEjecutada = 0;
					sigARevisar->quantum = quantum;
					
					pthread_mutex_lock(&currentCPU->mutexQR);
					queue_push(currentCPU->qR,sigARevisar);
					pthread_mutex_unlock(&currentCPU->mutexQR);
					log_planif_step("de bloqueado", "A EJECUCION");
				// }
				
			} else {
				pthread_mutex_lock(&currentCPU->mutexQB);
				queue_push(currentCPU->qB,sigARevisar);
				pthread_mutex_unlock(&currentCPU->mutexQB);
			}
		}
    }	
}

void actualizarQRaQE(t_queue_obj *currentCPU) {
	if (!queue_is_empty(currentCPU->qR))  {
		if(queue_size(currentCPU->qE) < currentCPU->instanciasTotales) {
			int disponibles = currentCPU->instanciasTotales - queue_size(currentCPU->qE);
			for(int i = 0; i < disponibles; i++) {
				pthread_mutex_lock(&currentCPU->mutexQR);
				t_proceso *sigARevisar = queue_pop(currentCPU->qR); // Lo sacamos de la cola de ready
				pthread_mutex_unlock(&currentCPU->mutexQR);
				
				pthread_mutex_lock(&currentCPU->mutexQE);
				queue_push(currentCPU->qE,sigARevisar);
				pthread_mutex_unlock(&currentCPU->mutexQE);
			}
		}
	}
}

void ejecutarCicloIO(t_queue_obj *currentCPU) {
	if (!queue_is_empty(ejecutandoIO)) {
		//nos fijamos si alguno termino su rafaga
		for(int i = 0; i < queue_size(ejecutandoIO);i++){
			pthread_mutex_lock(&mutexEjecutaIO);
			t_proceso *sigARevisar = queue_pop(ejecutandoIO); // Lo sacamos de la cola de Bloqueado
			pthread_mutex_unlock(&mutexEjecutaIO);
			if( sigARevisar->qInstruccionActual == 0 ){
				sigARevisar->posInstruccionActual++;
				t_instrucciones_receta *pasoSig = list_get(sigARevisar->pasosReceta,sigARevisar->posInstruccionActual);
				sigARevisar->instruccionActual = pasoSig->paso;
				sigARevisar->qInstruccionActual = pasoSig->qPaso;
				sigARevisar->qEjecutada = 0;
				if (string_equals_ignore_case(pasoSig->paso, "REPOSAR")) {
					log_planif_step("de IO", "A BLOQUEADO");
					sigARevisar->estado = REPOSANDO;
					pthread_mutex_lock(&currentCPU->mutexQB);
					queue_push(currentCPU->qB, sigARevisar);
					pthread_mutex_unlock(&currentCPU->mutexQB);
				} else{
					log_planif_step("de IO", "A READY");
					agregarQueue(1, sigARevisar); //agrego a ready slds
				}
			} else {
				sigARevisar->qEjecutada++;
				sigARevisar->qInstruccionActual--;
				pthread_mutex_lock(&mutexEjecutaIO);
					queue_push(ejecutandoIO,sigARevisar); // Lo sacamos de la cola de Bloqueado
				pthread_mutex_unlock(&mutexEjecutaIO);
				log_planif_step(sigARevisar->plato,sigARevisar->instruccionActual);
				sleep(tiempoRetardoCpu);
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
			log_planif_step("de esperando IO", "A EJECUCION");
		}
	}
}

void actualizarEsperaQB(t_queue_obj *currentCPU){
	pthread_mutex_lock(&currentCPU->mutexQB);
	for (int i = 0; i < queue_size(currentCPU->qB); i++) {
		t_proceso *current = queue_pop(currentCPU->qB);
		current->qEjecutada++;
		current->qInstruccionActual--;
		queue_push(currentCPU->qB, current);
	}
	pthread_mutex_unlock(&currentCPU->mutexQB);
}

void ejecutarFinalizar(t_proceso *currentProc){
	currentProc->estado = DONE;
	log_planif_step("agregado a Queue", "FINALIZADO");
	pthread_mutex_lock(&mutexQF);
	queue_push(qF,currentProc);
	pthread_mutex_unlock(&mutexQF);

	int conexionSindicato = conectarseA(SINDICATO);
	t_plato_listo *platoListo = malloc(sizeof(platoListo));
	platoListo->restaurante = nombreRestaurante;
	platoListo->idPedido = currentProc->pid;
	platoListo->plato = currentProc->plato;

	enviarPaquete(conexionSindicato, RESTAURANTE, PLATO_LISTO, platoListo);
	t_header *hrRtaPlatoListo = recibirHeaderPaquete(conexionSindicato);
	t_result *reqRtaPlatoListo = recibirPayloadPaquete(hrRtaPlatoListo, conexionSindicato);

	//todo avisar al modulo q solicito
}

void ejecutarCiclosFIFO(t_queue_obj *currentCPU){
	pthread_mutex_lock(&currentCPU->mutexQE);
	t_proceso *currentProc = queue_pop(currentCPU->qE);
	
	while (currentProc != NULL) {
		log_planif_step(currentProc->plato,currentProc->instruccionActual);
		currentProc->qEjecutada++;
		currentProc->qInstruccionActual--;
		if(currentProc->qInstruccionActual == 0) {
			currentProc->posInstruccionActual++;
			t_instrucciones_receta *pasoSig = list_get(currentProc->pasosReceta,currentProc->posInstruccionActual);
			
			if(pasoSig) {
				if (string_equals_ignore_case(pasoSig->paso, "REPOSAR")) {
					currentProc->estado = REPOSANDO;
					currentProc->instruccionActual = pasoSig->paso;
					currentProc->qInstruccionActual = pasoSig->qPaso;
					currentProc->qEjecutada = 0;
					
					pthread_mutex_lock(&currentCPU->mutexQB);
					queue_push(currentCPU->qB, currentProc);
					pthread_mutex_unlock(&currentCPU->mutexQB);
				} else if (string_equals_ignore_case(pasoSig->paso, "HORNEAR")) {
					currentProc->estado = ESPERANDO_HORNO;
					currentProc->instruccionActual = pasoSig->paso;
					currentProc->qInstruccionActual = pasoSig->qPaso;
					currentProc->qEjecutada = 0;
					
					pthread_mutex_lock(&mutexEsperaIO);
					queue_push(esperandoIO,currentProc);
					pthread_mutex_unlock(&mutexEsperaIO);
				} else {
					currentProc->estado = EJECUTANDO;
					currentProc->instruccionActual = pasoSig->paso;
					currentProc->qInstruccionActual = pasoSig->qPaso;
					currentProc->qEjecutada = 0;

					queue_push(currentCPU->qE, currentProc);
				}
			} else {
				ejecutarFinalizar(currentProc);
			}
		} else {
			queue_push(currentCPU->qE,currentProc);
		}
		
		ejecutarCicloIO(currentCPU);

		if(queue_size(currentCPU->qB)) {
			actualizarEsperaQB(currentCPU);
		}
		
		currentProc = queue_pop(currentCPU->qE); // Continúa el ciclo con el siguiente PCB
		pthread_mutex_unlock(&currentCPU->mutexQE);
		sleep(tiempoRetardoCpu);
	}
}

void ejecutarCiclosRR(t_queue_obj *currentCPU) {
	pthread_mutex_lock(&currentCPU->mutexQE);
	t_proceso *currentProc = queue_pop(currentCPU->qE);
	
	while (currentProc != NULL) {
		log_planif_step(currentProc->plato,currentProc->instruccionActual);
		currentProc->qEjecutada++;
		currentProc->qInstruccionActual--;
		currentProc->quantum--;
		if(currentProc->qInstruccionActual == 0) {
			currentProc->posInstruccionActual++;
			t_instrucciones_receta *pasoSig = list_get(currentProc->pasosReceta,currentProc->posInstruccionActual);
			
			if(pasoSig) {
				if (string_equals_ignore_case(pasoSig->paso, "REPOSAR")) {
					currentProc->estado = REPOSANDO;
					currentProc->instruccionActual = pasoSig->paso;
					currentProc->qInstruccionActual = pasoSig->qPaso;
					currentProc->qEjecutada = 0;
					
					pthread_mutex_lock(&currentCPU->mutexQB);
					queue_push(currentCPU->qB, currentProc);
					pthread_mutex_unlock(&currentCPU->mutexQB);
				} else if (string_equals_ignore_case(pasoSig->paso, "HORNEAR")) {
					currentProc->estado = ESPERANDO_HORNO;
					currentProc->instruccionActual = pasoSig->paso;
					currentProc->qInstruccionActual = pasoSig->qPaso;
					currentProc->qEjecutada = 0;
					
					pthread_mutex_lock(&mutexEsperaIO);
					queue_push(esperandoIO,currentProc);
					pthread_mutex_unlock(&mutexEsperaIO);
				} else {
					currentProc->estado = EJECUTANDO;
					currentProc->instruccionActual = pasoSig->paso;
					currentProc->qInstruccionActual = pasoSig->qPaso;
					currentProc->qEjecutada = 0;

					queue_push(currentCPU->qE, currentProc);
				}
			} else {
				ejecutarFinalizar(currentProc);
			}
		} else if (currentProc->quantum == 0) {
			currentProc->quantum = quantum;
			pthread_mutex_lock(&currentCPU->mutexQR);
			queue_push(currentCPU->qR, currentProc);
			pthread_mutex_unlock(&currentCPU->mutexQR);
			log_planif_step("Salio de ejecucion", "por QUANTUM");
		} else {
			queue_push(currentCPU->qE,currentProc);
		}
		
		ejecutarCicloIO(currentCPU);
		actualizarEsperaQB(currentCPU);
		currentProc = queue_pop(currentCPU->qE); // Continúa el ciclo con el siguiente PCB
		pthread_mutex_unlock(&currentCPU->mutexQE);
		sleep(tiempoRetardoCpu);
	}
}