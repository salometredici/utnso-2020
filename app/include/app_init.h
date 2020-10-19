#ifndef APP_INIT_H
#define APP_INIT_H

#include "../../shared/include/shared_commons.h"
#include "../../shared/include/shared_core.h"
#include "../../shared/include/shared_serialization.h"
#include "tests.h"

int conexionComanda;

/* Restaurante default */

t_list *platosResDefault; // Lista de strings
t_posicion *posResDefault;

/* Planificación */

double alpha;
int tiempoRetardoCpu;
int estimacionInicial;
int gradoMultiprocesamiento; // Cantidad de repartidores que pueden estar en Exec a la vez
char *algoritmoPlanificacion;

/* Queues */

t_queue *qR;
t_queue *qE;
t_queue *qB;
t_queue *qF;
int cantidadRepartidores; // Grado de multiprogramación o cantidad de instancias totales

/* Repartidores */

t_list *repartidores;

typedef struct {
	int freqDescanso; // Cantidad de ciclos de CPU que puede ejecutar seguidos
	int tiempoDescanso; // Cantidad de ciclos de CPU que debe descansar luego de ejecutar su frecuencia
	t_posicion *posRepartidor;
	// ?
} t_repartidor;











typedef struct appConfig {
    int gradoMultiprocesamiento;
    char* algoritmoPlanificacion;
    float alpha;
    int estimacionInicial;
    char** posicionRepartidores; //Por Commons se reciben las listas de esta forma. Recordar despues recorrer el char** y pasarlo a formato t_list*
    char** frecuenciaDescanso; //Por Commons se reciben las listas de esta forma. Recordar despues recorrer el char** y pasarlo a formato t_list*
    char** tiempoDescanso; //Por Commons se reciben las listas de esta forma. Recordar despues recorrer el char** y pasarlo a formato t_list*
	 //char* logFile;
    char** platosDefault;
    int posicionRestDefaultX;
    int posicionRestDefaultY;
} appConfig_t;

appConfig_t* appConfig;

typedef enum {
	NEW,
	READY,
	BLOCK,
	EXEC,
	EXIT
} Estado;

typedef struct repartidor
{
	int id;
	int posicionX;
	int posicionY;
    int frecuenciaDeDescanso;
    int tiempoDeDescanso;
	//t_pedido pedidoPCB; Falta armar la estructura del PCB
	Estado estado;
    float estimacionActual;
	//float estimacionAnterior;
	//int rafagaAnterior;
	//int rafagaActual;

} t_Repartidor;
t_Repartidor repartidor;

// LISTAS
t_list *restaurantes;
t_list *listaPedidos;
t_list *listaRepartidores;
t_list *NUEVOS;
t_list *LISTOS;
t_list *EJECUTANDO;
t_list *BLOQUEADOS;
t_list *FINALIZADOS;

// Defincion de funciones de APP
void cargarConfiguracionApp();
void inicializarEstructurasYListas();
void cargarPCByListas();
void planificar();
void planificarFIFO();
void planificarHRRN();
void planificarSJF();

#endif
