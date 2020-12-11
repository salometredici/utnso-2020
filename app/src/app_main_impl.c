#include "../include/app_main_impl.h"

/* Funciones y métodos */

// Limpiamos clientes desconectados de una lista
void revisar_conectados(t_list *lista) {
	for (int i = 0; i < list_size(lista); i++) {
		t_cliente *cliente_actual = list_get(lista, i);
		if (recv(cliente_actual->socketCliente, NULL, 1, MSG_PEEK | MSG_DONTWAIT) == 0) {
			list_remove(lista, i);
			liberarConexion(cliente_actual->socketCliente);
			free(cliente_actual);
		}
	}
}

// Revisamos si el cliente ya existe en la lista de conectados y limpiar los desconectados
void actualizar_clientes_conectados(t_cliente *cliente) {
	pthread_mutex_lock(&mutex_clientes);	
	revisar_conectados(clientes_conectados);
	pthread_mutex_unlock(&mutex_clientes);	
	pthread_mutex_lock(&mutex_rests);		
	revisar_conectados(rests_conectados);
	pthread_mutex_unlock(&mutex_rests);

	bool estaDuplicado(void *actual) {
		t_cliente *cliente_actual = actual;
		return string_equals_ignore_case(cliente->idCliente, cliente_actual->idCliente);
	};

	pthread_mutex_lock(&mutex_rests);
	pthread_mutex_lock(&mutex_clientes);
	if (cliente->esRestaurante &&
			(list_is_empty(rests_conectados) ||
			!list_any_satisfy(rests_conectados, &estaDuplicado)))
	{
		list_add(rests_conectados, cliente);
	} else if (list_is_empty(clientes_conectados) || !list_any_satisfy(clientes_conectados, &estaDuplicado))
	{
		list_add(clientes_conectados, cliente);
	}
	pthread_mutex_unlock(&mutex_rests);
	pthread_mutex_unlock(&mutex_clientes);	
}

int generar_id(int min, int max) {
	srand(time(NULL));
	
	int nuevoId = (rand() % (max - min + 1)) + min;
	
	bool idExistente(void *actual) {
		int idActual = actual;
		return idActual == nuevoId;
	}
	
	if (list_find(idsGenerados, &idExistente) == NULL) {
		list_add(idsGenerados, nuevoId);
		return nuevoId;
	} else {
		return generar_id(min, max);
	}
};

// CONSULTAR_PLATOS

t_cliente *get_rest_conectado(char *rest_buscado) { // !!!
	bool restFound(void *actual) {
		t_cliente *rest_actual = actual;
		return string_equals_ignore_case(rest_buscado, rest_actual->idCliente);
	};
	return list_find(rests_conectados, &restFound);
}

t_cliente *get_t_cliente(t_list *lista_conectados, char *buscado) {
	bool client_found(void *actual) {
		t_cliente *cliente_actual = actual;
		return string_equals_ignore_case(buscado, cliente_actual->idCliente);
	};
	return list_find(lista_conectados, &client_found);
}

bool esta_conectado_rest(char *rest_buscado) {
	return get_t_cliente(rests_conectados, rest_buscado) == NULL ? false : true;
}

// CONSULTAR_RESTAURANTES

t_list *obtener_rests_conectados() {
	t_list *nombres_rests = list_create();
	for (int i = 0; i < list_size(rests_conectados); i++) {
		t_cliente *current_rest = list_get(rests_conectados, i);
		list_add(nombres_rests, current_rest->idCliente);
	}
	return nombres_rests;
}

// SELECCIONAR_RESTAURANTE

void bind_client_to_rest(t_cliente *cliente, char *restSelecc) {
	cliente->restSelecc = restSelecc;
	if (string_equals_ignore_case(cliente->restSelecc, rest_default)) {
		cliente->posRest->posX = posResDefault->posX;
		cliente->posRest->posY = posResDefault->posY;
	} else {
		bool esElBuscado(void *actual) {
			t_cliente *rest_actual = actual;
			return string_equals_ignore_case(rest_actual->idCliente, restSelecc);
		};
		t_cliente *rest_buscado = list_find(rests_conectados, &esElBuscado);
		cliente->posRest->posX = rest_buscado->posCliente->posX;
		cliente->posRest->posY = rest_buscado->posCliente->posY;		
	}
}

bool es_valido_restaurante(char *rest) {
	if (string_equals_ignore_case(rest, rest_default)) {
		return true;
	} else {
		bool esElBuscado(void *actual) {
			t_cliente *rest_actual = actual;
			return string_equals_ignore_case(rest_actual->idCliente, rest);
		};
		return list_any_satisfy(rests_conectados, &esElBuscado);
	}
}