#ifndef COMANDA_INIT_H
#define COMANDA_INIT_H

#include "../../shared/include/shared_commons.h"
#include "../../shared/include/shared_core.h"
#include "../../shared/include/shared_serialization.h"
#include "tests.h"

#define PAGE_SIZE 32 // por enunciado 

t_list *memory;
t_list *maps;
t_list *restaurantes;

void **MEMORY;
int MEMORY_SIZE;
int SWAP_SIZE;
char *ALGORITMO_REEMPLAZO;
int id_segment;

typedef struct{
    uint32_t cantidad;
    uint32_t cantidadLista;
    char *comidas;
} t_frame;

typedef struct{
    char *name;
    uint32_t idsegment;
    int idpedido;
    void *pages;
} t_segment;

typedef struct{
    int frameNumber;
    int plato;
} t_page;

void initStructures();
void initVariables();
void initTables();
void imprimirEstructura();

#endif