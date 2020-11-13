#ifndef MEMORIA_CORE_H
#define MEMORIA_CORE_COMANDA_H

#include "../../shared/include/shared_serialization.h"
#include "../../shared/include/shared_commons.h"
#include "../../shared/include/shared_core.h"
#include <commons/bitarray.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>

#define PAGE_SIZE 32 // por enunciado 
#define MAX_LENGTH_COMIDA 24

/*SEMAFOROS*/
pthread_mutex_t memory_frames_bitarray;
pthread_mutex_t swap_frames_bitarray;

pthread_mutex_t mutex_paginas_en_memoria;
pthread_mutex_t mutex_asignar_pagina;

/*Tabla de segmentos*/
t_list *restaurantes;

t_bitarray *frame_usage_bitmap;
void* bitmap_pointer;

t_bitarray *swap_usage_bitmap;
void* swap_bitmap_pointer;

void **MEMORIA;
int MEMORY_SIZE;
int SWAP_SIZE;
char* SWAP_FILE;
FILE *swap_file;
char *ALGORITMO_REEMPLAZO;
int frames;
size_t size_char;

typedef struct{
    char *nombre;
    t_list *pedidos; /*Tabla de Paginas*/
} t_restaurante;

typedef struct {
    t_estado estado;
    int id_pedido;
    t_list *pages; //paginas
}t_pedidoc; // referencia a comanda c ver despues si se puede cambiar

typedef struct{
    int frame; 
	bool in_use; 
	bool modified;
    bool flag; //1 en memoria principal
}t_page; //plato conflict types

/*Marco en la memoria*/
typedef struct{
    uint32_t cantidad_pedida;
    uint32_t cantidad_lista;
    char *comida;
} t_frame;

t_restaurante *crear_restaurante(char *nombre_rest);
t_pedidoc *crear_pedido(int id_pedido);
t_restaurante *find_restaurante(char *nombre);
void add_pedido_to_restaurante(t_restaurante *restaurante, t_pedidoc *pedido);
t_pedidoc* find_pedido(t_restaurante *restaurante, int id);
t_page* find_plato(t_pedidoc *pedido, char *plato);
t_page* asignar_frame(char *plato, int cantidad);
t_list* find_frames(t_pedidoc *pedido);
#endif