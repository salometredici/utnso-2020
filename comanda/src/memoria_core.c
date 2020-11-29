#include "../include/memoria_core.h"

t_list* create_list(){
	t_list* aux = list_create();
	return aux;
}

t_restaurante *crear_restaurante(char *nombre_rest){
    t_restaurante *restaurante = malloc(sizeof(restaurante));
    restaurante->nombre = nombre_rest;
    restaurante->pedidos = create_list();
    log_comanda("Se creo un restaurante.");
    list_add(restaurantes, restaurante);
	print_restaurante();
	return restaurante;
}

t_pedidoc *crear_pedido(int id_pedido){
	t_pedidoc *pedido = malloc(sizeof(t_pedidoc));
	pedido->id_pedido = id_pedido;
	pedido->pages = create_list();
    pedido->estado = PENDIENTE; //veo si agrego otro estado 
	log_info(logger, "Se cre la tabla de Paginas para id_pedido-(%d) creado.", id_pedido);
	return pedido;
}

int find_free_bit(t_bitarray* bitmap, int limit) {
	for (int var = 0; var < limit; var++) {
		bool is_used = bitarray_test_bit(bitmap, var);
		if (!is_used) {
			return var;
		}
	}
	return -1;
}

int find_free_frame_memory() {
	pthread_mutex_lock(&memory_frames_bitarray);
	int free_bit = find_free_bit(frame_usage_bitmap, MEMORY_SIZE / PAGE_SIZE);
	pthread_mutex_unlock(&memory_frames_bitarray);
	return free_bit;
}

int find_free_swap_frame(){
	pthread_mutex_lock(&swap_frames_bitarray);
	int free_bit = find_free_bit(swap_usage_bitmap, SWAP_SIZE / PAGE_SIZE);
	pthread_mutex_unlock(&swap_frames_bitarray);
	return free_bit;	
}

void print_status_bitmap(t_bitarray* bitmap){
	int available_blocks = get_available_blocks_number();
	int lastBit = bitarray_get_max_bit(bitmap);
	printf("Tamaño del bitmap: %d"BREAK, lastBit);
	printf("Cant. de bloques disponibles: %d"BREAK, available_blocks);
	printf("Valor del primer bit: %d"BREAK, bitarray_test_bit(bitmap, 0));
	printf("Valor del último bit: %d"BREAK, bitarray_test_bit(bitmap, lastBit));
}

void clear_bitmap(t_bitarray* bitmap, int bits) {
	for (int var = 0; var < bits; var++) {
		bitarray_clean_bit(bitmap, var);
	}
	//print_status_bitmap(bitmap);
}

t_frame* get_frame_from_memory(int frame_number){
	void* frame;

	frame = MEMORIA[frame_number];

	t_frame *marco = malloc(sizeof(t_frame));

	uint32_t cantidad;
	memcpy(&cantidad, frame, sizeof(uint32_t));
	uint32_t cantidad_lista;
	memcpy(&cantidad_lista, frame + sizeof(uint32_t), sizeof(uint32_t));						
	char *plato_encontrado = malloc(size_char);
	memcpy(plato_encontrado, frame + sizeof(uint32_t) + sizeof(uint32_t), size_char);

	marco->cantidad_pedida = cantidad;
	marco->cantidad_lista = cantidad_lista;
	marco->comida = plato_encontrado;		

	return marco;
}

void print_restaurante(){
	printf("--------------------------------RESTAURANTES------------------------------\n");
	for(int i = 0; i < list_size(restaurantes); i++){
		t_restaurante* rest= list_get(restaurantes, i);
		if(rest)
			printf("| Indice: %d | Nombre del restaurante: %s \n", i, rest->nombre);
		//free(rest);
	}	
}

void print_pedidos(t_restaurante* rest){
	printf("----------------------------------PEDIDOS--------------------------------\n");
	for(int i = 0; i < list_size(rest->pedidos); i++){
		t_pedidoc* pedido = list_get(rest->pedidos, i);
		if(pedido)
			printf("| Indice: %d | Pedido ID : %d \n", i, pedido->id_pedido);
		free(pedido);
	}		
}

