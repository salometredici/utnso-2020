#include "../include/memoria_core.h"

t_restaurante *crear_restaurante(char *nombre_rest)
{
    t_restaurante *restaurante = malloc(sizeof(restaurante));
    restaurante->nombre = nombre_rest;
    restaurante->pedidos = list_create();
    log_comanda("Se creo un restaurante.");
    list_add(restaurantes, restaurante);
}

t_pedidoc *crear_pedido(int id_pedido)
{
	t_pedidoc *pedido = malloc(sizeof(t_pedidoc));
	pedido->id_pedido = id_pedido;
	pedido->platos = list_create();
    pedido->estado = PENDIENTE; //veo si agrego otro estado 
	log_info(logger, "Se cre la tabla de Paginas para id_pedido-(%d) creado.", id_pedido);
	return pedido;
}

t_restaurante* find_restaurante(char *nombre)
{
    bool es_restaurante_buscado(void *elemento)	{
		t_restaurante *x = (t_restaurante*) elemento;
		return string_equals_ignore_case(x->nombre, nombre);
	}

	t_restaurante *rest = list_find(restaurantes,&es_restaurante_buscado)
	return rest;
}

void add_pedido_to_restaurante(t_restaurante *restaurante, t_pedidoc *pedido){
	list_add(restaurante->pedidos, pedido);
}

t_pedidoc* find_pedido(t_restaurante *restaurante, int id){
    bool _find_pedido(void *elemento)	{
		t_pedidoc *x = (t_pedidoc*) elemento;
		return x->id_pedido = id;
	}

	t_pedidoc *pedido = list_find(restaurante->pedidos,&_find_pedido)
	return pedido;	
}

t_plato* find_plato(t_pedidoc *pedido, char *plato){

	int size = list_size(pedido->platos);
	
	if(size > 0){ //se fija si hay algun plato cargado
		void* frame;

		bool _find_plato(void* element){
			t_plato *x = (t_plato*) element;
			frame = MEMORIA[t_plato->frame];
			char *plato_encontrado;
			memcpy(&plato_encontrado, frame + sizeof(uint32_t) + sizeof(uint32_t), 24);
			return string_equals_ignore_case(plato_encontrado, plato);
		}

		t_plato *plato = list_find(pedido->platos, &_find_plato);

		if(t_plato == NULL){
			return NULL;
		}

		return plato;
	}
	return NULL;
}

t_plato* asignar_frame (char *plato, int cantidad){
	pthread_mutex_lock(&mutex_asignar_pagina);
	int frame_number = find_free_frame_memory();
	void *frame;
	if(frame_number == -1){
		log_comanda("No hay espacio en la memoria.. hacer swap");
		//Logica de area de swap
	}
	else{
		//primero alloco en la memoria
		frame = MEMORIA[frame_number];
		t_frame *plato_insertar = malloc(sizeof(t_frame));
		plato_insertar->cantidad = cantidad;
		plato_insertar->cantidad_lista = 0;
		plato_insertar->plato = plato;
		memcpy(frame, plato_insertar, sizeof(plato) + 1); //por las dudas ese /0
		free(plato_insertar);

		//esta en memoria ahora asignar a la tabla de paginas/platos
		t_plato *new_plato = malloc(sizeof(t_plato));
		new_plato->frame = frame_number;
		new_plato->in_use = 0;
		new_plato->flag = 1;
		new_plato->modified = 0;

		bitarray_set_bit(frame_usage_bitmap, frame_number);	
		pthread_mutex_unlock(&mutex_asignar_pagina);
		return new_plato;
	}
}

int find_free_frame_memory() {
	pthread_mutex_lock(&memory_frames_bitarray);
	int free_bit = find_free_bit(frame_usage_bitmap, MEMORY_SIZE / PAGE_SIZE);
	pthread_mutex_unlock(&memory_frames_bitarray);
	return free_bit;
}




