#ifndef CLIENTE_H_
#define CLIENTE_H_

#include <stdlib.h>
#include <pthread.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "../../shared/include/shared_utils.h"
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

// Mensajes Cliente - APP

void consultarRestaurantesAapp();
void seleccionarRestauranteAapp(char *nombreCliente, char *nombreRestaurante);
void consultarPlatosAapp(char *nombreRestaurante);
void crearPedidoAapp(); 
void aniadirPlatoAapp(char *nombrePlato, char* idPedido);
void confirmarPedidoAapp(char *idPedido); 
void platoListoAapp(char *nombreRestaurante, char *idPedido, char *nombreComida);
void consultarPedidoAapp(char *idPedido);

// Mensajes Clente - COMANDA

void guardarPedidoAComanda(char *nombreRestaurante, char *idPedido);
void guardarPlatoAComanda(char *nombreRestaurante, char *idPedido, char *nombreComida, char *cantidadPlatos);
void confirmarPedidoAComanda(char *idPedido); 
void platoListoAComanda(char *nombreRestaurante, char *idPedido, char *nombreComida);
void obtenerPedidoAComanda(char *nombreRestaurante, char *idPedido);

// Mensajes Cliente - RESTAURANTE

void consultarPlatosARestaurante(char *nombreRestaurante);
void crearPedidoARestaurante();
void aniadirPlatoARestaurante(char *nombrePlato, char *idPedido);
void confirmarPedidoARestaurante(char *idPedido);
void consultarPedidoARestaurante(char *idPedido);

// Mensajes Cliente - SINDICATO

void obtenerRestauranteASindicato(char *nombreRestaurante);
void consultarPlatosASindicato(char *nombreRestaurante);
void guardarPedidoASindicato(char *nombreRestaurante, char *idPedido);
void guardarPlatoASindicato(char *nombreRestaurante, char *idPedido, char *nombreComida, int cantidadPlatos);
void confirmarPedidoASindicato(char *idPedido);
void platoListoASindicato(char *nombreRestaurante, char *idPedido, char *nombreComida);
void obtenerPedidoASindicato(char *nombreRestaurante, char *idPedido);

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