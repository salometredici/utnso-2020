#include "../include/comanda_init.h"

void initVariables() {
    SWAP_SIZE = config_get_int_value(config, "TAMANIO_SWAP");
    MEMORY_SIZE = config_get_int_value(config, "TAMANIO_MEMORIA");
    ALGORITMO_REEMPLAZO = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
}

void initTables() {
    memory = list_create();
	restaurantes = list_create();
}

void imprimirEstructura()
{
	int size = list_size(memory);
	for(int i = 0; i < size; i++)
	{
		printf("Id Frame %d\n", i);
		log_info(logger, "Nro de Frame %d", i);
	}
	
}
void initMemoriaPrincipal()
{
    // Memoria
	int frames = MEMORY_SIZE / PAGE_SIZE;

	//MEMORY = calloc(frames, PAGE_SIZE);
	
	//primera forma para generar una tabla
	/*int i;
	for(i = 0; i < frames; i++) {
		void* new_frame = malloc(PAGE_SIZE);
		memset(new_frame, NULL, PAGE_SIZE);
		*(MEMORY + i) = new_frame;
	}*/


	int i;
	for(int i = 0; i < frames; i++)
	{
		t_frame* frame = malloc(sizeof(t_frame));
		frame->cantidad = 0;
		frame->cantidadLista = 0;
		frame->comidas = NULL;

		list_add(memory, frame);
	}

	imprimirEstructura();

}

void initComanda() {
    initVariables();
    initTables();
    initMemoriaPrincipal();
}
