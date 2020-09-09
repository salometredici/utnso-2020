#ifndef SINDICATO_H
#define SINDICATO_H
#include <stdio.h>
#include <commons/log.h>
#include <stdbool.h>
#include <pthread.h>
#include "../../shared/include/shared_utils.h"
#include "tests.h"
#include <commons/string.h>
#include <readline/readline.h>


void inicializarConsola();
void finalizarConsola();
void enviarConsultarRestaurantesAApp();
void enviarConsultarPlatosARestaurante();
void mostrarComandosValidos();

pthread_t threadConsola;

//consola
#define ERROR -1
#define CREARRESTAURANTE 1
#define CREARRECETA 2
#define AIUDA 3
#define BAI 4
#define CLEAR 5

typedef struct {
    char *key;
    int valor;
} p_keys;

static p_keys diccionarioProcesos[] = {
    { "CREARRESTAURANTE", CREARRESTAURANTE },
    { "CREARRECETA", CREARRECETA },
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