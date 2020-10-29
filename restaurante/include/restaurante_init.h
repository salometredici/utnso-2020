#ifndef RESTAURANTE_INIT_H
#define RESTAURANTE_INIT_H

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
char *algoritmo;
int algoritmoSeleccionado;

/* Diccionario de algortimos */

#define FIFO 1
#define RR 2

static t_keys diccionarioAlgoritmos[] = {
    { "FIFO", FIFO },
    { "RR", RR }
};

#define ALGORITMONKEYS (sizeof(diccionarioAlgoritmos)/sizeof(t_keys))

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

/* Queues generales y cocineros/afinidades (CPUs únicas) */
// dentro de queuesCocineros se encuentra qR qE por cada afinidad

t_list *queuesCocineros;
t_queue *qB; //no es e/s
t_queue *qF;


typedef struct {
    char *afinidad;
    int instanciasTotales;
    t_queue *qR;
    t_queue *qE;
    pthread_mutex_t mutexQR;
    pthread_mutex_t mutexQE;
} t_queue_obj;

void logInitQueuesRestaurante(t_list *queuesCocineros);

/* Queue I/O */
pthread_mutex_t mutexEsperaIO;
pthread_mutex_t mutexEjecutaIO;
t_queue *esperandoIO;
t_queue *ejecutandoIO;
int instanciasTotalesIO;

void logInitQueueIORestaurante();

/* Proceso */

typedef enum {
	NEW = 1,
	ESPERANDO_EJECUCION = 2, //cdo esta en ready pero no empezo a ejecutar
    EJECUTANDO = 3,
	ESPERANDO_HORNO = 4, //esperando que se libere instancia io
	EN_HORNO = 5, //en io
    REPOSANDO = 6, // en bloqueado hasta que se cumpla qReposada
} t_estado_proceso;



typedef struct {
	char *idCliente; // Si es cliente tiene CLIENTE_ID, si es un restaurante va a ser NOMBRE_RESTAURANTE
	int socketCliente;
	bool esRestaurante;
	t_posicion *posCliente;
	char *restauranteSeleccionado;
	t_posicion *posRestaurante;
} t_cliente;

t_list *clientesConectados; //por socket

typedef struct {
    int pid;
    t_list *pasosReceta; //instrucciones
    char *idCliente;
    char *plato;
    t_estado_proceso *estado;
    char *instruccionActual;
    int posInstruccionActual; 
    int qInstruccionActual; 
    int qEjecutada; // sirve para contar el tiempo en io tambien
} t_proceso; 

#endif
