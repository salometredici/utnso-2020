#ifndef CLIENTE_H_
#define CLIENTE_H_

#include <stdlib.h>
#include <pthread.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "../../shared/include/shared_commons.h"
#include "../../shared/include/shared_core.h"
#include "../../shared/include/shared_serialization.h"
#include "tests.h"

int conexionApp;
int conexionComanda;
int conexionRestaurante;
int conexionSindicato;

t_list *params;

pthread_t threadConsola;

void inicializarConsola();
void finalizarConsola();
void mostrarComandosValidos();

// API
void consultarRestaurantes();
void seleccionarRestaurante(char *nombreCliente, char *nombreRestaurante);
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

// Diccionario de opciones/módulos para la consola de Cliente

#define OPTION_APP 1
#define OPTION_COMANDA 2
#define OPTION_RESTAURANTE 3
#define OPTION_SINDICATO 4
#define OPTION_AIUDA 5
#define OPTION_BAI 6
#define OPTION_CLEAR 7

static t_keys diccionarioOpciones[] = {
    { "APP", OPTION_APP },
    { "COMANDA", OPTION_COMANDA },
    { "RESTAURANTE", OPTION_RESTAURANTE },
    { "SINDICATO", OPTION_SINDICATO },
    { "AIUDA", OPTION_AIUDA },
    { "BAI", OPTION_BAI },
    { "CLEAR", OPTION_CLEAR }
};

#define CONSOLENKEYS (sizeof(diccionarioOpciones)/sizeof(t_keys))

int clientOptionToKey(char *key) {
    t_keys *diccionario = diccionarioOpciones;
    for (int i = 0; i < CONSOLENKEYS; i++) {
        t_keys sym = diccionario[i];
        if (strcmp(sym.key, key) == 0) {
            return sym.valor;
        }
    }
    return ERROR;
}

#endif