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
	} while (i < 10 && dirs[i] != NULL);
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

t_estado string_to_t_estado(char *estado) {
    for (int i = 0; i < T_ESTADOSNKEYS; i++) {
    	t_keys sym = diccionarioTEstados[i];
    	if (string_equals_ignore_case(estado,sym.key)) {
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
		//printf("\tPrecio: $%d\n", platoActual->precio);
		printf("\tCantidad pedida: %d\n", platoActual->cantidadPedida);
		printf("\tCantidad lista: %d\n", platoActual->cantidadLista);
		// log_info(logger, "Plato %d: [%s], $%d, Pedido: %d, Listo: %d", i, platoActual->plato, platoActual->precio, platoActual->cantidadPedida, platoActual->cantidadLista);
		log_info(logger, "Plato %d: [%s], Pedido: %d, Listo: %d", i, platoActual->plato, platoActual->cantidadPedida, platoActual->cantidadLista);
		//free(platoActual);
	}
}

// Funciones

t_link_element *list_find_element(t_list *self, bool(*condition)(void*), int *index) {
	t_link_element *element = self->head;
	int position = 0;

	while (element != NULL && !condition(element->data)) {
		element = element->next;
		position++;
	}

	if (index != NULL) {
		*index = position;
	}

	return element;
}

// int calcularPrecioTotal(t_list *listaPlatos) { // rehacer
// 	int precioTotal = 0;
// 	int cantidadPlatos = list_size(listaPlatos);
// 	for (int i = 0; i < cantidadPlatos; i++) {
// 		t_plato *plato = list_get(listaPlatos, i);
// 		precioTotal += plato->precio;
// 	}
// 	return precioTotal;
// }

char *getStringEstadoPedido(t_estado estado) {
	switch (estado) {
		case PENDIENTE:
			return "PENDIENTE";
		case CONFIRMADO:
			return "CONFIRMADO";
		case TERMINADO:
			return "TERMINADO";
		case FINALIZADO:
			return "FINALIZADO";
		case REST_INEXISTENTE:
			return "REST_INEXISTENTE";
		case PEDIDO_INEXISTENTE:
			return "PEDIDO_INEXISTENTE";
		case SIN_PLATOS:
			return "SIN_PLATOS";
		default:
			break;
	}
}

double get_current_time(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	unsigned long long result = (((unsigned long long)tv.tv_sec) * 1000 + ((unsigned long long)tv.tv_usec) / 1000);
	double a = result;
	return a;
}

bool only_contains_numbers(char *input) {
	for (int i = 0; i < strlen(input); i++) {
		if (input[i] < '0' || input[i] > '9') { return false; }
	}
	return true;
}

int find_char_index(char *string, char caracter) {
	char *found_char = strchr(string, caracter);
	if (found_char != NULL) {
		return found_char - string + 1;
	} else {
		return ERROR;
	}
}

