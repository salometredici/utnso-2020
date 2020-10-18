#ifndef INIT_H
#define INIT_H

#include <commons/collections/list.h>
#include <stdlib.h>
#include <stdbool.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include "../../shared/include/shared_commons.h"
#include "../../shared/include/shared_core.h"

#define PAGE_SIZE 24 // por enunciado 

t_list * tables;
t_list * maps;

void ** MEMORY;
int MEMORY_SIZE;
int SWAP_SIZE;
char* ALGORITMO_REEMPLAZO;

typedef struct{
    int frame;
    int pid;
    char *restaurante;
}memoria;

typedef struct{
    uint32_t cantidad;
    uint32_t cantidadLista;
    char * comidas;
}frame;

void initStructures();
void initVariables();

#endif