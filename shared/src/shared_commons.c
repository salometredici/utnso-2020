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

void mostrarListaPlatos(t_list *listaPlatos) {
	int cantidadPlatos = list_size(listaPlatos);
	for (int i = 0; i < cantidadPlatos; i++) {
		t_plato *platoActual = list_get(listaPlatos, i);
		printf("Plato: %s, Precio: $%d\n", platoActual->plato, platoActual->precio);
		printf("Cantidad pedida: %d, Cantidad lista: %d\n", platoActual->cantidadPedida, platoActual->cantidadLista);
		free(platoActual);
	}
}

char *getStringEstadoPedido(t_estado estado) {
	switch (estado) {
		case PENDIENTE:
			return "PENDIENTE";
		case CONFIRMADO:
			return "CONFIRMADO";
		case FINALIZADO:
			return "FINALIZADO";
		default:
			break;
	}
}