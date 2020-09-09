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

// Interfaz Cliente
void inicializarConsola();
void finalizarConsola();
void enviarConsultarRestaurantesAApp();
void enviarConsultarPlatosARestaurante();
void mostrarComandosValidos();

// Opciones consola

#define ERROR -1
#define APP 1
#define COMANDA 2
#define RESTAURANTE 3
#define SINDICATO 4
#define AIUDA 5
#define BAI 6
#define CLEAR 7

typedef struct {
    char *key;
    int valor;
} p_keys;

static p_keys diccionarioProcesos[] = {
    { "APP", APP },
    { "COMANDA", COMANDA },
    { "RESTAURANTE", RESTAURANTE },
    { "SINDICATO", SINDICATO },
    { "AIUDA", AIUDA },
    { "BAI", BAI },
    { "CLEAR", CLEAR }
};

#define NKEYS (sizeof(diccionarioProcesos)/sizeof(p_keys))

int stringAProceso(char *key)
{
    int i;
    for (i=0; i < NKEYS; i++) {
        p_keys sym = diccionarioProcesos[i];
        if (strcmp(sym.key, key) == 0)
            return sym.valor;
    }
    return ERROR;
}

#endif