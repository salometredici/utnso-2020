#include "../include/app_handler.h"

t_list* _consultar_restaurantes() {
	t_list *restaurantes;

	if (list_is_empty(restaurantesConectados)) {
		restaurantes = list_create();
		list_add(restaurantes, restauranteDefault);
	} else {
        restaurantes = obtenerRestsConectados();
	}

	return restaurantes;
}

t_result* _seleccionar_restaurante(t_cliente *cliente, t_selecc_rest *tupla_cliente_rest) {
	t_result *rta_selec_rest;

	cliente->restSeleccionado = tupla_cliente_rest->restauranteSeleccionado;
	logSeleccionarRestaurante(tupla_cliente_rest);
	rta_selec_rest = getTResult("[SELECCIONAR_RESTAURANTE] Ok", false);
	
	return rta_selec_rest;
}