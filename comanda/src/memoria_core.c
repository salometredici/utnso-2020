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
		return x->id_pedido == id;
	}

	t_pedidoc *pedido = list_find(restaurante->pedidos,&_find_pedido);
	return pedido;	
}

double get_current_time(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	unsigned long long result = (((unsigned long long)tv.tv_sec) * 1000 + ((unsigned long long)tv.tv_usec) / 1000);
	double a = result;
	return a;
}

void escribir_swap(char* nombre_plato, int cantidad_pedida, int cantidad_lista, int page_swap){
	int offset = 0;

	void *contenido = malloc(PAGE_SIZE);
	memcpy(contenido, &cantidad_pedida, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(contenido + offset, &cantidad_lista, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(contenido + offset, nombre_plato, size_char);

	int offset_swap = page_swap * PAGE_SIZE;
	memcpy(archivo_swap + offset_swap, contenido, PAGE_SIZE);
	msync(archivo_swap, PAGE_SIZE, MS_SYNC);
}

void* get_content(int frame){
	void* contenido = malloc(PAGE_SIZE);
	int offset = frame * PAGE_SIZE;
	memcpy(contenido, archivo_swap + offset, PAGE_SIZE);
	return contenido;
}

t_list* paginas_en_memoria() {
	t_list* op(t_list* acum, t_restaurante* restaurante) {
		t_list* paginas = list_create();
		t_page* pagina;

		for(int i = 0; i < list_size(restaurante->pedidos); i++){
			t_pedidoc* pedido = list_get(restaurante->pedidos);

			for(int j = 0; j < list_size(pedido->pages); j++){
				t_page* page = list_get(pedido->pages, j);
				if(page->flag == 1){
					list_add(paginas, page);
				}
			}
		}

		list_add_all(acum, paginas);
		return acum;
	}
	pthread_mutex_lock(&mutex_paginas_en_memoria);
	t_list* paginas = list_fold(restaurantes, list_create(), (void*) op);
	pthread_mutex_unlock(&mutex_paginas_en_memoria);
	return paginas;
}

t_page* find_frame_victim(){
	//tengo que recorrer la tabla de paginas 
	//primero fijarme si hay un espacio libre en la mp
	t_page* victim_page = malloc(sizeof(t_page));

	for(int i = 0; i < list_size(restaurantes); i++){
		t_restaurante* x = list_get(restaurantes, i);
		
		for(int j = 0; j < list_size(x->pedidos); j++){
			t_pedidoc* pedido = list_get(x->pedidos, j);
			
			for(int h = 0; h < list_size(pedido->pages); h++){
				t_page* page = list_get(pedido->pages, h);
				if(page->flag == 1 && page->timestamp == 0 || page->timestamp > victim_page->timestamp){
					victim_page->frame = page->frame;
					victim_page->frame_mv = page->frame_mv;
					victim_page->flag = page->flag;//deberia de estar en memoria principal
					victim_page->in_use = page->in_use;
					victim_page->modified = page->modified;
					victim_page->timestamp = page->timestamp;
				}
			}
		}
	}

	return victim_page;
}

t_frame* find_frame_in_memory(t_page* page){

	if(page->flag == 1){
		void* frame;

		frame = MEMORIA[page->frame];

		t_frame *marco = malloc(sizeof(t_frame));

		uint32_t cantidad;
		memcpy(&cantidad, frame + (PAGE_SIZE * page->frame), sizeof(uint32_t));
		uint32_t cantidad_lista;
		memcpy(&cantidad_lista, frame + (PAGE_SIZE * page->frame) + sizeof(uint32_t), sizeof(uint32_t));						
		char *plato_encontrado = malloc(size_char);
		memcpy(plato_encontrado, frame + (PAGE_SIZE * page->frame) + sizeof(uint32_t) + sizeof(uint32_t), size_char);

		marco->cantidad_pedida = cantidad;
		marco->cantidad_lista = cantidad_lista;
		marco->comida = plato_encontrado;	
		return marco; 
	}else{
		t_page* frame_victima = find_frame_victim();
	}
}

t_list* find_frames(t_pedidoc *pedido){
	t_list* platos = list_create();

	int size = list_size(pedido->pages);
	for(int i = 0; i < size; i++){
		t_page *page = list_get(pedido->pages, i);

		if(page->flag == 1){
			t_frame *frame = find_frame_in_memory(page);
			list_add(platos, frame);
		}
		else{
			void* buffer = get_content(page->frame);
			int desplazamiento = 0;

			uint32_t cantidad;
			memcpy(&cantidad, buffer, sizeof(uint32_t));
			uint32_t cantidad_lista;
			desplazamiento += sizeof(uint32_t);
			memcpy(&cantidad_lista, buffer + desplazamiento, sizeof(uint32_t));
			desplazamiento += sizeof(uint32_t);						
			char *plato_encontrado = malloc(size_char);
			memcpy(plato_encontrado, buffer + desplazamiento, size_char);
		
			t_frame *marco = malloc(sizeof(t_frame));
			marco->cantidad_pedida = cantidad;
			marco->cantidad_lista = cantidad_lista;
			marco->comida = plato_encontrado;

			list_add(platos, marco);
		}
	}
	return platos;
}

t_page* find_plato(t_pedidoc *pedido, char *plato){

	int size = list_size(pedido->pages);
	
	if(size > 0){ //se fija si hay algun plato cargado
		bool _find_plato(void* element){
			t_page *x = (t_page*)element;
			int frame_number = x->frame;
			t_frame *plato_a_encontrar = find_frame_in_memory(frame_number, x->flag);
			return string_equals_ignore_case(plato, plato_a_encontrar->comida);
		}

		t_page *plato = list_find(pedido->pages, &_find_plato);

		if(plato == NULL){
			return NULL;
		}

		return plato;
	}
	return NULL;
}

t_page* asignar_frame (char *nombre_plato, int cantidad_pedida){
	pthread_mutex_lock(&mutex_asignar_pagina);
	int frame_number = find_free_frame_memory();
	void *frame;
	if(1){
		log_comanda("No hay espacio en la memoria.. hacer swap");
		int swap_frame = find_free_swap_frame();
		escribir_swap(nombre_plato, cantidad_pedida, 0, swap_frame);

		t_page *new_plato = malloc(sizeof(t_page));
		new_plato->frame = frame_number;
		new_plato->in_use = 0;
		new_plato->flag = 0;
		new_plato->modified = 0;
		new_plato->timestamp = get_current_time();
		new_plato->frame_mv = swap_frame;

		pthread_mutex_unlock(&mutex_asignar_pagina);
	}
	else{
		//Aloco en la memoria es igual que serializar un mensaje
		frame = MEMORIA[frame_number];

		uint32_t cantidad_lista = 0;
		memcpy(frame + (PAGE_SIZE * frame_number), &cantidad_pedida, sizeof(uint32_t));
		memcpy(frame + (PAGE_SIZE * frame_number) + sizeof(uint32_t), &cantidad_lista, sizeof(uint32_t));
		memcpy(frame + (PAGE_SIZE * frame_number) + sizeof(uint32_t) + sizeof(uint32_t), nombre_plato, size_char);
		
		//esta en memoria ahora asignar a la tabla de paginas/platos
		t_page *new_plato = malloc(sizeof(t_page));
		new_plato->frame = frame_number;
		new_plato->in_use = 0;
		new_plato->flag = 0;
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

int find_free_swap_frame(){
	pthread_mutex_lock(&swap_frames_bitarray);
	int free_bit = find_free_bit(swap_usage_bitmap, MEMORY_SIZE / PAGE_SIZE);
	pthread_mutex_unlock(&swap_frames_bitarray);
	return free_bit;	
}

void clear_bitmap(t_bitarray* bitmap, int bits) {
	for (int var = 0; var < bits; var++) {
		bitarray_clean_bit(bitmap, var);
	}
}
