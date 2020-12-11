#ifndef MEMORIA_CORE_H
#define MEMORIA_CORE_COMANDA_H

#include "../../shared/include/shared_serialization.h"
#include "../../shared/include/shared_commons.h"
#include "../../shared/include/shared_core.h"
#include "../../shared/include/shared_logging.h"
#include <commons/bitarray.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>


#define PAGE_SIZE 32 // por enunciado 
#define MAX_LENGTH_COMIDA 24
#define RUTASWAP "/home/utnso/swap.txt"
#define IN_MEMORY 1
#define IN_SWAP 0
#define PLATO_TERMINADO 2
#define PLATO_NO_TERMINADO 3 
#define LRU "LRU"
#define CLOCK "CLOCK_MEJ"
#define VISTIMA_NO_ENCONTRADA 5

/*SEMAFOROS*/
pthread_mutex_t memory_frames_bitarray;
pthread_mutex_t swap_frames_bitarray;

pthread_mutex_t mutex_paginas_en_memoria;
pthread_mutex_t mx_find_frame_in_memory;
pthread_mutex_t mx_find_frame_in_swap;
pthread_mutex_t mutex_asignar_pagina;
pthread_mutex_t write_memory;

/*Tabla de segmentos*/
t_list *restaurantes;

t_bitarray *frame_usage_bitmap;
void* bitmap_pointer;

t_bitarray *swap_usage_bitmap;
void* swap_bitmap_pointer;

void* archivo_swap;
int fd_swap;

void **MEMORIA;
int MEMORY_SIZE;
int SWAP_SIZE;
char* SWAP_FILE;
FILE *swap_file;
char *ALGORITMO_REEMPLAZO;
int frames;
int swap_frames;
size_t size_char;
int puntero_clock;

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
    int frame_mv; //el frame que esta en el area de swap
	bool in_use; 
	bool modified;
    bool flag; //1 en memoria principal
    double timestamp;
}t_page; //plato conflict types

/*Marco en la memoria*/
typedef struct{
    uint32_t cantidad_pedida;
    uint32_t cantidad_lista;
    char *comida;
} t_frame;

t_restaurante* crear_restaurante(char* nombre_rest);
t_pedidoc* crear_pedido(int id_pedido);
t_restaurante* find_restaurante(char* nombre);
void add_pedido_to_restaurante(t_restaurante* restaurante, t_pedidoc* pedido);
t_pedidoc* find_pedido(t_restaurante *restaurante, int id);
t_page* find_plato(t_pedidoc* pedido, char *plato);
t_page* asignar_frame(char* plato, int cantidad);
t_list* find_frames(t_pedidoc* pedido);
bool increase_cantidad_plato(t_page* page, int new_cantidad_plato);
int update_cantidad_lista(t_page* page);
void free_pages(t_list* pages);
void delete_pedido_from_restaurant(t_list* pedidos, int nro_pedido);

void print_swap();
void print_memory();
void print_pages_in_memory();

t_frame* get_frame_from_swap(int frame_swap);
void clear_bitmap(t_bitarray* bitmap, int bits);
int find_victim_and_bring_it_to_mp(t_page* page);
t_page* find_victim_clock();
t_page* find_victim_lru();

#endif