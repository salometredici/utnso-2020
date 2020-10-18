#include "../include/comanda_init.h"

void initVariables()
{
    MEMORY_SIZE =  config_get_int_value(config, "TAMANIO_MEMORIA");
    SWAP_SIZE = config_get_int_value(config, "TAMANIO_SWAP");
    ALGORITMO_REEMPLAZO = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
}

void initTables()
{
    tables = list_create();
}

void initMemoriaPrincipal()
{
    // Memoria
	int frames = MEMORY_SIZE / PAGE_SIZE;

	MEMORY = calloc(frames, PAGE_SIZE);
	int i;
	for(i = 0; i < frames; i++) {
		void* new_frame = malloc(PAGE_SIZE);
		memset(new_frame, 1, sizeof(bool));
		memset(new_frame + sizeof(bool), NULL, PAGE_SIZE - sizeof(bool));
		*(MEMORY + i) = new_frame;
	}
}

void initStructures()
{
    initVariables();
    initTables();
    initMemoriaPrincipal();
}