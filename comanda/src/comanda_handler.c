#include "../include/comanda_handler.h"

t_result* _guardar_pedido(t_request *request){
	
	t_restaurante *rest = find_restaurante(request->nombre);
	
	if(rest == NULL){
		//Crear el segmento *restaurante* y la tabla de pagina *pedidos*
		char* nombre_rest = string_new();
		string_append_with_format(&nombre_rest, "%s", request->nombre);
		
		t_restaurante *restaurante_creado = crear_restaurante(nombre_rest);
		int id_pedido = request->idPedido;

		if(restaurante_creado == NULL){
			t_result * result = getTResult("[GUARDAR_PEDIDO] Fail. No se pudo creer el restaurante", true);
			return result;
		}

		t_pedidoc *pedido = crear_pedido(id_pedido);

		add_pedido_to_restaurante(restaurante_creado, pedido);

		t_result * result = getTResult("[GUARDAR_PEDIDO] Ok. Se creo el Restaurante y se agrego el pedido", false);		
		return result;
	}

	t_pedidoc *pedido = find_pedido(rest, request->idPedido);

	if (pedido) {
		t_result *result = getTResult("[FINALIZAR_PEDIDO] Fail. Ya se ha creado un pedido con ese numero.", true);	
		return result;
	}

	t_pedidoc *pedido_creado = crear_pedido(request->idPedido); 
	add_pedido_to_restaurante(rest, pedido_creado);
	
	t_result * result = getTResult("[GUARDAR_PEDIDO] Ok. Se agrego el pedido al restaurante", false);	
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
	char* nombre_plato = string_new();
	string_append_with_format(&nombre_plato, "%s", request->plato);
	
	int size_name = strlen(nombre_plato) + 1;
	if(size_name > 24){
		t_result * result = getTResult("[GUARDAR_PLATO] Fail. El nombre exede los 24 bytes", true);	
		return result;		
	}

	t_restaurante *rest = find_restaurante(request->restaurante);	

	if(rest == NULL){
		t_result * result = getTResult("[GUARDAR_PLATO] Fail. SEGFAULT. No existe el restaurante", true);	
		return result;
	}
	
	t_pedidoc *pedido = find_pedido(rest, request->idPedido);

	if(pedido == NULL){
		t_result * result = getTResult("[GUARDAR_PLATO] Fail. No existe el pedido", true);	
		return result;
	}

	if(pedido->estado == CONFIRMADO || pedido->estado == FINALIZADO){
		t_result* result = getTResult("[GUARDAR_PLATO] Fail. El pedido tiene que estar en estado PENDIENTE", true);
		return result;
	}

	//el find plato tiene que buscarse en memoria principal y si esta apuntando a swap entonces tiene que sacarlo a mp 
	t_page *page = find_plato(pedido, nombre_plato);

	if(page == NULL){
		t_page *plato_creado = asignar_frame(nombre_plato, request->cantidadPlato);

		if(plato_creado != NULL){
			list_add(pedido->pages, plato_creado);

			print_pages_in_memory();
			print_swap();
			print_memory();
		
			t_result * result = getTResult("[GUARDAR_PLATO] Ok.", false);					
			return result;
		}
	}

	int done = increase_cantidad_plato(page, request->cantidadPlato);

	if(!done){
		t_result* result = getTResult("[GUARDAR_PLATO] Fail. Something went wrong", true);
		return result;
	}

	print_pages_in_memory();
	print_swap();
	print_memory();
	t_result *result = getTResult("[GUARDAR_PLATO] Ok", false);
	return result;
}

/*
 * 1° Verificar si existe la tabla de segmentos, si no existe tengo que mandar mensaje sobre la situacion.--> done
 * 2° Verificar si existe el segmento del pedido, tambien informar. --> done
 * 3° Si la pagina no se encuentra cargada en la memoria principal, se debe cargar la misma desde la swap (iniciando la eleccion de la victima de ser necesario)
 * 4° Enviar si se pudo con la respuesta
 */
t_pedido* _obtener_pedido(t_request* request){
	
	t_restaurante *rest = find_restaurante(request->nombre);	

	if(rest == NULL){
		t_result * result = getTResult("[GUARDAR_PLATO] Fail. SEGFAULT. No existe el segmento", true);	
		return result;
	}
	
	t_pedidoc *pedido = find_pedido(rest, request->idPedido);

	if(pedido == NULL){
		t_result * result = getTResult("[GUARDAR_PLATO] Fail. No existe el pedido", true);	
		return result;
	}

	t_list *marcos = find_frames(pedido);

	t_pedido *pedido_info = malloc(sizeof(t_pedido));

	t_list *platos = list_create();
	for(int i = 0; i < list_size(marcos); i++){
		t_frame *marco = list_get(marcos, i);
		t_plato *plato = malloc(sizeof(t_plato));
		plato->plato = marco->comida;
		plato->cantidadPedida = marco->cantidad_pedida;
		plato->cantidadLista = marco->cantidad_lista;
		list_add(platos, plato);
	}

	pedido_info->restaurante = request->nombre;
	pedido_info->estado = pedido->estado;
	pedido_info->platos = platos;
	pedido_info->precioTotal = 0; //ver que onda por que comanda no tiene esa info	

	list_destroy_and_destroy_elements(marcos, &free);
	print_pages_in_memory();
	print_swap();
	print_memory();
	return pedido_info;
}

/*
 * 1° Verificar si existe la tabla de segmentos, si no existe tengo que mandar mensaje sobre la situacion.--> done
 * 2° Verificar si existe el segmento del pedido, tambien informar. --> done
 * 3° Verificar que el pedido este en estado PENDIENTE, si no esta informar la situacion
 * 4° Cambiar el estado PENDIENTE a CONFIRMADO
 * 5° Responder el mensaje con Ok/Fail
 */
