#include "../include/comanda_init.h"

void print_init_structure(){
	printf(CYAN"→ Tamanio de MP: "BOLDCYAN"%d"RESET BREAK, MEMORY_SIZE);
	printf(CYAN"→ Cant. de frames de MP: " BOLDCYAN"%d"RESET BREAK, frames);
	printf(CYAN"→ Tamanio de SWAP: "BOLDCYAN"%d"RESET BREAK, SWAP_SIZE);
	printf(CYAN"→ Cant. de frames de SWAP: "BOLDCYAN"%d"RESET BREAK, swap_frames);
	printf(CYAN"→ Algoritmo de reemplazo: "BOLDCYAN"%s"RESET BREAK, ALGORITMO_REEMPLAZO);

	log_info(logger,"---------------------------------------------------");	
	log_info(logger, "→ Tamanio de MP: %d", MEMORY_SIZE);
	log_info(logger, "→ Cant. de frames de MP: %d", frames);
	log_info(logger, "→ Tamanio de SWAP: %d", SWAP_SIZE);
	log_info(logger, "→ Cant. de frames de SWAP: %d", swap_frames);
	log_info(logger, "→ Algoritmo de reemplazo: %s", ALGORITMO_REEMPLAZO);
	log_info(logger,"---------------------------------------------------");		
}

void init_variables() {
    SWAP_SIZE = config_get_int_value(config, "TAMANIO_SWAP");
    MEMORY_SIZE = config_get_int_value(config, "TAMANIO_MEMORIA");
    ALGORITMO_REEMPLAZO = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
}

void init_restaurantes() {
	restaurantes = list_create();
}

void init_mutexs() {
	pthread_mutex_init(&memory_frames_bitarray, NULL);
	pthread_mutex_init(&swap_frames_bitarray, NULL);
	pthread_mutex_init(&mutex_paginas_en_memoria, NULL);
	pthread_mutex_init(&mutex_asignar_pagina, NULL);
	pthread_mutex_init(&mx_find_frame_in_memory, NULL);
	pthread_mutex_init(&mx_find_frame_in_swap, NULL);
	pthread_mutex_init(&write_memory, NULL);
}

void init_memory(){
	frames = MEMORY_SIZE / PAGE_SIZE;
	
	int bitmap_size_in_bytes = ceil((double) frames / 8);
	bitmap_pointer = malloc(bitmap_size_in_bytes);
	frame_usage_bitmap = bitarray_create_with_mode(bitmap_pointer, bitmap_size_in_bytes, LSB_FIRST);

	clear_bitmap(frame_usage_bitmap, frames);

	MEMORIA = calloc(frames, PAGE_SIZE);
	
	for(int i = 0; i < frames; i++) {
		void* new_frame = malloc(PAGE_SIZE);
		memset(new_frame, NULL, PAGE_SIZE);
		*(MEMORIA + i) = new_frame;
	}

	size_char = MAX_LENGTH_COMIDA;
}

void* create_swap(){
	int fd = open(RUTASWAP, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

	if(fd == ERROR){
		exit(EXIT_FAILURE);
	}
	
	ftruncate(fd, SWAP_SIZE);
	void * archivo_data = mmap(NULL, SWAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		
	memset(archivo_data, 0, SWAP_SIZE);
	msync(archivo_data, SWAP_SIZE, MS_SYNC);
	
	return archivo_data;
}

void init_virtual() {
	swap_frames = SWAP_SIZE / PAGE_SIZE;

	archivo_swap = create_swap();

	int swap_bitmap_size_in_bytes = ceil((double) swap_frames / 8);

	swap_bitmap_pointer = malloc(swap_bitmap_size_in_bytes);
	swap_usage_bitmap = bitarray_create_with_mode(swap_bitmap_pointer, swap_bitmap_size_in_bytes, LSB_FIRST);

	clear_bitmap(swap_usage_bitmap, swap_frames);
}

void init_comanda() {
	puntero_clock = 0;
    init_variables();
    init_restaurantes();
	init_mutexs();
	init_memory();
	init_virtual();
}
