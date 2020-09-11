#ifndef CLIENTE_H_
#define CLIENTE_H_

#include <stdlib.h>
#include <pthread.h>
#include <commons/string.h>
#include <readline/readline.h>
#include "../../shared/include/shared_utils.h"
#include "tests.h"

int conexionApp;
int conexionComanda;
int conexionRestaurante;
int conexionSindicato;

pthread_t threadConsola;

/////////////// INTERFAZ DE CLIENTE ///////////////////
///// General ////
void inicializarConsola();
void finalizarConsola();
void mostrarComandosValidos();

// Mensajes de Ciente al modulo APP (1)
void consultarRestaurantesAapp();
void seleccionarRestauranteAapp();
void consultarPlatosAapp();
void crearPedidoAapp();
void aniadirPlatoAapp();
void confirmarPedidoAapp();
void platoListoAapp();
void consultarPedidoAapp();

// Mensajes de Ciente al modulo COMANDA(2)
void guardarPedidoAComanda();
void guardarPlatoAComanda();
void confirmarPedidoAComanda();
void platoListoAComanda();
void obtenerPedidoAComanda();

// Mensajes de Ciente al modulo RESTAURANTE (3)
void consultarPlatosARestaurante();
void crearPedidoARestaurante();
void aniadirPlatoARestaurante();
void confirmarPedidoARestaurante();
void consultarPedidoARestaurante();

// Mensajes de Ciente al modulo SINDICATO (4)
void obtenerRestauranteASindicato();
void consultarPlatosASindicato();
void guardarPedidoASindicato();
void guardarPlatoASindicato();
void confirmarPedidoASindicato();
void platoListoASindicato();
void obtenerPedidoASindicato();



// Opciones - consola

#define ERROR -1
#define APP 1
#define COMANDA 2
#define RESTAURANTE 3
#define SINDICATO 4
#define AIUDA 5
#define BAI 6
#define CLEAR 7

// Opciones - comandos

#define CONSULTAR_RESTAURANTES 11
#define SELECCIONAR_RESTAURANTE 12
#define OBTENER_RESTAURANTE 13
#define CONSULTAR_PLATOS 14
#define CREAR_PEDIDO 15
#define GUARDAR_PEDIDO 16
#define ANIADIR_PLATO 17
#define GUARDAR_PLATO 18
#define CONFIRMAR_PEDIDO 19
#define PLATO_LISTO 20
#define CONSULTAR_PEDIDO 21
#define OBTENER_PEDIDO 22
#define FINALIZAR_PEDIDO 23
#define TERMINAR_PEDIDO 24
#define OBTENER_RECETA 25

typedef struct {
    char *key;
    int valor;
} t_keys;

static t_keys diccionarioProcesos[] = {
    { "APP", APP },
    { "COMANDA", COMANDA },
    { "RESTAURANTE", RESTAURANTE },
    { "SINDICATO", SINDICATO },
    { "AIUDA", AIUDA },
    { "BAI", BAI },
    { "CLEAR", CLEAR }
};

#define CONSOLENKEYS (sizeof(diccionarioProcesos)/sizeof(t_keys))

static t_keys diccionarioComandos[] = {
    { "CONSULTAR_RESTAURANTES", CONSULTAR_RESTAURANTES },
    { "SELECCIONAR_RESTAURANTE", SELECCIONAR_RESTAURANTE },
    { "OBTENER_RESTAURANTE", OBTENER_RESTAURANTE },
    { "CONSULTAR_PLATOS", CONSULTAR_PLATOS },
    { "CREAR_PEDIDO", CREAR_PEDIDO },
    { "GUARDAR_PEDIDO", GUARDAR_PEDIDO },
    { "ANIADIR_PLATO", ANIADIR_PLATO },
    { "GUARDAR_PLATO", GUARDAR_PLATO },
    { "CONFIRMAR_PEDIDO", CONFIRMAR_PEDIDO },
    { "PLATO_LISTO", PLATO_LISTO },
    { "CONSULTAR_PEDIDO", CONSULTAR_PEDIDO },
    { "OBTENER_PEDIDO", OBTENER_PEDIDO },
    { "FINALIZAR_PEDIDO", FINALIZAR_PEDIDO },
    { "TERMINAR_PEDIDO", TERMINAR_PEDIDO },
    { "OBTENER_RECETA", OBTENER_RECETA }
};

#define COMMANDNKEYS (sizeof(diccionarioComandos)/sizeof(t_keys))

int stringAKey(char *key, int size) {
    t_keys *diccionario;
    if (size == CONSOLENKEYS) { diccionario = diccionarioProcesos; }
    else { diccionario = diccionarioComandos; }
    for (int i = 0; i < size; i++) {
        t_keys sym = diccionario[i];
        if (strcmp(sym.key, key) == 0) {
            return sym.valor;
        }
    }
    return ERROR;
}

#endif