#include "../include/shared_commons.h"

// Utils

void limpiarPantalla() {
	system("clear");
}

// Devuelve el valor en string del proceso/comando
char* getStringKeyValue(int key, int option) {
	t_keys *diccionario; int size;
	switch (option) {
		case PROCNKEYS:
			diccionario = diccionarioProcesos; size = PROCNKEYS;
			break;
		case COMMANDNKEYS:
			diccionario = diccionarioComandos; size = COMMANDNKEYS;
			break;
		default:
			return ERROR;
	}
    for (int i = 0; i < size; i++) {
    	t_keys sym = diccionario[i];
    	if (key == sym.valor) {
        	return sym.key;
    	}
	}
    return ERROR;
}

// Devuelve el valor correspondiente al enum del comando para utilizarlo en un switch o similar
int commandToString(char *key) {
    t_keys *diccionario = diccionarioComandos;
    for (int i = 0; i < COMMANDNKEYS; i++) {
        t_keys sym = diccionario[i];
        if (strcmp(sym.key, key) == 0) {
            return sym.valor;
        }
    }
    return ERROR;
}

/* Config */

int obtenerPuertoEscucha() {
	return config_get_int_value(config, "PUERTO_ESCUCHA");
}

char* obtenerNombreRestaurante() {
	return config_get_string_value(config, "NOMBRE_RESTAURANTE");
}

bool obtenerActiveConsole() { 
	return config_get_int_value(config, "ACTIVE_CONSOLE");
}

int obtenerLogLevel() {
	return config_get_int_value(config, "LOG_LEVEL");
}

// Mostrar por consola

void mostrarListaStrings(t_list *listaStrings) {
	int cantidadElementos = list_size(listaStrings);
	for (int i = 0; i < cantidadElementos; i++) {
		char *palabra = list_get(listaStrings, i);
		printf("\tPosición %d: %s\n", i, palabra);
	}
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

// Funciones

int calcularPrecioTotal(t_list *listaPlatos) {
	int precioTotal = 0;
	int cantidadPlatos = list_size(listaPlatos);
	for (int i = 0; i < cantidadPlatos; i++) {
		t_plato *plato = list_get(listaPlatos, i);
		precioTotal += plato->precio;
	}
	return precioTotal;
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