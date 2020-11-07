#include "../include/comanda_init.h"

void init_variables() {
    SWAP_SIZE = config_get_int_value(config, "TAMANIO_SWAP");
    MEMORY_SIZE = config_get_int_value(config, "TAMANIO_MEMORIA");
    ALGORITMO_REEMPLAZO = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
}

void init_restaurantes() {
	restaurantes = list_create();
}

void print_structure()
{	
	for(int i = 0; i < frames; i++){
		if(*(char*)(MEMORIA + i)!='\0'){
			printf("%c",*(char*)(MEMORIA + i));
		}
	}
	printf("\n");
	fflush(stdout);	
}

void log_comanda(char *text){
	log_info(logger, text);
}

void init_memory()
{
	frames = MEMORY_SIZE / PAGE_SIZE;
	int bitmap_size_in_bytes = ceil((double) frames / 8);
	
	bitmap_pointer = malloc(bitmap_size_in_bytes);
	frame_usage_bitmap = bitarray_create_with_mode(bitmap_pointer, bitmap_size_in_bytes, LSB_FIRST);

	MEMORIA = calloc(frames, PAGE_SIZE);
	int i;
	for(i = 0; i < frames; i++) {
		void* new_frame = malloc(PAGE_SIZE);
		memset(new_frame, NULL, PAGE_SIZE);
		*(MEMORIA + i) = new_frame;
	}
	size_char = MAX_LENGTH_COMIDA;
	//print_structure();
}

void init_virtual() {

	swap_file = fopen(SWAP_FILE, "wb+");
	
	int swap_frames = SWAP_SIZE / PAGE_SIZE;
	int swap_bitmap_size_in_bytes = ceil((double) swap_frames / 8);
	swap_bitmap_pointer = malloc(swap_bitmap_size_in_bytes);
	swap_usage_bitmap = bitarray_create_with_mode(swap_bitmap_pointer, swap_bitmap_size_in_bytes, LSB_FIRST);

	clear_bitmap(swap_usage_bitmap, swap_frames);
}

void init_comanda() {
    init_variables();
    init_restaurantes();
	init_memory();
	init_virtual();
}