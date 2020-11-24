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

char *get_string_file_value(char *line, int index_equal, int index_endl) {
	return string_substring(line, index_equal, index_endl);
}

// Retorna una lista de strings
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

t_list *get_instrucciones_from_string(char *info) {
	t_list *inst_list = list_create();
	t_list *pasos = get_list_from_string(info, 0, 7);
	t_list *tiempos = get_list_from_string(info, 1, 14);
	for (int i = 0; i < list_size(pasos); i++) {
		char *current_step = list_get(pasos, i);
		char *current_time = list_get(tiempos, i);
		t_instrucciones_receta *current_inst = getTPaso(current_step, atoi(current_time));
		list_add(inst_list, current_inst);
	}
	return inst_list;
}

t_receta *get_receta_from_string(char *info, char *receta_a_buscar) {
	t_receta *receta = malloc(sizeof(t_receta));
	receta->plato = receta_a_buscar;
	receta->instrucciones = get_instrucciones_from_string(info);
	return receta;
}

t_list *get_platos_restaurante(char *info_restaurante) {
	return get_list_from_string(info_restaurante, 3, 8);
}

/* Funcionalidades */

t_list *obtener_platos_restaurante(char *restaurante) {
	t_list *platos_restaurante = list_create();
	char *info_restaurante = get_info(RESTAURANTE, restaurante);
	list_add_all(platos_restaurante, get_platos_restaurante(info_restaurante));
	return platos_restaurante;
}

t_list *obtener_receta(char *receta_a_buscar) {
	char *info_receta = get_info(RECETA, receta_a_buscar);
	t_receta *receta = get_receta_from_string(info_receta, receta_a_buscar);
	return receta;
}