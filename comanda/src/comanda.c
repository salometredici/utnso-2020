#include "../include/comanda.h"

void *atenderConexiones(void *conexionNueva)
{
    pthread_data *t_data = (pthread_data*) conexionNueva;
    int socketCliente = t_data->socketThread;
    free(t_data);
	p_code proceso_conectado = ERROR;
	t_cliente *cliente_conectado = malloc(sizeof(t_cliente));
	
	while (1) {

        t_header *header = recibirHeaderPaquete(socketCliente);

		if (header->procesoOrigen == ERROR || header->codigoOperacion == ERROR) {
			log_comanda_client_disconnection(proceso_conectado, socketCliente, cliente_conectado->idCliente);
			liberarConexion(socketCliente);
    		pthread_exit(EXIT_SUCCESS);
			free(cliente_conectado);
			return EXIT_FAILURE;
		} else {
			proceso_conectado = header->procesoOrigen;
		}
		
		switch (header->codigoOperacion) {
			case OBTENER_PROCESO:;
				enviarPaquete(socketCliente, COMANDA, RTA_OBTENER_PROCESO, COMANDA);
				break;
			case ENVIAR_DATACLIENTE:;
				cliente_conectado = recibirPayloadPaquete(header, socketCliente);
				cliente_conectado->socketCliente = socketCliente;
				log_DataCliente(cliente_conectado);
				break;
			case GUARDAR_PEDIDO:;
				t_request *request_gp = recibirPayloadPaquete(header, socketCliente);
				logRequest(request_gp, header->codigoOperacion);
				
				t_result *result_gp = _guardar_pedido(request_gp);
				enviarPaquete(socketCliente, COMANDA, RTA_GUARDAR_PEDIDO, result_gp);

				free_t_request(request_gp);
				free_t_result(result_gp);
				break;
			case GUARDAR_PLATO:;
				t_req_plato *request_gpl = recibirPayloadPaquete(header, socketCliente);
				log_GuardarPlato(request_gpl);

				t_result *result_gpl = _guardar_plato(request_gpl);
				free_t_req_plato(request_gpl);
				enviarPaquete(socketCliente, COMANDA, RTA_GUARDAR_PLATO, result_gpl);
				free_t_result(result_gpl);
				break;
			case OBTENER_PEDIDO:;
				t_request *request_obp = recibirPayloadPaquete(header, socketCliente);
				log_ObtenerPedido(request_obp, header->codigoOperacion);
		
				t_pedido *pedido = _obtener_pedido(request_obp);
				enviarPaquete(socketCliente, COMANDA, RTA_OBTENER_PEDIDO, pedido);
				free_t_request(request_obp);
				list_destroy_and_destroy_elements(pedido->platos, &free);
				free(pedido);
				break;
			case CONFIRMAR_PEDIDO:;
				t_request *request_conf = recibirPayloadPaquete(header, socketCliente);
				logRequest(request_conf, header->codigoOperacion);
			
				t_result *result_conf = _confirmar_pedido(request_conf);
				free_t_request(request_conf);
				enviarPaquete(socketCliente, COMANDA, RTA_CONFIRMAR_PEDIDO, result_conf);
				free_t_result(result_conf);
				break;
			case PLATO_LISTO:;
				t_plato_listo *request_pl = recibirPayloadPaquete(header, socketCliente);
				log_PlatoListo(request_pl);
				
				t_result *result_pl = _plato_listo(request_pl);
				free_t_req_plato(request_pl);
				enviarPaquete(socketCliente, COMANDA, RTA_PLATO_LISTO, result_pl);
				free_t_result(result_pl);
				break;
			case FINALIZAR_PEDIDO:;
				t_request *request_fin = recibirPayloadPaquete(header, socketCliente);
				log_FinalizarPedido(request_fin, header->codigoOperacion);
				
				t_result *result_fin = _finalizar_pedido(request_fin);
				free_t_request(request_fin);				
				enviarPaquete(socketCliente, COMANDA, RTA_FINALIZAR_PEDIDO, result_fin);
				free_t_result(result_fin);
				break;
			default:
				printf("Operación desconocida. Llegó el código: %d. No quieras meter la pata!!!(｀Д´*)\n", header->codigoOperacion);
				break;
		}
		free(header);
	}
	
    pthread_exit(EXIT_SUCCESS);
    return 0;
}

int main(int argc, char **argv) {
	
	inicializarProceso(COMANDA);
    socketServidor = iniciarServidor();
	
	init_comanda();
	print_init_structure();
    
	int fd;
    while (1) {
        fd = aceptarCliente(socketServidor);
        if (fd != -1) {
			pthread_data *t_data = (pthread_data *) malloc(sizeof(*t_data));
			t_data->socketThread = fd;
			pthread_create(&threadConexiones, NULL, (void*)atenderConexiones, t_data);
			pthread_detach(threadConexiones);
			log_new_client_connection(fd);
		} else {
			pthread_kill(threadConexiones, SIGTERM);
		}
    }

    liberarConexion(socketServidor);
    finalizarProceso();
    return EXIT_SUCCESS;
}