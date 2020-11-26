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
	string_substring(line, equal_index, strlen(line)-1);//
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
t_list *get_platos_con_precios_from_rest(char *info_restaurante) {
	t_list *platos_con_precios = list_create();
	t_list *platos = get_list_from_string(info_restaurante, 3, 8);
	t_list *precios = get_list_from_string(info_restaurante, 4, 15);
	for (int i = 0; i < list_size(platos); i++) {
		t_md_receta *plato_actual = malloc(sizeof(t_md_receta));
		plato_actual->plato = list_get(platos, i);
		plato_actual->precio = atoi(list_get(precios, i));
		list_add(platos_con_precios, plato_actual);
	}
	free(platos); free(precios);
	return platos_con_precios;
}

int obtener_precio_plato(char *plato, char *info_restaurante) {
	t_list *menu_restaurante = get_platos_con_precios_from_rest(info_restaurante);
	bool es_plato_actual(void *actual) {
		t_md_receta *plato_menu_actual = actual;
		return string_equals_ignore_case(plato, plato_menu_actual->plato);
	};
	t_md_receta *plato_encontrado = list_find(menu_restaurante, &es_plato_actual);
	int precio_plato = plato_encontrado != NULL ? plato_encontrado->precio : ERROR;
	free(menu_restaurante); if (plato_encontrado != NULL) { free(plato_encontrado); }
	return precio_plato;
}

// Retorna una lista de t_plato a partir de tres listas de strings, una para los platos, otra para sus cant. pedidas y otra para sus cant. listas
t_list *get_t_plato_list_from_lists(t_list *platos, t_list *cant_pedidas, t_list *cant_listas) {
	t_list *t_plato_list = list_create();
	for (int i = 0; i < list_size(platos); i++) {
		t_plato *plato_actual = malloc(sizeof(t_plato));
		plato_actual->plato = list_get(platos, i);
		plato_actual->cantidadPedida = atoi(list_get(cant_pedidas, i));
		plato_actual->cantidadLista = atoi(list_get(cant_listas, i));
		list_add(t_plato_list, plato_actual);
	}
	return t_plato_list;	
}

// Retorna un md
t_md *get_md_from_string(char *afip_content) {
	t_md *md = getEmptyMd();
	md->cantidadHornos = atoi(get_plain_line_content(afip_content, 5));
	md->cantidadCocineros = atoi(get_plain_line_content(afip_content, 0));
	md->cantidadPedidos = atoi(get_plain_line_content(afip_content, 6));

	md->platos = get_platos_con_precios_from_rest(afip_content);

	md->afinidades = get_list_from_string(afip_content, 2, 20);

	t_posicion *posicion = get_posicion_from_string(get_plain_line_content(afip_content, 1));
	md->posX = posicion->posX;
	md->posY = posicion->posY;

	free(posicion);
	return md;
}

int obtener_precio_pedido(t_list *platos_con_precios, t_list *platos_pedido) {
	int precio_total = 0;
	for (int i = 0; i < list_size(platos_pedido); i++) {

		t_plato *plato_actual = list_get(platos_pedido, i);

		bool es_plato_actual(void *actual) {
			t_md_receta *plato_menu_actual = actual;
			return string_equals_ignore_case(plato_actual->plato, plato_menu_actual->plato);
		};

		t_md_receta *plato_encontrado = list_find(platos_con_precios, &es_plato_actual);

		if (plato_encontrado) { precio_total += plato_encontrado->precio; }

		free(plato_actual); free(plato_encontrado);
	}
	return precio_total;
}

t_pedido *get_pedido_from_string(char *info, t_request *request, char *info_restaurante) {
	t_pedido *pedido = malloc(sizeof(t_pedido));
	pedido->restaurante = request->nombre;
	pedido->estado = string_to_t_estado(get_plain_line_content(info, 0));

	t_list *platos = get_list_from_string(info, 1, 14);
	t_list *cant_pedidas = get_list_from_string(info, 2, 17);
	t_list *cant_listas = get_list_from_string(info, 3, 16);

	pedido->platos = get_t_plato_list_from_lists(platos, cant_pedidas, cant_listas);

	t_list *menu_restaurante = get_platos_con_precios_from_rest(info_restaurante);
	pedido->precioTotal = obtener_precio_pedido(menu_restaurante, pedido->platos);

	free(platos); free(cant_pedidas); free(cant_listas);
	return pedido;
}

/* Funcionalidades */

bool sabe_preparar_plato_restaurante(t_req_plato *request) {
	t_md *restaurante = obtener_restaurante(request->restaurante);
	bool existe_plato(void *actual) {
		t_md_receta *plato_actual = actual;
		return string_equals_ignore_case(plato_actual->plato, request->plato);
	};
	return list_any_satisfy(restaurante->platos, &existe_plato);
}

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

t_pedido *obtener_pedido(t_request *request) {
	char *info_restaurante = get_info(RESTAURANTE, request->nombre);
	char *info_pedido = get_info(PEDIDO, get_full_pedido_path(request));
	t_pedido *pedido;
	if (string_equals_ignore_case(info_pedido, BLOQUES_NO_ASIGNADOS)) {
		pedido = getEmptyPedido(SIN_PLATOS);
	} else {
		pedido = get_pedido_from_string(info_pedido, request, info_restaurante);
	}
	free(info_restaurante);
	return pedido;
}

void incrementar_plato_en_pedido(t_req_plato *request) {

}

void guardar_plato_en_pedido(t_req_plato *request) {

}

void agregar_plato_a_pedido(t_req_plato *request, t_pedido *pedido_actual) {
	char *info_restaurante = get_info(RESTAURANTE, request->restaurante);
	if (list_is_empty(pedido_actual->platos)) {
		int precio_plato = obtener_precio_plato(request->plato, info_restaurante);
		guardar_primer_plato(request, precio_plato);
	} else if(existe_plato_en_pedido(request->plato, pedido_actual)) {
		incrementar_plato_en_pedido(request);
	} else {
		guardar_plato_en_pedido(request);
	}
}

t_result *check_and_add_plato(t_req_plato *request) {
	t_request *req_pedido_buscado = getTRequest(request->idPedido, request->restaurante);
	if (!existe_pedido(req_pedido_buscado)) {
		free(req_pedido_buscado);
		return getTResult(PEDIDO_NO_EXISTE, true);
	} else {
		t_pedido *pedido_a_guardar_plato = obtener_pedido(req_pedido_buscado);
		if (pedido_a_guardar_plato->estado == TERMINADO ||
			pedido_a_guardar_plato->estado == FINALIZADO) {
			free(req_pedido_buscado); free(pedido_a_guardar_plato);
			return getTResult(ESTADO_AVANZADO, true);
		} else {
			agregar_plato_a_pedido(request, pedido_a_guardar_plato);
			free(req_pedido_buscado); free(pedido_a_guardar_plato);
			return getTResult(PEDIDO_ACTUALIZADO, false);
		}
	}
}