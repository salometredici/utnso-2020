#include "../include/shared_commons.h"

// Inicialización

bool fdExists(char *fd) {
	struct stat dirStat;
	if (stat(fd, &dirStat) < 0) {
		return false;
	}
	return true;
}

void createDirectory(char *directory) {
	if (!fdExists(directory)) {
		recursiveCreateDir(directory);
	}
}

void recursiveCreateDir(char *path) {
	int i = 0;
	char **dirs = string_split(path, "/");
	char initialDir[string_length(dirs[i]) + 1];
	strcpy(initialDir, "/");
	char *route = initialDir;
	do {
		strcat(initialDir, dirs[i]); strcat(route, "/");
		if (mkdir(route, 0777) && errno != EEXIST) {
			log_debug(logger, "El directorio %s ya existe", route);
		}
		i++;
	} while (dirs[i] != NULL);
}


// Utils

void limpiarPantalla() {
	system("clear");
}

bool stringFound(char *expected, char *string) {
	char* actual = string;
	return string_equals_ignore_case(expected, actual);
}

// Devuelve el p_code correspondiente a un int 
p_code intToPCode(int key) {
	for (int i = 0; i < PROCNKEYS; i++) {
		t_keys sym = diccionarioProcesos[i];
		if (key == sym.valor) {
			return sym.valor;
		}
	}
	return ERROR;
}

// Devuelve el valor en string de un proceso/comando, el parámetro option corresponde al diccionario (PROCNKEYS o COMMANDNKEYS)
char *getStringKeyValue(int key, int option) {
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

// Devuelve el número correspondiente a un comando para ser utilizado en un switch o comparación
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

char *obtenerLogFileName() {
	return config_get_string_value(config, "ARCHIVO_LOG");
}

char *obtenerCliente() {
	return config_get_string_value(config, "ID_CLIENTE");
}

int obtenerPuertoEscucha() {
	return config_get_int_value(config, "PUERTO_ESCUCHA");
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
		printf("\t[%d] - %s\n", i, palabra);
		log_info(logger, "\t[%d] - %s", i, palabra);
	}
}

void mostrarListaPlatos(t_list *listaPlatos) {
	int cantidadPlatos = list_size(listaPlatos);
	for (int i = 0; i < cantidadPlatos; i++) {
		t_plato *platoActual = list_get(listaPlatos, i);
		printf("Plato: [%s]\n", platoActual->plato);
		printf("\tPrecio: $%d\n", platoActual->precio);
		printf("\tCantidad pedida: %d\n", platoActual->cantidadPedida);
		printf("\tCantidad lista: %d\n", platoActual->cantidadLista);
		log_info(logger, "Plato %d: [%s], $%d, Pedido: %d, Listo: %d", i, platoActual->plato, platoActual->precio, platoActual->cantidadPedida, platoActual->cantidadLista);
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