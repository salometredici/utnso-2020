#include "../include/sindicato_logic.h"

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

// Retorna un t_pedido
t_pedido *get_pedido_from_string(char *info_pedido, t_request *request, char *info_restaurante) {
	t_pedido *pedido = getEmptyPedido();
	pedido->estado = string_to_t_estado(get_plain_line_content(info_pedido,0));
	pedido->precioTotal = atoi(get_plain_line_content(info_pedido, 4));
	pedido->restaurante = request->nombre;
	t_list *platos_pedido = get_list_from_string(info_pedido, 1, 14);
	t_list *num_pedidos = get_list_from_string(info_pedido, 2, 17);
	t_list *num_listos = get_list_from_string(info_pedido, 3, 16);
	int cant_platos = list_size(num_pedidos);
	for (int i = 0; i < cant_platos; i++) {
		t_plato *plato_pedido = malloc(sizeof(t_plato));
		plato_pedido->cantidadPedida = atoi(list_get(num_pedidos, i));
		plato_pedido->cantidadLista = atoi(list_get(num_listos, i));
		plato_pedido->plato = list_get(platos_pedido, i);
		list_add(pedido->platos, plato_pedido);
	}
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
		pedido = getEmptyPedido_with_error(SIN_PLATOS);
	} else {
		pedido = get_pedido_from_string(info_pedido, request, info_restaurante);
	}
	free(info_restaurante);
	return pedido;
}

void incrementar_plato_en_pedido(t_req_plato *request, int precio_plato) {
	t_request *req = getTRequest(request->idPedido, request->restaurante);
	char *pedido_path = get_full_pedido_path(req);
	char *pedido_actual = get_info(PEDIDO, pedido_path);
	int cant_actual = get_required_blocks_number(strlen(pedido_actual));

	char *pedido_actualizado = get_GuardarPlato_Data(request, precio_plato, false);
	int cant_actualizada = get_required_blocks_number(strlen(pedido_actualizado));

	free(pedido_actual); free(req);

	update_content(pedido_path, PEDIDO, pedido_actualizado, cant_actual == cant_actualizada ?
																MISMOS_BLOQUES :
																(cant_actual < cant_actualizada ?
																	AGREGAR_BLOQUES :
																	QUITAR_BLOQUES));

	printf("%s", pedido_actualizado);
}

void guardar_plato_en_pedido(t_req_plato *request, int precio_plato) {
	t_request *req = getTRequest(request->idPedido, request->restaurante);
	char *pedido_path = get_full_pedido_path(req);
	char *pedido_actual = get_info(PEDIDO, pedido_path);
	int cant_actual = get_required_blocks_number(strlen(pedido_actual));

	char *pedido_actualizado = get_GuardarPlato_Data(request, precio_plato, true);
	int cant_actualizada = get_required_blocks_number(strlen(pedido_actualizado));
	free(pedido_actual); free(req);

	update_content(pedido_path, PEDIDO, pedido_actualizado, cant_actual == cant_actualizada ?
																MISMOS_BLOQUES :
																(cant_actual < cant_actualizada ?
																	AGREGAR_BLOQUES :
																	QUITAR_BLOQUES));

	printf("%s", pedido_actualizado);
}

void agregar_plato_a_pedido(t_req_plato *request, t_pedido *pedido_actual) {
	char *info_restaurante = get_info(RESTAURANTE, request->restaurante);
	int precio_plato = obtener_precio_plato(request->plato, info_restaurante);
	if (list_is_empty(pedido_actual->platos)) {
		guardar_primer_plato(request, precio_plato);
	} else if (existe_plato_en_pedido(request->plato, pedido_actual)) {
		incrementar_plato_en_pedido(request, precio_plato);
	} else {
		guardar_plato_en_pedido(request, precio_plato);
	}
}

t_result *check_and_add_plato(t_req_plato *request) {
	t_request *req_pedido_buscado = getTRequest(request->idPedido, request->restaurante);
	if (!existe_pedido(req_pedido_buscado)) {
		free(req_pedido_buscado);
		return getTResult(PEDIDO_NO_EXISTE, true);
	} else {
		t_pedido *pedido_a_guardar_plato = obtener_pedido(req_pedido_buscado);
		if (pedido_a_guardar_plato->estado != PENDIENTE) {
			free(req_pedido_buscado); free(pedido_a_guardar_plato);
			return getTResult(ESTADO_AVANZADO, true);
		} else {
			agregar_plato_a_pedido(request, pedido_a_guardar_plato);
			free(req_pedido_buscado); free(pedido_a_guardar_plato);
			return getTResult(PEDIDO_ACTUALIZADO, false);
		}
	}
}