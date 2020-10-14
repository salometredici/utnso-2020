#include "../include/shared_logging.h"

/* Formatted logs */

void logInitializedProcess() {
	printf("\033[1mProceso \033[0m\033[1;34m%s\033[0m\033[1m iniciado...\033[0m\n", getStringKeyValue(process, PROCNKEYS));
	log_info(logger, "Proceso %s iniciado...", getStringKeyValue(process, PROCNKEYS));
}

void logConnectionAttempt(p_code process, char *ip, int puerto) {
	if (process != CLIENTE) {
		printf("Intentando conectarse a \033[1;34m%s\033[0m en IP: %s, PUERTO: %d...\n", getStringKeyValue(process, PROCNKEYS), ip, puerto);
		log_info(logger, "Intentando conectarse a %s en IP: %s, PUERTO: %d...", getStringKeyValue(process, PROCNKEYS), ip, puerto);
	} else {
		printf("Intentando conectarse a IP: %s, PUERTO: %d...\n", ip, puerto);
		log_info(logger, "Intentando conectarse a IP: %s, PUERTO: %d...", ip, puerto);
	}
}

void logConnectionSuccess(p_code process, int puerto) {
	if (process != CLIENTE) {
		printf("\033[1;32mConexión creada con \033[0m\033[1;34m%s\033[0m\033[1;32m en el puerto %d\033[0m\n", getStringKeyValue(process, PROCNKEYS), puerto);
		log_info(logger, "Conexión creada con %s en el puerto %d", getStringKeyValue(process, PROCNKEYS), puerto);
	} else {
		printf("\033[1;32mConexión creada con el puerto %d\033[0m\n", puerto);
		log_info(logger, "Conexión creada con el puerto %d", puerto);
	}
}

void logConnectionCliente(p_code process) {
	printf("Conectado al proceso \033[1;34m%s\033[0m\n", getStringKeyValue(process, PROCNKEYS));
	log_info("Conectado al proceso %s", getStringKeyValue(process, PROCNKEYS));
}

void logAwaitingConnections(int puerto) {
	printf("\033[1;34m%s\033[0m\033[1;32m escuchando conexiones en el puerto %d\033[0m\n", getStringKeyValue(process, PROCNKEYS), puerto);
	log_info(logger, "%s escuchando conexiones en el puerto %d", getStringKeyValue(process, PROCNKEYS), puerto);
}

void logConsoleInput(char *input) {
	log_debug(logger, "Comando ingresado: %s", input);
}

void logClientDisconnection(int socketCliente) {
	printf("\033[0;31mEl cliente %d se desconectó. Finalizando el hilo...（・∩・)\033[0m\n", socketCliente);
	log_info(logger, "El cliente %d se desconectó. Finalizando el hilo...（・∩・)", socketCliente);
}

void logNewClientConnection(int socket) {
	printf("\033[1;32mNuevo hilo para atender al cliente \033[0m\033[1;34m%d\033[0m\033[1;32m\033[0m\n", socket);
	log_info(logger, "Nuevo hilo para atender al cliente %d", socket);
}

/* Serialization logs */

void logListaRecetas(t_list *listaRecetas) {
	int cantidadRecetas = list_size(listaRecetas);
	for (int i = 0; i < cantidadRecetas; i++) {
		t_md_receta *recetaActual = list_get(listaRecetas, i);
		printf("\t\033[1m[Receta #%d]\033[0m\n");
		printf("\tPlato: \033[1m[%s]\033[0m\n", recetaActual->plato);
		printf("\tPrecio: \033[1m$%d\033[0m\n", recetaActual->precio);
		log_info(logger, "Receta [%d] - Plato: [%s], Precio: $%d", i, recetaActual->plato, recetaActual->precio);
		free(recetaActual);
	}
}

