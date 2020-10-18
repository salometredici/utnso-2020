#include "../include/restaurante_init.h"

char *getNombreRestaurante() {
	return config_get_string_value(config, "NOMBRE_RESTAURANTE");
}

char *getQuantum() {
	return config_get_int_value(config, "QUANTUM");
}

void obtenerMetadata() {
	enviarPaquete(conexionSindicato, RESTAURANTE, OBTENER_RESTAURANTE, nombreRestaurante);
	t_header *header = recibirHeaderPaquete(conexionSindicato);
	md_restaurante *md = recibirPayloadPaquete(header, conexionSindicato);
	inicializarVariables(md);
	logMetadata(md);
	free(header);
	free(md);
}

void inicializarVariables(md_restaurante *md) {
	quantum = getQuantum();
	recetasDisponibles = list_create();
	afinidadesMd = list_create();
	posicion = malloc(sizeof(t_posicion));
	recetasDisponibles = md->platos;
	posicion->posX = md->posX;
	posicion->posY = md->posY;
	cantidadHornos = md->cantidadHornos;
	cantidadPedidos = md->cantidadPedidos;
	cantidadCocineros = md->cantidadCocineros;
	afinidadesMd = md->afinidades;
}

void inicializarListaCocineros()
{
	queuesCocineros = list_create();
	afinidadesUnicas = list_create();
	int QAfinidadesMd = list_size(afinidadesMd);

	for (int i = 0; i < QAfinidadesMd; i++) {
		char *afinidadActual = list_get(afinidadesMd, i);

		bool stringFound(void *actual) {
			char *stringActual = actual;
			return string_equals_ignore_case(afinidadActual, stringActual);
		}

		// Buscamos si esa afinidad ya fue agregada a la lista de afinidades únicas
		t_list *filtradas = list_filter(afinidadesUnicas, &stringFound); 
		
		if (list_is_empty(filtradas)) {
			list_add(afinidadesUnicas, afinidadActual);
			int ocurrenciasAA = list_count_satisfying(afinidadesMd, &stringFound); //
		}
	}

	if (QAfinidadesMd < cantidadCocineros) {
		int afMdUnicas = list_size(afinidadesUnicas);
		int o = cantidadCocineros - QAfinidadesMd; //afMdUnicas;
		list_add(afinidadesUnicas, "General");
	}
		
	int we = list_size(afinidadesUnicas);
	printf("afinidades unicas: %d\n", we);
	
	for (int i = 0; i < we; i++) {
		printf("afinidad %d: %s\n", i, list_get(afinidadesUnicas, i));
	}

	// Una vez que tenemos la lista de afinidades únicas creamos su t_queue_obj

	QAfinidadesUnicas = list_size(afinidadesUnicas);

	for (int i = 0; i < QAfinidadesUnicas; i++) {
		t_queue_obj *cpuActual = malloc(sizeof(t_queue_obj));
		char *afinidadActual = list_get(afinidadesUnicas, i);//afinidadesDisponibles, i);
		cpuActual->afinidad = afinidadActual;//afinidadActual != NULL ? afinidadActual : "General" ;

		bool stringFound(void *actual) {
			char *stringActual = actual;
			return string_equals_ignore_case(afinidadActual, stringActual);
		}

		if (!string_equals_ignore_case(cpuActual->afinidad, "General")) {
			cpuActual->instanciasTotales = list_count_satisfying(afinidadesMd, &stringFound);
		} else {
			cpuActual->instanciasTotales = cantidadCocineros - QAfinidadesMd;
		}
		
		t_queue *qR = queue_create();
		t_queue *qE = queue_create();
		t_queue *qB = queue_create();
		t_queue *qF = queue_create();

		cpuActual->qR = qR;
		cpuActual->qE = qE;
		cpuActual->qB = qB;
		cpuActual->qF = qF;

		list_add(queuesCocineros, cpuActual);
	}

	for (int i = 0; i < list_size(queuesCocineros); i++) {
		t_queue_obj *cpuActual = list_get(queuesCocineros, i);
		printf("%d: afinidad: %s, instancias: %d, qR: %d, qE: %d, qB: %d, qF: %d\n", i, cpuActual->afinidad, cpuActual->instanciasTotales, &cpuActual->qR, &cpuActual->qE, &cpuActual->qB, &cpuActual->qF);
	}

	

}

void inicializarQueuesIO()
{
	queuesIO = list_create();

	for (int i = 0; i < cantidadHornos; i++) {
		t_queue_IO *hornoActual = malloc(sizeof(t_queue_IO));
		hornoActual->idHorno = i+1;
		hornoActual->estaOcupado = false;
		hornoActual->queueIO = queue_create();
		list_add(queuesIO, hornoActual);
	}
}


void initRestaurante() {
	conexionSindicato = conectarseA(SINDICATO);
	nombreRestaurante = getNombreRestaurante();
	obtenerMetadata();
	inicializarListaCocineros();
	inicializarQueuesIO();
}