#include "../include/shared_commons.h"

// Utils

void limpiarPantalla() {
	system("clear");
}

// // Devuelve el tamaño en bytes de una lista de strings (bytes de cada palabra) más un int por cada uno, que representará a su longitud
// int getBytesAEnviarListaStrings(t_list *listaStrings) {
// 	int cantidadElementos = list_size(listaStrings);
// 	int bytesAEnviar = cantidadElementos * sizeof(int);

// 	for (int i = 0; i < cantidadElementos; i++) {
// 		char *palabra = list_get(listaStrings, i);
// 		bytesAEnviar += strlen(palabra) + 1;
// 	}

// 	return bytesAEnviar;
// }

void mostrarListaStrings(t_list *listaStrings) {
	int cantidadElementos = list_size(listaStrings);
	for (int i = 0; i < cantidadElementos; i++) {
		char *palabra = list_get(listaStrings, i);
		printf("\tPosición %d: %s\n", i, palabra);
	}
}