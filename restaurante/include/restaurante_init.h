#ifndef RESTAURANTE_H_
#define RESTAURANTE_H_

#include "../../shared/include/shared_commons.h"
#include "../../shared/include/shared_core.h"
#include "../../shared/include/shared_serialization.h"
#include "tests.h"

int socketServidor;
int conexionSindicato;
char *nombreRestaurante;

// Valores obtenidos de md

int cantidadHornos; // i/o
int cantidadPedidos; // necesita semaforo, se va a usar para generar id de pedidos nuevos
int cantidadCocineros; // define multiprocesamiento, Y AFINIDADES!
t_list *recetasDisponibles;
t_list *afinidadesDisponibles;
t_posicion *posicion;

// Planificación

t_list *queuesCocineros;

typedef struct {
    int idCocinero;
    char *afinidad;
    t_queue *queueReady;
    t_queue *queueExec;
    t_queue *queueBloq;
    t_queue *queueFinished;
} t_queue_obj;

t_list *queuesIO;

typedef struct {
    int idHorno;
    // PCB ?
    bool estaOcupado;
    t_queue *queueIO;
} t_queue_IO;

int quantum;

// Otros

pthread_t threadConexiones;

char* respuesta;

#endif
