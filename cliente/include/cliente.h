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

int conexion;
p_code procesoConectado;

t_list *params;

pthread_t threadConsola;

// API
void consultarRestaurantes();
void seleccionarRestaurante(int socketCliente, char *nombreRestaurante);
void obtenerRestaurante(char *nombreRestaurante);
void consultarPlatos(char *nombreRestaurante);
void crearPedido();
void guardarPedido(char *nombreRestaurante, int idPedido);
void aniadirPlato(char *nombrePlato, int idPedido);
void guardarPlato(char *nombreRestaurante, int idPedido, char *nombreComida, int cantidadPlatos);
void confirmarPedido(int idPedido, char *nombreRestaurante); 
void platoListo(char *nombreRestaurante, int idPedido, char *nombreComida);
void consultarPedido(int idPedido);
void obtenerPedido(char *nombreRestaurante, int idPedido);

//Listas 
 t_list *listaRestaurantes;

#endif