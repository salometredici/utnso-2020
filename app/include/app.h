#ifndef APP_H
#define APP_H

#include "../../shared/include/shared_commons.h"
#include "../../shared/include/shared_core.h"
#include "../../shared/include/shared_serialization.h"
#include "tests.h"

int conexionComanda;
int socketServidor;
pthread_t threadConexiones;

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