void logMetadata(md_restaurante *md) {//(t_posicion *posicion) {
	// printf("\tUbicación del restaurante: posX: %d, posY: %d\n", posicion->posX, posicion->posY);
	// log_info(logger, "Ubicación del restaurante: posX: %d, posY: %d", posicion->posX, posicion->posY);
	printf("\033[1m[Metadata del restaurante]:\033[0m\n");
	log_info(logger, "Metadata del restaurante:");
	printf("\tCantidad de cocineros: \033[1m%d\033[0m\n", md->cantidadCocineros);
	printf("\tCantidad de hornos: \033[1m%d\033[0m\n", md->cantidadHornos);
	printf("\tCantidad de pedidos: \033[1m%d\033[0m\n", md->cantidadPedidos);
	printf("\tPosición del restaurante: \033[1m[%d,%d]\033[0m\n", md->posX, md->posY);
	log_info(logger, "\tCantidad de cocineros: %d", md->cantidadCocineros);
	log_info(logger, "\tCantidad de hornos: %d", md->cantidadHornos);
	log_info(logger, "\tCantidad de pedidos: %d", md->cantidadPedidos);
	log_info(logger, "\tPosición del restaurante: [%d,%d]", md->posX, md->posY);
	printf("\t\033[1mAfinidades:\033[0m\n");
	log_info(logger, "\tAfinidades:");
	mostrarListaStrings(md->afinidades);
	printf("\t\033[1mRecetas:\033[1m\n");
	logListaRecetas(md->platos);
}

void logHeader(m_code codigoOperacion, p_code procesoOrigen) {
	printf("\033[1;33m[HEADER]\033[0m Received \033[0;33m%s\033[0m from \033[1;34m%s\033[0m\n",
		getStringKeyValue(codigoOperacion, COMMANDNKEYS),
		getStringKeyValue(procesoOrigen, PROCNKEYS));
	log_info(logger, "[HEADER] Received %s from %s",
		getStringKeyValue(codigoOperacion, COMMANDNKEYS),
		getStringKeyValue(procesoOrigen, PROCNKEYS));
}

void logMessageSent(m_code codigoOperacion) {
	printf("Message \033[0;33m%s\033[0m sent\n", getStringKeyValue(codigoOperacion, COMMANDNKEYS));
	log_info(logger, "Message %s sent", getStringKeyValue(codigoOperacion, COMMANDNKEYS));
}

void logMetadataRequest(char *nombreRestaurante) {
	printf("\tMetadata requested by \033[0;35m%s\033[0m\n", nombreRestaurante);
	log_info(logger, "Metadada requested by %s", nombreRestaurante);
}

/* t_request */

void logReqRestaurante(t_request *reqRestaurante) {
	printf("Datos del request:\n");
	if (!string_is_empty(reqRestaurante->nombre)) {
		printf("\tRestaurante: \033[1;35m%s\033[0m\n", reqRestaurante->nombre);
	}
	printf("\tPedido: \033[1m%d\033[0m\n", reqRestaurante->idPedido);
	log_info(logger, "Datos del request:");
	if (!string_is_empty(reqRestaurante->nombre)) {
		log_info(logger, "Restaurante: %s, Pedido: %d", reqRestaurante->nombre, reqRestaurante->idPedido);
	} else {
		log_info(logger, "Pedido: %d", reqRestaurante->idPedido);
	}
}

void logReqPlato(t_request *reqPlato) {
	printf("Datos del request:\n");
	printf("\tPlato: \033[1m[%s]\033[0m\n", reqPlato->nombre);
	printf("\tPedido: \033[1m%d\033[0m\n", reqPlato->idPedido);
	log_info(logger, "Datos del request:");
	log_info(logger, "Plato: %s, Pedido: %d", reqPlato->nombre, reqPlato->idPedido);
}

void logRequest(t_request *request, m_code codigoOperacion) {
	switch (codigoOperacion) {
		case GUARDAR_PEDIDO:
		case OBTENER_PEDIDO:
		case TERMINAR_PEDIDO:
		case CONFIRMAR_PEDIDO: // En este caso sólo si se envía a comanda o a sindicato tendrá nombreRestaurante
		case FINALIZAR_PEDIDO:
			logReqRestaurante(request);
			break;
		case ANIADIR_PLATO:
			logReqPlato(request);
			break;
		default:
			break;
	}
}

/* t_req_plato */

void logRequestPlato(t_req_plato *plato) {
	printf("Datos del request del plato:\n");
	printf("\tRestaurante: \033[1;35m%s\033[0m\n", plato->restaurante);
	printf("\tPedido: \033[1m%d\033[0m\n", plato->idPedido);
	printf("\tPlato: [\033[1m%s\033[0m]\n", plato->plato);
	printf("\tCantidad plato: \033[1m%d\033[0m\n", plato->cantidadPlato);
	log_info(logger, "Restaurante: %s, Pedido: %d, Plato: %s, Cantidad plato: %d", plato->restaurante, plato->idPedido, plato->plato, plato->cantidadPlato);
}

void logConsultaPlatos(char *restaurante) {
	printf("\tRestaurante: \033[1;35m%s\033[0m\n", restaurante);
	log_info(logger, "\tRestaurante %s", restaurante);
}