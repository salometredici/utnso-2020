#include "../include/app.h"

/* Hilo de planificación */

void *planificar(void *args) {
	while (1) {
		// Largo plazo
		update_QR_con_QN();
		update_QR_con_QB();
		switch (algoritmoSeleccionado) {
			case FIFO:
				update_QE_con_QR_FIFO();
				break;
			case HRRN:
				update_QE_con_QR_HRRN();
				break;
			case SJF:
				update_QE_con_QR_SJF();
				break;
			default:
				break;
		}
		// Corto plazo
		ejecutar_ciclos();
	}
}

/* Hilo de conexiones */

void *atenderConexiones(void *conexionNueva)
{
    pthread_data *t_data = (pthread_data*) conexionNueva;
    int socketCliente = t_data->socketThread;
    free(t_data);

	t_cliente *cliente = malloc(sizeof(t_cliente));
	int socketComanda = ERROR;

	while (1) {
		
    	t_header *header = recibirHeaderPaquete(socketCliente);

		if (header->procesoOrigen == ERROR || header->codigoOperacion == ERROR) {
			log_app_client_disconnection(cliente->idCliente, socketCliente);
			liberarConexion(socketCliente);
    		pthread_exit(EXIT_SUCCESS);
			return EXIT_FAILURE;
		}

    	switch (header->codigoOperacion) {
			case OBTENER_PROCESO:;
				enviarPaquete(socketCliente, APP, RTA_OBTENER_PROCESO, APP);
				break;
			case ENVIAR_DATACLIENTE:;
				cliente = recibirPayloadPaquete(header, socketCliente);
				cliente->socketCliente = socketCliente;
				actualizar_clientes_conectados(cliente);
				log_rta_EnviarDataCliente(cliente);
				break;
        	case CONSULTAR_RESTAURANTES:;
				t_list *restaurantes;
				if (list_is_empty(rests_conectados)) {
					restaurantes = getEmptyRestsConectados(rest_default);
				} else {
					restaurantes = obtener_rests_conectados();
				}
				enviarPaquete(socketCliente, APP, RTA_CONSULTAR_RESTAURANTES, restaurantes);
				list_destroy(restaurantes); // O free?
        		break;
			case SELECCIONAR_RESTAURANTE:;
				t_selecc_rest *seleccion = recibirPayloadPaquete(header, socketCliente);
				t_result *result_selecc_rest;
				if (!es_valido_restaurante(seleccion->restSelecc)) {
					result_selecc_rest = getTResult(REST_NO_CONECTADO, true);
				} else {
					bind_client_to_rest(cliente, seleccion->restSelecc);
					log_SeleccionarRestaurante(seleccion);
					result_selecc_rest = getTResult(REST_ASOCIADO, false);
				}
				enviarPaquete(socketCliente, APP, RTA_SELECCIONAR_RESTAURANTE, result_selecc_rest);
				free(result_selecc_rest);
				free(seleccion);
				break;
			case CONSULTAR_PLATOS:;
				char *consulta = recibirPayloadPaquete(header, socketCliente); free(consulta);
				if (list_is_empty(rests_conectados)) {
					log_ConsultarPlatos_default(platos_rest_default);
					enviarPaquete(socketCliente, APP, RTA_CONSULTAR_PLATOS, platos_rest_default);		
				} else if (!esta_conectado_rest(cliente->restSelecc)) {
					t_list *list_with_error = getEmptyPlatos_with_error(REST_NO_CONECTADO);
					enviarPaquete(socketCliente, APP, RTA_CONSULTAR_PLATOS, list_with_error);
					list_destroy(list_with_error); // O free?
				} else {
					t_cliente *rest_conectado = get_rest_conectado(cliente->restSelecc);
					log_ConsultarPlatos_a_rest(rest_conectado->idCliente);
					enviarPaquete(rest_conectado->socketCliente, APP, CONSULTAR_PLATOS, string_new());
					
					t_header *header_ConsultarPlatos = recibirHeaderPaquete(rest_conectado->socketCliente);
					t_list *platos_rest = recibirPayloadPaquete(header_ConsultarPlatos, rest_conectado->socketCliente);
					enviarPaquete(socketCliente, APP, RTA_CONSULTAR_PLATOS, platos_rest);
					log_rta_ConsultarPlatos(platos_rest);

					list_destroy(platos_rest); // O free?
					free(header_ConsultarPlatos);
				}
				break;	
			case CREAR_PEDIDO:;
				conexionComanda = conectarseA(COMANDA);
				int new_id_pedido = ERROR;
				t_request *req_guardar_pedido;
				t_result *result_crear_pedido;

				if (list_is_empty(rests_conectados)) {
					new_id_pedido = generar_id(0, 100);
					req_guardar_pedido = getTRequest(new_id_pedido, rest_default);
				} else {
					t_cliente *rest_a_crear_pedido = get_rest_conectado(cliente->restSelecc);
					log_CrearPedido_app(cliente->idCliente, rest_a_crear_pedido->idCliente);
					enviarPaquete(rest_a_crear_pedido->socketCliente, APP, CREAR_PEDIDO, NULL);
					
					t_header *h_crear_pedido = recibirHeaderPaquete(rest_a_crear_pedido->socketCliente);
					new_id_pedido = recibirPayloadPaquete(h_crear_pedido, rest_a_crear_pedido->socketCliente);
					log_rta_CrearPedido(new_id_pedido);
					
					req_guardar_pedido = getTRequest(new_id_pedido, rest_a_crear_pedido->idCliente);
					
					free(h_crear_pedido);
				}
				

				if (new_id_pedido != ERROR) {
					// Si el restaurante no devolvió ERROR, le pedimos a COMANDA que guarde el pedido y le enviamos el resultado a CLIENTE
					enviarPaquete(conexionComanda, APP, GUARDAR_PEDIDO, req_guardar_pedido);
					t_header *h_guardar_pedido = recibirHeaderPaquete(conexionComanda);
					t_result *result_guardar_pedido = recibirPayloadPaquete(h_guardar_pedido, conexionComanda);
					logTResult(result_guardar_pedido);

					enviarPaquete(socketCliente, APP, RTA_CREAR_PEDIDO, result_guardar_pedido->hasError ? ERROR : new_id_pedido);
					free(h_guardar_pedido);
					free(result_guardar_pedido);
				} else {
					enviarPaquete(socketCliente, APP, RTA_CREAR_PEDIDO, req_guardar_pedido->idPedido);
				}
				free(req_guardar_pedido);
				liberarConexion(conexionComanda);
				break;	
			case ANIADIR_PLATO:;
				conexionComanda = conectarseA(COMANDA);
				t_request *req_aniadir_plato = recibirPayloadPaquete(header, socketCliente);
				logRequest(req_aniadir_plato, header->codigoOperacion);

				t_header *h_aniadir_plato;
				t_result *result_aniadir_plato;
				t_req_plato *req_plato = getTReqPlato(cliente->restSelecc, req_aniadir_plato->idPedido, req_aniadir_plato->nombre, 1);

				if (string_equals_ignore_case(cliente->restSelecc, rest_default)) {
					
					// Si es rest_default, enviamos el t_request a COMANDA y después al CLIENTE
					enviarPaquete(conexionComanda, APP, GUARDAR_PLATO, req_plato);
					h_aniadir_plato = recibirHeaderPaquete(conexionComanda);
					result_aniadir_plato = recibirPayloadPaquete(h_aniadir_plato, conexionComanda);
					logTResult(result_aniadir_plato);
					enviarPaquete(socketCliente, APP, RTA_ANIADIR_PLATO, result_aniadir_plato);

				} else if (!esta_conectado_rest(cliente->restSelecc)) {
					result_aniadir_plato = getTResult(REST_NO_CONECTADO, true);
					enviarPaquete(socketCliente, APP, RTA_CONSULTAR_PLATOS, result_aniadir_plato); 
				} else {

					// Sino, enviamos t_request al RESTAURANTE dueño del pedido
					t_cliente *rest_a_aniadir_plato = get_rest_conectado(cliente->restSelecc);
					enviarPaquete(rest_a_aniadir_plato->socketCliente, APP, ANIADIR_PLATO, req_aniadir_plato);

					h_aniadir_plato = recibirHeaderPaquete(rest_a_aniadir_plato->socketCliente);
					result_aniadir_plato = recibirPayloadPaquete(h_aniadir_plato, rest_a_aniadir_plato->socketCliente);
					logTResult(result_aniadir_plato);
					
					if (result_aniadir_plato->hasError) {
						// Si el RESTAURANTE dice que falló, informamos directamente al CLIENTE
						enviarPaquete(socketCliente, APP, RTA_ANIADIR_PLATO, result_aniadir_plato);
					} else {
						// Sino, enviamos el t_request a COMANDA e informamos el resultado al CLIENTE
						enviarPaquete(conexionComanda, APP, GUARDAR_PLATO, req_plato);
						h_aniadir_plato = recibirHeaderPaquete(conexionComanda);
						result_aniadir_plato = recibirPayloadPaquete(h_aniadir_plato, conexionComanda);
						logTResult(result_aniadir_plato);
						enviarPaquete(socketCliente, APP, RTA_ANIADIR_PLATO, result_aniadir_plato);
					}
				}

				free(req_plato);
				free(req_aniadir_plato);
				free(h_aniadir_plato);
				free(result_aniadir_plato);
				liberarConexion(conexionComanda);
				break;	
			case PLATO_LISTO:;
				t_plato_listo *plato_listo = recibirPayloadPaquete(header, socketCliente);
				
				enviarPaquete(conexionComanda, APP, PLATO_LISTO, plato_listo);
				t_header *h_plato_listo = recibirHeaderPaquete(conexionComanda);
				t_result *result_plato_listo = recibirPayloadPaquete(h_plato_listo, conexionComanda);
				logTResult(result_plato_listo);

				if (!result_plato_listo->hasError) {
					// Obtener pedido de COMANDA para verificar cantidad de platos listos
					t_request *req_obtener_pedido = getTRequest(plato_listo->restaurante, plato_listo->idPedido);
					t_pedido *pedido_a_verificar = get_pedido_from_comanda(req_obtener_pedido);
					if (todos_platos_listos(pedido_a_verificar->platos)) {
						desbloquear_PCB(req_obtener_pedido->idPedido); // Si o si va a estar bloqueado?
					}
				}
				
				enviarPaquete(socketCliente, APP, RTA_PLATO_LISTO, result_plato_listo);
				free(result_plato_listo);
				free(h_plato_listo);
				free(plato_listo);
				break;	
			case CONFIRMAR_PEDIDO:;
				t_request *req_confirmar_pedido = recibirPayloadPaquete(header, socketCliente);
				logRequest(req_confirmar_pedido, header->codigoOperacion);
				req_confirmar_pedido->nombre = cliente->restSelecc;
				conexionComanda = conectarseA(COMANDA);

				// Obtener el pedido de COMANDA
				enviarPaquete(conexionComanda, APP, OBTENER_PEDIDO, req_confirmar_pedido);
				t_header *h_confirmar_pedido = recibirHeaderPaquete(conexionComanda);
				t_pedido *pedido_a_confirmar = recibirPayloadPaquete(h_confirmar_pedido, conexionComanda);
				log_rta_ObtenerPedido(pedido_a_confirmar, req_confirmar_pedido);

				t_pcb *pcb;
				t_result *result_confirmar_pedido;

				// Si es RDefault, generar PCB y añadirlo para planificar, sino, consultar al restaurante que corresponda
				if (string_equals_ignore_case(cliente->restSelecc, rest_default)) {
					result_confirmar_pedido = getTResult(PEDIDO_ACTUALIZADO, false);
				} else {
					t_cliente *rest_a_conf_pedido = get_rest_conectado(req_confirmar_pedido->nombre);
					enviarPaquete(rest_a_conf_pedido->socketCliente, APP, CONFIRMAR_PEDIDO, req_confirmar_pedido);
					h_confirmar_pedido = recibirHeaderPaquete(rest_a_conf_pedido->socketCliente);
					result_confirmar_pedido = recibirPayloadPaquete(h_confirmar_pedido, rest_a_conf_pedido->socketCliente);
					logTResult(result_confirmar_pedido);
				}

				if (result_confirmar_pedido->hasError) {
					enviarPaquete(socketCliente, APP, RTA_CONFIRMAR_PEDIDO, result_confirmar_pedido);
				} else {
					pcb = crear_pcb(cliente, req_confirmar_pedido->idPedido);
					agregar_a_QN(pcb);
					// Informar a COMANDA la actualización del estado del pedido
					enviarPaquete(conexionComanda, APP, CONFIRMAR_PEDIDO, req_confirmar_pedido);
					h_confirmar_pedido = recibirHeaderPaquete(conexionComanda);
					result_confirmar_pedido = recibirPayloadPaquete(h_confirmar_pedido, conexionComanda);
					logTResult(result_confirmar_pedido);

					enviarPaquete(socketCliente, APP, RTA_CONFIRMAR_PEDIDO, result_confirmar_pedido);
				}

				free(req_confirmar_pedido);
				free(h_confirmar_pedido);
				free(result_confirmar_pedido);
				liberarConexion(conexionComanda);
				break;
			case CONSULTAR_PEDIDO:;
				int id_pedido_a_consultar = recibirPayloadPaquete(header, socketCliente);
				conexionComanda = conectarseA(COMANDA);
				
				enviarPaquete(conexionComanda, APP, CONSULTAR_PEDIDO, id_pedido_a_consultar);
				t_header *h_consultar_pedido = recibirHeaderPaquete(conexionComanda);
				t_pedido *pedido_consultado = recibirPayloadPaquete(h_consultar_pedido, conexionComanda);
				log_rta_ConsultarPedido(pedido_consultado, id_pedido_a_consultar);

				enviarPaquete(socketCliente, APP, RTA_CONSULTAR_PEDIDO, pedido_consultado);
				free(h_consultar_pedido);
				free(pedido_consultado);
				liberarConexion(conexionComanda);
				break;			
        	default:
            	printf("Operación desconocida. Llegó el código: %d. No quieras meter la pata!!!(｀Д´*)\n", header->codigoOperacion);
            	break;
 	   	}
		// free(header);
	}
	// TODO: Liberar conexion a comanda y sacar al cliente de la lista de conectados
    pthread_exit(EXIT_SUCCESS);
    return 0;
}

int main(int argc, char* argv[])
{
	inicializarProceso(APP);
	socketServidor = iniciarServidor();
	initApp();

	pthread_create(&threadPlanificacion, NULL, (void*)planificar, NULL);

	int fd;
	while (1) {
		fd = aceptarCliente(socketServidor);
		if (fd != ERROR) {
			// Creo un nuevo hilo para la conexión aceptada
			pthread_data *t_data = (pthread_data *) malloc(sizeof(*t_data));
			t_data->socketThread = fd;
			pthread_create(&threadConexiones, NULL, (void*)atenderConexiones, t_data);
			pthread_detach(threadConexiones);
			log_new_client_connection(fd);
		} else {
			pthread_kill(threadConexiones, SIGTERM);
		}
	}
	
	pthread_kill(threadPlanificacion, SIGTERM);
	liberarConexion(socketServidor);
    finalizarProceso();
    return EXIT_SUCCESS;
}