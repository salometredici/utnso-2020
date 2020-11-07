#include "../include/comanda_handler.h"

t_result* _guardar_pedido(char *nombre_rest, int id_pedido){
	
	t_restaurante *rest = find_restaurante(nombre_rest);
	t_result *result = malloc(sizeof(t_result));
	
	if(rest == NULL){
		//Crear el segmento *restaurante* y la tabla de pagina *pedidos*
		t_restaurante *restaurante_creado = crear_restaurante(nombre_rest);

		if(restaurante_creado == NULL){
			result->msg = "[GUARDAR_PEDIDO] Fail";
			result->hasError = true;
			return t_result;
		}
		t_pedidoc *pedido = crear_pedido(id_pedido);
		add_pedido_to_restaurante(restaurante, pedido);
		
		result->msg = "[GUARDAR_PEDIDO] Ok";
		result->hasError = false;
		return t_result;
	}

	t_pedidoc *pedido = crear_pedido(); 
	add_pedido_to_restaurante(rest, pedido);
	
	result->msg = "[GUARDAR_PEDIDO] Ok"
	result->hasError = false;
	return t_result;
}

t_result _guardar_plato(char *nombre_rest, int id_pedido, char *plato, int cantidad){
	/*
	 * Primero Verificar si existe la tabla de segmentos, si no existe tengo que mandar mensaje sobre la situacion --done
	 * Desp verificar si existe el segmento del pedido, tambien informar --done
	 * verificar si existe el plato dentro de la tabla de paginas del pedido. en caso contrario, se debera asignar una nueva pagina al segmento
	 * en caso de que la pagina no se encuentre cargada en la memoria principal se debe cargar la misma desde la swap (iniciando la eleccion de la victima de ser necesario)
	 * se debera agregar el plato y anexar la cantidad que se tiene cocinar de dicho plato
	 * responde el mensaje indicando si se puedo realizar
	 */

	if(strlen(plato) + 1 > 24){
		t_result->msg = "[GUARDAR_PLATO]El nombre del plato exede el tamanio";
		t_result->hasError = true;
		return t_result;		
	}

	t_result *result = malloc(sizeof(result));

	t_restaurante *rest = find_restaurante(nombre_rest);	

	if(rest == NULL){
		t_result->msg = "[GUARDAR_PLATO]SEGFAULT - No existe el Restaurante";
		t_result->hasError = true;
		return t_result;
	}
	
	t_pedidoc *pedido = find_pedido(rest, id_pedido);

	if(pedido == NULL){
		t_result->msg = "[GUARDAR_PLATO] No existe el pedido";
		t_result->hasError = true;
		return t_result;
	}

	t_plato *plato = find_plato(pedido->platos);

	if(plato == NULL){
		t_plato *plato_creado = asignar_frame(plato, cantidad);

		if(plato_creado != NULL){
			list_add(pedido->platos, plato_creado);
			result->msg = "[GUARDAR_PLATO] Exito";
			resutl->hasError = false;
			return t_result;
		}

	}

}

void _obtener_pedido(char *nombre_rest, int id_pedido)
{

}

void _confirmar_pedido(char *nombre_rest, int id_pedido)
{

}

void _plato_listo(char *nombre_rest, int id_pedido, char *plato_listo)
{

}

void _finalizar_pedido(char *nombre_rest, int id_pedido)
{

}
