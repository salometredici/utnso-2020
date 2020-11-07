#include "../include/memoria_core.h"

t_restaurante *crear_restaurante(char *nombre_rest)
{
    t_restaurante *restaurante = malloc(sizeof(restaurante));
    restaurante->nombre = nombre_rest;
    restaurante->pedidos = list_create();
    restaurante->estado = (int)INICIAL;
    log_comanda("Se creo un restaurante.");
    list_add(restaurantes, restaurante);
}

t_pedidoc *crear_pedido(int id_pedido)
{
	t_pedidoc *pedido = malloc(sizeof(t_pedidoc));
	pedido->id_pedido = id_pedido;
	pedido->platos = list_create();
	log_info(logger, "Se cre la tabla de Paginas para id_pedido-(%d) creado.", id_pedido);
	return pedido;
}

t_restaurante* find_restaurante(char *nombre)
{
    bool es_restaurante_buscado(void *elemento)	{
		t_restaurante *x = (t_restaurante*) elemento;
		return !strcmp(x->nombre, nombre);
	}

	t_restaurante *rest = list_find(restaurantes,&es_restaurante_buscado)
	return rest;
}

void add_pedido_to_restaurante(t_restaurante *restaurante, t_pedidoc *pedido){
	list_add(restaurante->pedidos, pedido);
}

t_pedidoc* find_pedido(t_restaurante *restaurante, int id){
    bool _find_pedido(void *elemento)	{
		t_pedidoc *x = (t_pedidoc*) elemento;
		return x->id_pedido = id;
	}

	t_pedidoc *pedido = list_find(restaurante->pedidos,&_find_pedido)
	return pedido;	
}

t_plato* find_plato(t_pedidoc *pedido, char *plato){
	/*Tengo que fijarme si existe el plato esto quiere decir que primero tengo que agarrar el size de los pedidos
	 * 
	*/

    bool es_restaurante_buscado(void *elemento)	{
		t_restaurante *x = (t_restaurante*) elemento;
		return !strcmp(x->nombre, nombre);
	}

	t_restaurante *rest = list_find(restaurantes,&es_restaurante_buscado)
	return rest;	
}




