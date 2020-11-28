#include "../include/comanda_init.h"

void init_variables() {
    SWAP_SIZE = config_get_int_value(config, "TAMANIO_SWAP");
    MEMORY_SIZE = config_get_int_value(config, "TAMANIO_MEMORIA");
    ALGORITMO_REEMPLAZO = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
}

void init_restaurantes() {
	restaurantes = list_create();
}

void log_comanda(char *text){
	log_info(logger, text);
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
	//printf("Bitmap RAM (aka MEMORIA_PRINCIPAL)\n");
	//int i = bitarray_get_max_bit(frame_usage_bitmap);
	//printf("Lcoo %d", i);
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
	
	int truncate_result = ftruncate(fd, SWAP_SIZE);
	void * archivo_data = mmap(NULL, SWAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		
	memset(archivo_data, 0, SWAP_SIZE);
	msync(archivo_data, SWAP_SIZE, MS_SYNC);
	
	return archivo_data;
}

void init_virtual() {
	//la ruta del swap va a la config desp vemos
	swap_frames = SWAP_SIZE / PAGE_SIZE;

	archivo_swap = create_swap();

	int swap_bitmap_size_in_bytes = ceil((double) swap_frames / 8);
	//printf("SWAP SIZE: %d\n", swap_bitmap_size_in_bytes);
	swap_bitmap_pointer = malloc(swap_bitmap_size_in_bytes);
	swap_usage_bitmap = bitarray_create_with_mode(swap_bitmap_pointer, swap_bitmap_size_in_bytes, LSB_FIRST);
	//printf("Bitmap disco (aka MEMORIA_VIRTUAL\n");
	//int i = bitarray_get_max_bit(frame_usage_bitmap);
	//printf("Lcoo virtual %d", i);
	clear_bitmap(swap_usage_bitmap, swap_frames);
}

void init_comanda() {
    init_variables();
    init_restaurantes();
	init_mutexs();
	init_memory();
	init_virtual();
}