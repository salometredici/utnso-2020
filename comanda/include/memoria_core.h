#ifndef MEMORIA_CORE_H
#define MEMORIA_CORE_COMANDA_H

#include "../../shared/include/shared_serialization.h"
#include "../../shared/include/shared_commons.h"
#include "../../shared/include/shared_core.h"
#include "../include/comanda_init.h"

/*SEMAFOROS*/
pthread_mutex_t memory_frames_bitarray;
pthread_mutex_t swap_frames_bitarray;

pthread_mutex_t mutex_paginas_en_memoria;
pthread_mutex_t mutex_asignar_pagina;

/*Tabla de segmentos*/
t_list *restaurantes;

t_bitarray *frame_usage_bitmap;
void* bitmap_pointer;

typedef enum {
    INICIAL,
    ENPROCESO,
    FINALIZADOP
}estado;

typedef struct{
    char *nombre;
    t_list *pedidos; /*Tabla de Paginas*/
    int estado;
} t_restaurante;

typedef struct {
    int id_pedido;
    t_list *platos; //platos
}t_pedidoc;

typedef struct{
    int *frame; //puntero a frame
	bool in_use; 
	bool modified;
    bool flag; 
} t_plato; // referencia a comanda c ver despues si se puede cambiar

/*Marco en la memoria*/
typedef struct{
    uint32_t cantidad;
    uint32_t cantidadLista;
    char *comida;
} t_frame;

t_restaurante *crear_restaurante(char *nombre_rest);
t_pedidoc *crear_pedido(int id_pedido);
t_restaurante *find_restaurante(char *nombre);
void add_pedido_to_restaurante(t_restaurante *restaurante, t_pedidoc *pedido);
t_pedidoc* find_pedido(t_restaurante *restaurante, int id)

#endif