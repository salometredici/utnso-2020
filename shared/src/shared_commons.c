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