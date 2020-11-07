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
			return result;
		}
		t_pedidoc *pedido = crear_pedido(id_pedido);
		add_pedido_to_restaurante(restaurante_creado, pedido);
		
		result->msg = "[GUARDAR_PEDIDO] Ok";
		result->hasError = false;
		return result;
	}

	t_pedidoc *pedido = crear_pedido(id_pedido); 
	add_pedido_to_restaurante(rest, pedido);
	
	result->msg = "[GUARDAR_PEDIDO] Ok";
	result->hasError = false;
	return result;
}

t_result* _guardar_plato(char *nombre_rest, int id_pedido, char *plato, int cantidad){
	/*
	 * Primero Verificar si existe la tabla de segmentos, si no existe tengo que mandar mensaje sobre la situacion --done
	 * Desp verificar si existe el segmento del pedido, tambien informar --done
	 * verificar si existe el plato dentro de la tabla de paginas del pedido. en caso contrario, se debera asignar una nueva pagina al segmento
	 * en caso de que la pagina no se encuentre cargada en la memoria principal se debe cargar la misma desde la swap (iniciando la eleccion de la victima de ser necesario)
	 * se debera agregar el plato y anexar la cantidad que se tiene cocinar de dicho plato
	 * responde el mensaje indicando si se puedo realizar
	 */
	t_result *result = malloc(sizeof(result));

	if(strlen(plato) + 1 > 24){
		result->msg = "[GUARDAR_PLATO]El nombre del plato exede el tamanio";
		result->hasError = true;
		return result;		
	}

	t_restaurante *rest = find_restaurante(nombre_rest);	

	if(rest == NULL){
		result->msg = "[GUARDAR_PLATO]SEGFAULT - No existe el Restaurante";
		result->hasError = true;
		return result;
	}
	
	t_pedidoc *pedido = find_pedido(rest, id_pedido);

	if(pedido == NULL){
		result->msg = "[GUARDAR_PLATO] No existe el pedido";
		result->hasError = true;
		return result;
	}

	t_page *page = find_plato(pedido, plato);

	if(page == NULL){
		t_page *plato_creado = asignar_frame(plato, cantidad);

		if(plato_creado != NULL){
			list_add(pedido->pages, plato_creado);

			/*Validarrrrr si se guardo*/
			t_page *plato_enc = find_plato(pedido, plato);

			result->msg = "[GUARDAR_PLATO] Exito";
			result->hasError = false;
			return result;
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
