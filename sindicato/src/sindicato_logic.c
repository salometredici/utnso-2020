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

char *get_plain_line_content(char *content, int line_number) {
	char **lines = string_split(content, "\n");
	char *line = lines[line_number];
	int equal_index = find_char_index(line, '=');
	free(lines);
	return string_substring(line, equal_index, strlen(line));
}

char *get_full_line_content(char *content, int line_number) {
	char **lines = string_split(content, "\n");
	char *line = lines[line_number];
	free(lines);
	return line;	
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

t_list *get_platos_restaurante(char *info_restaurante) {
	return get_list_from_string(info_restaurante, 3, 8);
}

// Retorna una lista de t_instruccion_receta
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
	free(pasos); free(tiempos);
	return inst_list;
}

// Retorna un t_receta
t_receta *get_receta_from_string(char *info, char *receta_a_buscar) {
	t_receta *receta = malloc(sizeof(t_receta));
	receta->plato = receta_a_buscar;
	receta->instrucciones = get_instrucciones_from_string(info);
	return receta;
}

// Retorna un t_posicion
t_posicion *get_posicion_from_string(char *info){
	char *list = string_substring(info, 1, strlen(info)-1);
	char **values = string_split(list, ",");
	t_posicion *posicion = malloc(sizeof(t_posicion));
	posicion->posX = atoi(values[0]);
	posicion->posY = atoi(values[1]);
	free(list); free(values);
	return posicion;
}

// Retorna una lista de t_md_receta a partir de dos listas de strings, una para los platos y otra para sus precios
t_list *get_md_platos_from_lists(t_list *platos, t_list *precios) {
	t_list *platos_con_precios = list_create();
	for (int i = 0; i < list_size(platos); i++) {
		t_md_receta *plato_actual = malloc(sizeof(t_md_receta));
		plato_actual->plato = list_get(platos, i);
		plato_actual->precio = atoi(list_get(precios, i));
		list_add(platos_con_precios, plato_actual);
	}
	return platos_con_precios;
}

// Retorna un md
t_md *get_md_from_string(char *afip_content) {
	t_md *md = getEmptyMd();
	md->cantidadHornos = atoi(get_plain_line_content(afip_content, 5));
	md->cantidadCocineros = atoi(get_plain_line_content(afip_content, 0));
	md->cantidadPedidos = atoi(get_plain_line_content(afip_content, 6));

	t_list *platos = get_list_from_string(afip_content, 3, 8);
	t_list *precios = get_list_from_string(afip_content, 4, 15);
	md->platos = get_md_platos_from_lists(platos, precios);

	md->afinidades = get_list_from_string(afip_content, 2, 20);

	t_posicion *posicion = get_posicion_from_string(get_plain_line_content(afip_content, 1));
	md->posX = posicion->posX;
	md->posY = posicion->posY;

	free(platos); free(precios); free(posicion);
	return md;
}

/* Funcionalidades */

t_list *obtener_platos_restaurante(char *restaurante) {
	t_list *platos_restaurante = list_create();
	char *info_restaurante = get_info(RESTAURANTE, restaurante);
	list_add_all(platos_restaurante, get_platos_restaurante(info_restaurante));
	free(info_restaurante);
	return platos_restaurante;
}

t_list *obtener_receta(char *receta_a_buscar) {
	char *info_receta = get_info(RECETA, receta_a_buscar);
	return get_receta_from_string(info_receta, receta_a_buscar);
}

t_md *obtener_restaurante(char *restaurante) {
	char *info_restaurante = get_info(RESTAURANTE, restaurante);
	return get_md_from_string(info_restaurante);
}