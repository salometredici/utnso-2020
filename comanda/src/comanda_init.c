#include "../include/comanda_init.h"

void init_variables() {
    SWAP_SIZE = config_get_int_value(config, "TAMANIO_SWAP");
    MEMORY_SIZE = config_get_int_value(config, "TAMANIO_MEMORIA");
    ALGORITMO_REEMPLAZO = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
}

void init_tables() {
    memory = list_create();
	restaurantes = list_create();
}

void print_structure()
{
	int size = list_size(memory);
	for(int i = 0; i < size; i++)
	{
		printf("Id Frame %d\n", i);
		log_info(logger, "Nro de Frame %d", i);
	}
	
}

void init_memory()
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

	print_structure();

}

t_pedido *create_pedido(char *name, int id_pedido)
{
	t_pedido *pedido = malloc(sizeof(t_pedido));
	pedido->nombre = name;
	pedido->id_pedido = id_pedido;
	pedido->frames = NULL;
	log_info(logger, "Nuevo Tabla de Paginas para id_pedido-(%d) creado.", id_pedido);
}

t_segment *create_segment(char* nombre)
{
	t_segment *segment = malloc(sizeof(segment));
	segment->name = nombre;
	segment->idsegment = id_segment == 0 ? id_segment : id_segment++;
	segment->pedidos = NULL;
	log_info(logger, "Nuevo segmento (%s) creado.", nombre);
	return segment;	
}

void add_restaurant(char *nombre)
{
	list_add(restaurantes, nombre);
}

void init_comanda() {
    init_variables();
    init_tables();
	init_memory();
}
