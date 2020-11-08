#include "../include/comanda_handler.h"

t_result* _guardar_pedido(t_request *request){
	
	t_restaurante *rest = find_restaurante(request->nombre);
	
	if(rest == NULL){
		//Crear el segmento *restaurante* y la tabla de pagina *pedidos*
		t_restaurante *restaurante_creado = crear_restaurante(request->nombre);

		if(restaurante_creado == NULL){
			t_result * result = getTResult("[GUARDAR_PEDIDO] Fail.", true);
			return result;
		}
		t_pedidoc *pedido = crear_pedido(request->idPedido);
		add_pedido_to_restaurante(restaurante_creado, pedido);

		t_result * result = getTResult("[GUARDAR_PEDIDO] Ok.", false);		
		return result;
	}

	t_pedidoc *pedido = crear_pedido(request->idPedido); 
	add_pedido_to_restaurante(rest, pedido);
	
	t_result * result = getTResult("[GUARDAR_PEDIDO] Ok.", false);	
	return result;
}

/*
 * 1° Verificar si existe la tabla de segmentos, si no existe tengo que mandar mensaje sobre la situacion.--> done
 * 2° Verificar si existe el segmento del pedido, tambien informar. --> done
 * 3° Verificar si existe el plato dentro de la tabla de paginas del pedido. En caso contrario, se debera asignar una nueva pagina al segmento. --> done
 * 4° Si la pagina no se encuentra cargada en la memoria principal, se debe cargar la misma desde la swap (iniciando la eleccion de la victima de ser necesario)
 * disclaimer: se debera agregar el plato y anexar la cantidad que se tiene cocinar de dicho plato
 * disclaimer: responde el mensaje indicando si se puedo realizar
 */
t_result* _guardar_plato(t_req_plato *request){
	if(strlen(request->plato) + 1 > 24){
		t_result * result = getTResult("[GUARDAR_PLATO] Fail. El nombre exede los 24 bytes", true);	
		return result;		
	}

	t_restaurante *rest = find_restaurante(request->restaurante);	

	if(rest == NULL){
		t_result * result = getTResult("[GUARDAR_PLATO] Fail. SEGFAULT. No existe el segmento", true);	
		return result;
	}
	
	t_pedidoc *pedido = find_pedido(rest, request->idPedido);

	if(pedido == NULL){
		t_result * result = getTResult("[GUARDAR_PLATO] Fail. No existe el pedido", true);	
		return result;
	}

	t_page *page = find_plato(pedido, request->plato);

	if(page == NULL){
		t_page *plato_creado = asignar_frame(request->plato, request->cantidadPlato);

		if(plato_creado != NULL){
			list_add(pedido->pages, plato_creado);

			/*Validarrrrr si se guardo*/
			t_page *plato_enc = find_plato(pedido, request->plato);

			if(plato_enc){
				t_result * result = getTResult("[GUARDAR_PLATO] Ok.", false);					
				return result;
			}
			else{
				t_result * result = getTResult("[GUARDAR_PLATO] Fail. Somenthing went wrong.", true);				
				return result;
			}
		}
	}

	t_result *result = getTResult("[GUARDAR_PLATO] Este plato ya existe en memoria", false);
	return result;
}

/*
 * 1° Verificar si existe la tabla de segmentos, si no existe tengo que mandar mensaje sobre la situacion.--> done
 * 2° Verificar si existe el segmento del pedido, tambien informar. --> done
 * 3° Verificar si existe el plato dentro de la tabla de paginas del pedido. En caso contrario, se debera asignar una nueva pagina al segmento. --> done
 * 4° Si la pagina no se encuentra cargada en la memoria principal, se debe cargar la misma desde la swap (iniciando la eleccion de la victima de ser necesario)
 * 5° Eniviar si se pudo con la respuesta
 */
t_pedido* _obtener_pedido(t_request* request){
	t_restaurante *rest = find_restaurante(request->restaurante);	

	if(rest == NULL){
		t_result * result = getTResult("[GUARDAR_PLATO] Fail. SEGFAULT. No existe el segmento", true);	
		return result;
	}
	
	t_pedidoc *pedido = find_pedido(rest, request->idPedido);

	if(pedido == NULL){
		t_result * result = getTResult("[GUARDAR_PLATO] Fail. No existe el pedido", true);	
		return result;
	}

	/*			t_pedido *pedido = malloc(sizeof(t_pedido)); t_list *platos = list_create();
				t_plato *milanesa = malloc(sizeof(t_plato)); t_plato *empanadas = malloc(sizeof(t_plato)); t_plato *ensalada = malloc(sizeof(t_plato));

				milanesa->plato = "Milanesa"; milanesa->precio = 200; milanesa->cantidadPedida = 2; milanesa->cantidadLista = 1;
				empanadas->plato = "Empanadas"; empanadas->precio = 880; empanadas->cantidadPedida = 12; empanadas->cantidadLista = 6;
				ensalada->plato = "Ensalada"; ensalada->precio = 120; ensalada->cantidadPedida = 1; ensalada->cantidadLista = 0;
				list_add(platos, milanesa); list_add(platos, empanadas); list_add(platos, ensalada);

				pedido->estado = PENDIENTE; pedido->platos = platos; pedido->precioTotal = calcularPrecioTotal(platos);

				enviarPaquete(socketCliente, COMANDA, RTA_OBTENER_PEDIDO, pedido);
				free(pedido); free(milanesa); free(empanadas); free(ensalada);*/
	return NULL;
}

/*
 * 1° Verificar si existe la tabla de segmentos, si no existe tengo que mandar mensaje sobre la situacion.--> done
 * 2° Verificar si existe el segmento del pedido, tambien informar. --> done
 * 3° Verificar que el pedido este en estado PENDIENTE, si no esta informar la situacion
 * 4° Cambiar el estado PENDIENTE a CONFIRMADO
 * 5° Responder el mensaje con Ok/Fail
 */
void _confirmar_pedido(char *nombre_rest, int id_pedido){

}

/*
 * 1° Verificar si existe la tabla de segmentos, si no existe tengo que mandar mensaje sobre la situacion.--> done
 * 2° Verificar si existe el segmento del pedido, tambien informar. --> done
 * 3° Verificar si en MP sino tenes que traerlo de area de swap
 * 3° Verificar que el pedido este en estado CONFIRMADO, si no esta informar la situacion
 * 4° Se debera aumentar en uno la cantidad del plato. Si todos los platos estan terminados, se deberá cambiar el estado
 * del pedido a TERMINADO
 * 5° Responder el mensaje con Ok/Fail
 */
void _plato_listo(char *nombre_rest, int id_pedido, char *plato_listo)
{

}

/*
 * 1° Verificar si existe la tabla de segmentos, si no existe tengo que mandar mensaje sobre la situacion.--> done
 * 2° Verificar si existe el segmento del pedido, tambien informar. --> done
 * 3° Se elimina la paginas correspondientes a ese segmento
 * 4° Se elimina el segmento
 * 5° Responder el mensaje con Ok/Fail
 */
void _finalizar_pedido(char *nombre_rest, int id_pedido)
{

}
