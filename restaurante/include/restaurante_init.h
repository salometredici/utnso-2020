#ifndef RESTAURANTE_H_
#define RESTAURANTE_H_

#include "../../shared/include/shared_commons.h"
#include "../../shared/include/shared_core.h"
#include "../../shared/include/shared_serialization.h"
#include "tests.h"

int socketServidor;
int conexionSindicato;
char *nombreRestaurante;

pthread_t threadConexiones;
char *respuesta;

int quantum;
char *algoritmoPlanificacion;

/* Metadata */

int cantidadHornos;
int cantidadPedidos; // necesita semaforo, se va a usar para generar id de pedidos nuevos
int cantidadCocineros;
t_list *recetasDisponibles;
t_list *afinidadesMd;
t_posicion *posicion;

// Otros datos

int QAfinidadesMd;
int QAfinidadesUnicas;
t_list *afinidadesUnicas;

/* Queues cocineros/afinidades (CPUs únicas) */

t_list *queuesCocineros;

typedef struct {
    char *afinidad;
    int instanciasTotales;
    t_queue *qR;
    t_queue *qE;
    t_queue *qB;
    t_queue *qF;
} t_queue_obj;

void logInitQueuesRestaurante(t_list *queuesCocineros);

/* Queue I/O */

t_queue *queueIO;
int instanciasTotalesIO;

void logInitQueueIORestaurante();

#endif