t_frame* find_frame_in_memory(t_page* page){
	if(page == NULL){
		printf("Hubo un problema");
		return NULL;
	}

	if(page->flag == IN_MEMORY){
		t_frame *frame = get_frame_from_memory(page->frame);	
		page->timestamp = get_current_time();
		return frame; 
	}
	else{
		int frame_victim = find_victim_and_bring_it_to_mp(page);

		page->flag = 1;
		page->timestamp = get_current_time();
		page->in_use = 1;
		page->modified = 0;
		page->frame = frame_victim;
		
		t_frame* frame = find_frame_in_memory(page);
		
		return frame;
	}
}

t_page* create_page(int frame_in_mp, int frame_swap){
	t_page *new_plato = malloc(sizeof(t_page));
	new_plato->frame = frame_in_mp;
	new_plato->in_use = 1;
	new_plato->flag = 1;
	new_plato->modified = 0;
	new_plato->timestamp = get_current_time();
	new_plato->frame_mv = frame_swap;

	return new_plato;		
}

t_restaurante* find_restaurante(char *nombre){	
    bool es_restaurante_buscado(void *elemento){
		t_restaurante *x = (t_restaurante*) elemento;
		return string_equals_ignore_case(x->nombre, nombre);
	};

	t_restaurante *rest = list_find(restaurantes,&es_restaurante_buscado);
	
	//print_restaurante();
	return rest;
}

void add_pedido_to_restaurante(t_restaurante *restaurante, t_pedidoc *pedido){
	list_add(restaurante->pedidos, pedido);
}

t_pedidoc* find_pedido(t_restaurante *restaurante, int id){
    bool _find_pedido(void *elemento){
		t_pedidoc *x = (t_pedidoc*) elemento;
		return x->id_pedido == id;
	};

	t_pedidoc *pedido = list_find(restaurante->pedidos,&_find_pedido);
	return pedido;	
}

