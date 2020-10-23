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
int id_segment = 0;

/*
 * Estructura de lo que esta dentro de la memoria
 */
typedef struct{
    uint32_t cantidad;
    uint32_t cantidadLista;
    char *comidas;
} t_frame;

typedef struct{
    char *name;
    uint32_t idsegment; // necesario ??
    t_list *pedidos;
} t_segment;

/*
 * Estructura de lo una tabla de Paginas
 */
typedef struct{
    char *nombre;
    int id_pedido;
    t_list *frames;
}t_pedido;

void init_sructures();
void init_variables();
void init_tables();
void print_structure();
void add_restaurant(char *nombre);

#endif