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
	/*
	int size = list_size(memory);
	for(int i = 0; i < size; i++)
	{
		printf("Id Frame %d\n", i);
		log_info(logger, "Nro de Frame %d", i);
	}*/
	
	for(int i = 0; i < frames; i++){
		if(*(char*)(memory+i)!='\0'){
			printf("%c",*(char*)(memory+i));
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

	MEMORY = calloc(frames, PAGE_SIZE);
	int i;
	for(i = 0; i < frames; i++) {
		void* new_frame = malloc(PAGE_SIZE);
		memset(new_frame, 1, sizeof(bool));
		memset(new_frame + sizeof(bool), NULL, PAGE_SIZE - sizeof(bool));
		*(MEMORY + i) = new_frame;
	}

	/*int i;
	for(int i = 0; i < frames; i++)
	{
		t_frame* frame = malloc(sizeof(t_frame));
		frame->cantidad = 0;
		frame->cantidadLista = 0;
		frame->comidas = NULL;

		list_add(memory, frame);
	}*/

	print_structure();

}

t_pedidoc *create_pedido(char *name, int id_pedido)
{
	t_pedidoc *pedido = malloc(sizeof(t_pedidoc));
	pedido->id_pedido = id_pedido;
	pedido->frame = NULL;
	pedido->in_use = 0;
	pedido->modified = 0;
	log_info(logger, "Nuevo Tabla de Paginas para id_pedido-(%d) creado.", id_pedido);
	return pedido;
}

t_segment *create_segment(char* nombre)
{
	t_segment *segment = malloc(sizeof(segment));
	segment->name = nombre;
	segment->idsegment = id_segment == 0 ? id_segment : id_segment++;
	segment->pedidos = list_create();
	segment->estado = 0;
	log_info(logger, "Nuevo segmento-restaurante (%s) creado.", nombre);
	return segment;	
}

void agregar_pedido_a_restaurante(t_segment *segment, t_pedidoc *pedido){
	list_add(segment->pedidos, pedido);
	log_comanda("Se agrego un pedido");
}

void create_restaurant(char *name, int id_pedido)
{
	t_segment *segment = create_segment(name);
	t_pedidoc *pedido = create_pedido("pedido", id_pedido);
	agregar_pedido_a_restaurante(segment, pedido);
	add_restaurant(name, segment);
}

void add_restaurant(char *nombre, t_segment *segment)
{
	t_restaurante *restaurante = malloc(sizeof(t_restaurante));
	restaurante->nombre = nombre;
	restaurante->segment = segment;
	list_add(restaurantes, restaurante);
	//printf();
}

void init_comanda() {
    init_variables();
    init_tables();
	init_memory();
}
