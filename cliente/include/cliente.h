#ifndef CLIENTE_H_
#define CLIENTE_H_

#include <stdlib.h>
#include <pthread.h>
#include <commons/string.h>
#include <readline/history.h>
#include <readline/readline.h>
#include "../../shared/include/shared_commons.h"
#include "../../shared/include/shared_core.h"
#include "../../shared/include/shared_serialization.h"
#include "../include/client_console.h"
#include "tests.h"

int conexionApp;
int conexionComanda;
int conexionRestaurante;
int conexionSindicato;

t_list *params;

pthread_t threadConsola;

// API
void consultarRestaurantes();
void seleccionarRestaurante(int socketCliente, char *nombreRestaurante);
void obtenerRestaurante(char *nombreRestaurante);
void consultarPlatos(int conexion, char *nombreRestaurante);
void crearPedido(int conexion); 
void guardarPedido(int conexion, char *nombreRestaurante, int idPedido);
void aniadirPlato(int conexion, char *nombrePlato, int idPedido);
void guardarPlato(int conexion, char *nombreRestaurante, int idPedido, char *nombreComida, int cantidadPlatos);
void confirmarPedido(int conexion, int idPedido, char *nombreRestaurante); 
void platoListo(int conexion, char *nombreRestaurante, int idPedido, char *nombreComida);
void consultarPedido(int conexion, int idPedido);
void obtenerPedido(int conexion, char *nombreRestaurante, int idPedido);

//Listas 
 t_list *listaRestaurantes;

#endif