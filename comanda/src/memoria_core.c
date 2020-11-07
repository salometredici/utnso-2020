#include "../include/memoria_core.h"

t_restaurante *crear_restaurante(char *nombre_rest)
{
    t_restaurante *restaurante = malloc(sizeof(restaurante));
    restaurante->nombre = nombre_rest;
    restaurante->pedidos = list_create();
    log_comanda("Se creo un restaurante.");
    list_add(restaurantes, restaurante);
	return restaurante;
}

t_pedidoc *crear_pedido(int id_pedido)
{
	t_pedidoc *pedido = malloc(sizeof(t_pedidoc));
	pedido->id_pedido = id_pedido;
	pedido->pages = list_create();
    pedido->estado = PENDIENTE; //veo si agrego otro estado 
	log_info(logger, "Se cre la tabla de Paginas para id_pedido-(%d) creado.", id_pedido);
	return pedido;
}

t_restaurante* find_restaurante(char *nombre){
	
    bool es_restaurante_buscado(void *elemento){
		t_restaurante *x = (t_restaurante*) elemento;
		return string_equals_ignore_case(x->nombre, nombre);
	}

	t_restaurante *rest = list_find(restaurantes,&es_restaurante_buscado);
	
	return rest;
}

void add_pedido_to_restaurante(t_restaurante *restaurante, t_pedidoc *pedido){
	list_add(restaurante->pedidos, pedido);
}

t_pedidoc* find_pedido(t_restaurante *restaurante, int id){
    bool _find_pedido(void *elemento){
		t_pedidoc *x = (t_pedidoc*) elemento;
		return x->id_pedido = id;
	}

	t_pedidoc *pedido = list_find(restaurante->pedidos,&_find_pedido);
	return pedido;	
}

char* find_plato_in_memory(char *comida, int frame_number){
	void* frame;

	frame = MEMORIA[frame_number];

	uint32_t cantidad;
	memcpy(&cantidad, frame + (PAGE_SIZE * frame_number), sizeof(uint32_t));
	uint32_t cantidad_lista;
	memcpy(&cantidad_lista, frame + (PAGE_SIZE * frame_number) + sizeof(uint32_t), sizeof(uint32_t));						
	char *plato_encontrado = malloc(size_char);
	memcpy(plato_encontrado, frame + (PAGE_SIZE * frame_number) + sizeof(uint32_t) + sizeof(uint32_t), size_char);

	return plato_encontrado; 
}

t_page* find_plato(t_pedidoc *pedido, char *plato){

	int size = list_size(pedido->pages);
	
	if(size > 0){ //se fija si hay algun plato cargado
		bool _find_plato(void* element){
			t_page *x = (t_page*)element;
			int frame_number = x->frame;
			char *plato_a_encontrar = find_plato_in_memory(plato, frame_number);
			return string_equals_ignore_case(plato, plato_a_encontrar);
		}

		t_page *plato = list_find(pedido->pages, &_find_plato);

		if(plato == NULL){
			return NULL;
		}

		return plato;
	}
	return NULL;
}

t_page* asignar_frame (char *plato, int cantidad){
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
		/*t_frame *plato_insertar = malloc(sizeof(t_frame)); --EXPERIMENTO 1 jej
		plato_insertar->cantidad = cantidad;
		plato_insertar->cantidad_lista = 0;
		plato_insertar->comida = plato;

		memcpy(frame, plato_insertar, sizeof(plato) + 1); //por las dudas ese /0
		free(plato_insertar);*/
		uint32_t value = 0;
		memcpy(frame + (PAGE_SIZE * frame_number), &cantidad, sizeof(uint32_t));
		memcpy(frame + (PAGE_SIZE * frame_number) + sizeof(uint32_t), &value, sizeof(uint32_t));
		memcpy(frame + (PAGE_SIZE * frame_number) + sizeof(uint32_t) + sizeof(uint32_t), plato, size_char);
		
		//esta en memoria ahora asignar a la tabla de paginas/platos
		t_page *new_plato = malloc(sizeof(t_page));
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

int find_free_bit(t_bitarray* bitmap, int limit) {
	for (int var = 0; var < limit; ++var) {
		bool is_used = bitarray_test_bit(bitmap, var);
		if (!is_used) {
			return var;
		}
	}
	return -1;
}

int find_free_frame() {
	pthread_mutex_lock(&memory_frames_bitarray);
	int free_bit = find_free_bit(frame_usage_bitmap, MEMORY_SIZE / PAGE_SIZE);
	pthread_mutex_unlock(&memory_frames_bitarray);
	return free_bit;
}

void clear_bitmap(t_bitarray* bitmap, int bits) {
	for (int var = 0; var < bits; var++) {
		bitarray_clean_bit(bitmap, var);
	}
}