t_result* _confirmar_pedido(t_request *request){
	t_restaurante *rest = find_restaurante(request->nombre);	

	if(rest == NULL){
		t_result *result = getTResult("[CONFIRMAR_PEDIDO] Fail. SEGFAULT. No existe el restaurante", true);	
		return result;
	}
	
	t_pedidoc *pedido = find_pedido(rest, request->idPedido);

	if(pedido == NULL){
		t_result *result = getTResult("[GUARDAR_PLATO] Fail. No existe el pedido", true);	
		return result;
	}

	if(pedido->estado != PENDIENTE){
		t_result *result = getTResult("[CONFIMAR_PEDIDO] Fail. No esta en estado pendiente", false);
		return result;
	}

	pedido->estado = CONFIRMADO;
	t_result* result = getTResult("[CONFIRMAR_PEDIDO] Ok.", false);
	
	if(string_equals_ignore_case(request->nombre, REST_DEFAULT)){
		t_list* platos = find_frames(pedido);

		for(int i = 0; i < list_size(platos); i++){
			t_frame* frame = list_get(platos, i);
			
			for(int j = 0; j < frame->cantidad_pedida; j++){
				t_plato_listo* request_pl = getTPlatoListo(request->nombre, request->idPedido, frame->comida);
				t_result* result = _plato_listo(request_pl);
			
				if(result->hasError){
					t_result *result = getTResult(result->msg, true);
					return result;
				}

				free(request_pl);
				free_t_result(result);
			}
		}
	}
	return result;
}

/*
 * 1° Verificar si existe la tabla de segmentos, si no existe tengo que mandar mensaje sobre la situacion.--> done
 * 2° Verificar si existe el segmento del pedido, tambien informar. --> done
 * 3° Verificar si en MP sino tenes que traerlo de area de swap
 * 3° Verificar que el pedido este en estado CONFIRMADO, si no esta informar la situacion --> done
 * 4° Se debera aumentar en uno la cantidad del plato. Si todos los platos estan terminados, se deberá cambiar el estado
 * del pedido a TERMINADO  --> 50% done
 * 5° Responder el mensaje con Ok/Fail --> done
 */
t_result* _plato_listo(t_plato_listo* request) {
	t_restaurante *rest = find_restaurante(request->restaurante);

	if (rest == NULL) {
		t_result *result = getTResult("[PLATO_LISTO] Fail. No existe el restaurante.", true);	
		return result;
	}

	t_pedidoc *pedido = find_pedido(rest, request->idPedido);

	if (pedido == NULL) {
		t_result *result = getTResult("[PLATO_LISTO] Fail. No existe el pedido.", true);	
		return result;
	}

	if (pedido->estado != CONFIRMADO) {
		if(pedido->estado == PENDIENTE){
			t_result *result = getTResult("[PLATO_LISTO] Fail. Pedido sin confirmar. Se encuentra en estado PENDIENTE", true);	
			return result;
		}
		else if(pedido->estado == TERMINADO){
			t_result *result = getTResult("[PLATO_LISTO] Fail. Pedido sin confirmar. Se encuentra en estado TERMINADO", true);	
			return result;
		}
	}

	// Buscar nombre_plato_listo en pages del pedido
	t_page *pl_page = find_plato(pedido, request->plato);

	if(pl_page == NULL){
		t_result* result = getTResult("[PLATO_LISTO] Fail. No existe el plato", true);
		return result;
	}

	int response = update_cantidad_lista(pl_page);

	if(response == PLATO_TERMINADO){
		//validar que todos los platos del pedido esten terminados
		t_list *marcos = find_frames(pedido);

		bool _is_plato_terminado(void * elemento){
			t_frame* marco = elemento;
			return marco->cantidad_lista == marco->cantidad_pedida; 
		};

		int value = list_all_satisfy(marcos, &_is_plato_terminado);

		list_destroy_and_destroy_elements(marcos, &free);

		if(value){	
			print_pages_in_memory();			
			print_swap();
			print_memory();
			pedido->estado = TERMINADO;
			t_result *result = getTResult("[PLATO_LISTO] Ok. Todos los platos estan terminados. Se termino el pedido", false);	
			return result;
		}

	}

	print_pages_in_memory();
	print_swap();
	print_memory();
	t_result *result = getTResult("[PLATO_LISTO] Ok.", false);	
	return result;
}

/*
 * 1° Verificar si existe la tabla de segmentos, si no existe tengo que mandar mensaje sobre la situacion.--> done
 * 2° Verificar si existe el segmento del pedido, tambien informar. --> done
 * 3° Se elimina la paginas correspondientes a ese segmento
 * 4° Se elimina el segmento
 * 5° Responder el mensaje con Ok/Fail
 */
t_result* _finalizar_pedido(t_request* request){
	t_restaurante *rest = find_restaurante(request->nombre);

	if (rest == NULL) {
		t_result *result = getTResult("[FINALZAR_PEDIDO] Fail. No existe tabla de segmentos.", true);	
		return result;
	}

	t_pedidoc *pedido = find_pedido(rest, request->idPedido);

	if (pedido == NULL) {
		t_result *result = getTResult("[FINALZAR_PEDIDO] Fail. No existe segmento.", true);	
		return result;
	}

	free_pages(pedido->pages);

	if(pedido->pages){
		delete_pedido_from_restaurant(rest->pedidos, request->idPedido);
		t_result *result = getTResult("[FINALZAR_PEDIDO] Ok.", false);	
		return result;
	}

	t_result *result = getTResult("[FINALZAR_PEDIDO] Fail. Somethin wen wrong", true);	
	return result;
}
