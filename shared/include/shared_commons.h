#ifndef SHARED_COMMONS_H
#define SHARED_COMMONS_H

#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/socket.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>

#define BASE_PATH "/utnso/tp-2020-2c-death-code/"
#define OK -7
#define ERROR -1

// Structs

typedef struct {
    int posX;
	int posY;
} t_posicion;

typedef struct {
    char *restaurante;
    int idPedido;
} t_req_pedido;

typedef struct {
	char *restaurante;
	int idPedido;
	char *plato;
	int cantidadPlato;
} t_req_plato;

typedef enum {
	PENDIENTE = 1,
	CONFIRMADO = 2,
	FINALIZADO = 3
} t_estado;

typedef struct {
	char *plato;
	int cantidadPedida;
	int cantidadLista;
	int precio;
} t_plato;

typedef struct { // Ir actualizando con erratas del TP! No debería tener el id también?
	t_estado estado;
	t_list *platos;
	int precioTotal; // Quizás después corresponda un float o double
} t_pedido;

void limpiarPantalla();

void mostrarListaStrings(t_list *listaStrings);
void mostrarListaPlatos(t_list *listaPlatos);

int calcularPrecioTotal(t_list *listaPlatos);
char *getStringEstadoPedido(t_estado estado);

#endif