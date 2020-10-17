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
	afinidadesDisponibles = list_create();
	posicion = malloc(sizeof(t_posicion));
	recetasDisponibles = md->platos;
	posicion->posX = md->posX;
	posicion->posY = md->posY;
	cantidadHornos = md->cantidadHornos;
	cantidadPedidos = md->cantidadPedidos;
	cantidadCocineros = md->cantidadCocineros;
	afinidadesDisponibles = md->afinidades;
}

void inicializarListaCocineros()
{
	queuesCocineros = list_create();

	for (int i = 0; i < cantidadCocineros; i++) {
		t_queue_obj *cocineroActual = malloc(sizeof(t_queue_obj));
		cocineroActual->idCocinero = i;
		cocineroActual->afinidad = list_get(afinidadesDisponibles, i) != NULL ? list_get(afinidadesDisponibles, i) : "General" ;
		
		t_queue *qR = queue_create();
		t_queue *qE = queue_create();
		t_queue *qB = queue_create();
		t_queue *qF = queue_create();

		cocineroActual->queueReady = qR;
		cocineroActual->queueExec = qE;
		cocineroActual->queueBloq = qB;
		cocineroActual->queueFinished = qF;

		list_add(queuesCocineros, cocineroActual);
	}

}

void inicializarQueuesIO()
{
	queuesIO = list_create();

	for (int i = 0; i < cantidadHornos; i++) {
		t_queue_IO *hornoActual = malloc(sizeof(t_queue_IO));
		hornoActual->idCocinero = i+1;
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