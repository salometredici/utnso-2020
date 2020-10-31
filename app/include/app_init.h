#ifndef APP_INIT_H
#define APP_INIT_H

#include "../../shared/include/shared_commons.h"
#include "../../shared/include/shared_core.h"
#include "../../shared/include/shared_serialization.h"
#include "tests.h"

int conexionComanda;
t_list *restaurantesConectados;
t_list *idsGenerados;

/* Diccionario de algortimos */

#define FIFO 1
#define HRRN 2
#define SJF 3

static t_keys diccionarioAlgoritmos[] = {
    { "FIFO", FIFO },
    { "HRRN", HRRN },
    { "SJF", SJF }
};

#define ALGORITMONKEYS (sizeof(diccionarioAlgoritmos)/sizeof(t_keys))

/* Restaurante default */

#define restauranteDefault "RestauranteDefault"
t_list *platosResDefault; // Lista de strings, por ej.: ["Milanesa", "AsadoConFritas"]
t_posicion *posResDefault;
// Según el issue #1942, hay que agregar un precio default a todos los platos

/* Planificación */

double alpha;
int tiempoRetardoCpu;
int estimacionInicial;
int gradoMultiprocesamiento; // Cantidad de repartidores que pueden estar en Exec a la vez
char *algoritmo;
int algoritmoSeleccionado;

/* Queues */

t_queue *qN;
t_queue *qR;
t_queue *qE;
t_queue *qB;
t_queue *qF;
int cantidadRepartidores; // Grado de multiprogramación o cantidad de instancias totales

/* Repartidores */

t_list *repartidoresDisponibles;
t_list *repartidoresOcupados;

typedef struct {
	int idRepartidor;
	int freqDescanso; // Cantidad de ciclos de CPU que puede ejecutar seguidos
	int tiempoDescanso; // Cantidad de ciclos de CPU que debe descansar luego de ejecutar su frecuencia
	t_posicion *posRepartidor;
} t_repartidor;

typedef enum {
	SIN_ASIGNAR = 1,
	ESPERANDO_EJECUCION = 2,
	ESPERANDO_PLATO = 3,
	REPARTIDOR_DESCANSANDO = 4,
	EN_CAMINO_A_RESTAURANTE = 5,
	EN_CAMINO_A_CLIENTE = 6
} t_estado_pcb;

typedef struct {
	int pid;
	int qRecorrido;
	int qDescansado;
	t_estado_pcb estado;
	bool alcanzoRestaurante;
	//t_list *instrucciones; // ?

	t_repartidor *repartidor;
	char *idCliente;
	int socketCliente; //=
	t_posicion *posCliente;
	char *restaurante;
	t_posicion *posRest;
} t_pcb;

typedef struct {
	char *idCliente; // Si es cliente tiene CLIENTE_ID, si es un restaurante va a ser NOMBRE_RESTAURANTE
	int socketCliente;
	bool esRestaurante;
	t_posicion *posCliente;
	char *restauranteSeleccionado;
	t_posicion *posRestaurante;
} t_cliente;

void planificarProximo(t_pcb *pcb, t_md *md);
t_repartidor *getRepartidorMasCercano(t_posicion *posRest);
double getDistancia(t_posicion *posRepartidor, t_posicion *posRestaurante);

#endif