void delete_pedido_from_restaurant(t_list* pedidos, int nro_pedido){
	for(int i = 0; i < list_size(pedidos); i++){
		t_pedidoc* pedido = list_get(pedidos, i);

		if(pedido->id_pedido == nro_pedido){
			list_remove_and_destroy_element(pedidos, i, &free);
		}
	}
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
	free(contenido);

	// printf("---------------------Escrimos en disco\n");
	//print_status_bitmap(swap_usage_bitmap);
	bitarray_set_bit(swap_usage_bitmap, page_swap);
	//print_status_bitmap(swap_usage_bitmap);
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
			t_pedidoc* pedido = list_get(restaurante->pedidos, i);

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

t_frame* get_frame_from_swap(int frame_swap){
	void* buffer = get_content(frame_swap);
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

	free(buffer);
	return marco;
}

void print_swap(){
	printf("-------------------------------MEMORIA VIRTUAL-------------------------------\n");	
	for(int i = 0; i < swap_frames; i++){
		t_frame* swap_frame = get_frame_from_swap(i);
		if(!string_is_empty(swap_frame->comida))
			printf("Indice: %d | Nombre del plato: %s | Cantidad pedido: %d | Cantidad lista: %d \n", i, swap_frame->comida, swap_frame->cantidad_pedida, swap_frame->cantidad_lista);
		free(swap_frame->comida);
		free(swap_frame);
	}
}

void print_memory(){
	printf("-------------------------------MEMORIA PRINCIPAL-----------------------------\n");
	for(int i = 0; i < frames; i++){
		t_frame* mp_frame = get_frame_from_memory(i);
		if(!string_is_empty(mp_frame->comida))
			printf("Indice: %d | Nombre del plato: %s | Cantidad pedido: %d | Cantidad lista: %d \n", i, mp_frame->comida, mp_frame->cantidad_pedida, mp_frame->cantidad_lista);
		free(mp_frame->comida);
		free(mp_frame);
	}
}

//tengo que recorrer la tabla de paginas 
//primero fijarme si hay un espacio libre en la mp
t_page* find_frame_victim(){
	t_page* victim_page = malloc(sizeof(t_page));
	victim_page->timestamp = 0;

	t_list* memory_pages = paginas_en_memoria();

	for(int i = 0; i < list_size(memory_pages); i++){
		t_page* page = list_get(memory_pages, i);
		if(victim_page->timestamp == 0 ){
			victim_page->frame = page->frame;
			victim_page->frame_mv = page->frame_mv;
			victim_page->flag = page->flag;//deberia de estar en memoria principal
			victim_page->in_use = page->in_use;
			victim_page->modified = page->modified;
			victim_page->timestamp = page->timestamp;
		}
		else if(page->flag == 1 && page->timestamp < victim_page->timestamp){
			victim_page->frame = page->frame;
			victim_page->frame_mv = page->frame_mv;
			victim_page->flag = page->flag;//deberia de estar en memoria principal
			victim_page->in_use = page->in_use;
			victim_page->modified = page->modified;
			victim_page->timestamp = page->timestamp;
		}			
	}
	return victim_page;
}

void write_frame_memory(char* comida, uint32_t cantidad_pedida, uint32_t cantidad_lista, int frame_number){
	pthread_mutex_lock(&write_memory);
	void* frame = MEMORIA[frame_number];

	memcpy(frame, &cantidad_pedida, sizeof(uint32_t));
	memcpy(frame + sizeof(uint32_t), &cantidad_lista, sizeof(uint32_t));
	memcpy(frame + sizeof(uint32_t) + sizeof(uint32_t), comida, size_char);
	log_info(logger, "[MEMORIA_PRINCIPAL] En el frame %d se inserta el plato %s - cantidad_pedida: %d - cantidad_lista: %d", frame_number, comida, cantidad_pedida, cantidad_lista);
	pthread_mutex_unlock(&write_memory);
}

//te devuelve el nro de victima para setearlo al que lo necesita
int find_victim_and_bring_it_to_mp(t_page* page){
	t_page* victim_page = find_frame_victim();
	t_frame* frame_victim = find_frame_in_memory(victim_page);
	t_frame* frame_to_move = get_frame_from_swap(page->frame_mv);

	printf("------------------ANTES DE ESCRIBIR EN SWAP----------------\n");
	print_swap();
	//ACTUALIZAR EL SWAP CON EL CONTENIDO DE MP DEL FRAME VICTIM
	escribir_swap(frame_victim->comida, frame_victim->cantidad_pedida, frame_victim->cantidad_lista, victim_page->frame_mv);
	printf("--------------------DESPUES DE ESCRIBIR EN SWAP----------------\n");
	print_swap();

	//ACTUALIZAR LA MP CON EL CONTENIDO DE SWAP EN EL FRAME DE LA VICTIMA
	printf("-------------------ANTES DE ESCRIBIR EN MEMORIA PRINCIPAL------\n");	
	print_memory();
	write_frame_memory(frame_to_move->comida, frame_to_move->cantidad_pedida, frame_to_move->cantidad_lista, victim_page->frame);
	printf("-------------------DESPUES DE ESCRIBIR EN MEMORIA PRINCIPAL-----\n");
	print_memory();

	t_list* memory_pages = paginas_en_memoria();
	for(int i = 0; i < list_size(memory_pages); i++){
		t_page* page = list_get(memory_pages, i);

		if(victim_page->frame_mv == page->frame_mv){
			victim_page->flag = false;
		}			
	}

	int frame_victim_nro = victim_page->frame;
	free(frame_victim->comida);
	free(frame_victim);
	free(victim_page);
	return frame_victim_nro;
}

int find_victim_and_update_swap(){
	t_page* victim_page = find_frame_victim();
	t_frame* frame_victim = find_frame_in_memory(victim_page);

	escribir_swap(frame_victim->comida, frame_victim->cantidad_pedida, frame_victim->cantidad_lista, victim_page->frame_mv);
	victim_page->flag = false;

	t_list* memory_pages = paginas_en_memoria();

	for(int i = 0; i < list_size(memory_pages); i++){
		t_page* page = list_get(memory_pages, i);

		if(victim_page->frame_mv == page->frame_mv){
			page->flag = false;
		}			
	}

	int frame_victim_nro = victim_page->frame;
	free(frame_victim->comida);
	free(frame_victim);
	free(victim_page);
	return frame_victim_nro;	
}

t_list* find_frames(t_pedidoc *pedido){
	t_list* platos = list_create();

	int size = list_size(pedido->pages);
	for(int i = 0; i < size; i++){
		t_page *page = list_get(pedido->pages, i);

		t_frame* frame = find_frame_in_memory(page);
		list_add(platos, frame);
	}
	return platos;
}

t_page* find_plato(t_pedidoc *pedido, char *plato){
	int size = list_size(pedido->pages);
	
	if(size > 0){ //se fija si hay algun plato cargado
		bool _find_plato(void* element){
			t_page *x = (t_page*)element;
			int frame_number = x->frame;
			t_frame *plato_a_encontrar = find_frame_in_memory(x);
			bool value = string_equals_ignore_case(plato, plato_a_encontrar->comida);
			free(plato_a_encontrar->comida);
			free(plato_a_encontrar);
			return value;
		}

		t_page *plato = list_find(pedido->pages, &_find_plato);

		if(plato == NULL){
			return NULL;
		}

		return plato;
	}
	return NULL;
}

bool increase_cantidad_plato(t_page* page, int new_cantidad_plato){
	t_frame* frame = find_frame_in_memory(page);
	int sum = frame->cantidad_pedida + new_cantidad_plato;
	write_frame_memory(frame->comida, sum, frame->cantidad_lista, page->frame);
	return true;
}

int update_cantidad_lista(t_page* page){
	t_frame *frame_a_actualizar = find_frame_in_memory(page);
	int cantidad_lista = frame_a_actualizar->cantidad_lista + 1;

	if(frame_a_actualizar->cantidad_lista == frame_a_actualizar->cantidad_pedida){
		return PLATO_TERMINADO;
	}

	write_frame_memory(frame_a_actualizar->comida, frame_a_actualizar->cantidad_pedida, cantidad_lista, page->frame);

	if(frame_a_actualizar->cantidad_pedida == cantidad_lista){
		return PLATO_TERMINADO;
	}

	return PLATO_NO_TERMINADO;
}

int get_available_blocks_number() {
    int cont = 0;
    for (int i = 0; i < swap_frames; i++) {
        if (bitarray_test_bit(swap_usage_bitmap, i) == 0) { cont++; }
    }
    return cont;
}

//este metodo se utiliza para un frame nuevo
t_page* asignar_frame (char *nombre_plato, int cantidad_pedida){
	pthread_mutex_lock(&mutex_asignar_pagina);
	int frame_number = find_free_frame_memory();
	int swap_frame = find_free_swap_frame();
	
	if(frame_number == -1){
		log_comanda("No hay espacio en la memoria.. hacer swap");

		if(swap_frame == -1){
			log_comanda("No hay espacio en swap");
			return NULL;
		}

		int frame_in_mp = find_victim_and_update_swap();

		escribir_swap(nombre_plato, cantidad_pedida, 0, swap_frame);
		write_frame_memory(nombre_plato, cantidad_pedida, 0, frame_in_mp);

		t_page* page = create_page(frame_in_mp, swap_frame);

		pthread_mutex_unlock(&mutex_asignar_pagina);
		return page;
	}
	else{
		// printf("----------------------Escribo en RAM\n");
		//print_status_bitmap(frame_usage_bitmap);

		escribir_swap(nombre_plato, cantidad_pedida, 0, swap_frame);
		write_frame_memory(nombre_plato, cantidad_pedida, 0, frame_number);

		t_page* new_page = create_page(frame_number, swap_frame);

		bitarray_set_bit(frame_usage_bitmap, frame_number);

		// printf("------------------------Update RAM\n");
		//print_status_bitmap(frame_usage_bitmap);
		pthread_mutex_unlock(&mutex_asignar_pagina);
		return new_page;
	}
}

void free_pages(t_list* pages){
	for(int i = 0; i < list_size(pages); i++){
		t_page* page= list_get(pages, i);
		
		if(page->flag == IN_MEMORY){
			pthread_mutex_lock(&memory_frames_bitarray);
			bitarray_clean_bit(frame_usage_bitmap, page->frame);
			pthread_mutex_unlock(&memory_frames_bitarray);
		}

		pthread_mutex_lock(&swap_frames_bitarray);
		bitarray_clean_bit(swap_usage_bitmap, page->frame_mv);
		pthread_mutex_unlock(&swap_frames_bitarray);		
	}

	list_destroy_and_destroy_elements(pages, &free);
}

