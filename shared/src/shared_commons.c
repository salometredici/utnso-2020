#include "../include/shared_commons.h"

// Utils

void limpiarPantalla() {
	system("clear");
}

void mostrarListaStrings(t_list *listaStrings) {
	int cantidadElementos = list_size(listaStrings);
	for (int i = 0; i < cantidadElementos; i++) {
		char *palabra = list_get(listaStrings, i);
		printf("\tPosición %d: %s\n", i, palabra);
	}
}

int calcularPrecioTotal(t_list *listaPlatos) {
	int precioTotal = 0;
	int cantidadPlatos = list_size(listaPlatos);
	for (int i = 0; i < cantidadPlatos; i++) {
		t_plato *plato = list_get(listaPlatos, i);
		precioTotal += plato->precio;
	}
	return precioTotal;
}