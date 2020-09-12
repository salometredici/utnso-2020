#ifndef SINDICATO_H
#define SINDICATO_H

#include <commons/string.h>
#include <readline/readline.h>
#include "../../shared/include/shared_utils.h"
#include "tests.h"

int socketServidor;
pthread_t threadConsola;
pthread_t threadConexiones;

void inicializarConsola();
void finalizarConsola();
void enviarConsultarRestaurantesAApp();
void enviarConsultarPlatosARestaurante();
void mostrarComandosValidos();

// Mensajes por socket

static t_keys diccionarioMsgs[] = {
    { "CONSULTAR_PLATOS", CONSULTAR_PLATOS },
    { "GUARDAR_PEDIDO", GUARDAR_PEDIDO },
    { "GUARDAR_PLATO", GUARDAR_PLATO },
    { "CONFIRMAR_PEDIDO", CONFIRMAR_PEDIDO },
    { "OBTENER_PEDIDO", OBTENER_PEDIDO },
    { "OBTENER_RESTAURANTE", OBTENER_RESTAURANTE },
    { "PLATO_LISTO", PLATO_LISTO },
    { "TERMINAR_PEDIDO", TERMINAR_PEDIDO },
    { "OBTENER_RECETA", OBTENER_RECETA }
};

#define MSGSNKEYS (sizeof(diccionarioMsgs)/sizeof(t_keys))

// Consola

#define OPT_CREAR_RESTAURANTE 1
#define OPT_CREAR_RECETA 2
#define OPT_AIUDA 3
#define OPT_BAI 4
#define OPT_CLEAR 5
#define ERROR -1

static t_keys diccionarioConsola[] = {
    { "CREAR_RESTAURANTE", OPT_CREAR_RESTAURANTE },
    { "CREAR_RECETA", OPT_CREAR_RECETA },
    { "AIUDA", OPT_AIUDA },
    { "BAI", OPT_BAI },
    { "CLEAR", OPT_CLEAR }
};

#define CONSOLENKEYS (sizeof(diccionarioConsola)/sizeof(t_keys))

int stringAKey(char *key, int size) {
    t_keys *diccionario;
    if (size == CONSOLENKEYS) { diccionario = diccionarioConsola; }
    else { diccionario = diccionarioMsgs; }
    for (int i = 0; i < size; i++) {
        t_keys sym = diccionario[i];
        if (strcmp(sym.key, key) == 0) {
            return sym.valor;
        }
    }
    return ERROR;
}

#endif