#include "../include/sindicato_logic.h"

/* Retorna el valor comprendido entre el char '=' y '\n' como int
	Ejemplo:
		line = "SIZE=158\n"
		index_equal = 4
		index_endl = 8
	Retorna el valor comprendido entre ambos -> 158 */
int get_int_file_value(char *line, int index_equal, int index_endl) {
	return atoi(string_substring(line, index_equal, index_endl));
}

t_list *get_list_from_string(char *info, int line_number, int start_index) {
	t_list *new_list = list_create();
	char **lines = string_split(info, "\n");
	char *line_to_use = lines[line_number];
	char *list = strrchr(line_to_use, '[');
	char *string_list = string_substring(list, 1, strlen(list)-2);
	int values_quantity = getValuesQuantity(string_list);
	char **values = string_split(string_list, ",");
	for (int i = 0; i < values_quantity; i ++) {
		list_add(new_list, values[i]);
	}
	free(lines); free(line_to_use); free(string_list); free(values);
	return new_list;
}

t_list *get_platos_restaurante(char *info_restaurante) {
	return get_list_from_string(info_restaurante, 3, 8);
}

t_list *obtener_platos_restaurante(char *restaurante) {
	t_list *platos_restaurante = list_create();
	char *info_restaurante = get_info(RESTAURANTE, restaurante);
	list_add_all(platos_restaurante, get_platos_restaurante(info_restaurante));
	return platos_restaurante;
}