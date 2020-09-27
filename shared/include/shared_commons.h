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

void limpiarPantalla();
void mostrarListaStrings(t_list *listaStrings);
int calcularPrecioTotal(t_list *listaPlatos);

typedef struct {
	char *plato;
	int cantidadPedida;
	int cantidadLista;
	int precio;
} t_plato;

#endif