#include "../include/shared_logging.h"

/* APP */

void logClientInfo(t_cliente *cliente) {
	printf(TAB"Cliente: "BOLD"[%s]"RESET", Socket: "BOLD"[%d]"RESET", ¿Es un restaurante?: %s" BREAK, cliente->idCliente, cliente->socketCliente, cliente->esRestaurante ? "Sí" : "No");
	printf(TAB"[Posición en el mapa]: [%d,%d]"BREAK, cliente->posCliente->posX, cliente->posCliente->posY);
	log_info(logger, TAB"Cliente: %s, Socket: %d, ¿Es un restaurante?: %s", cliente->idCliente, cliente->socketCliente, cliente->esRestaurante ? "Sí" : "No");
	log_info(logger, TAB"[Posición en el mapa]: [%d,%d]", cliente->posCliente->posX, cliente->posCliente->posY);
}

void logRtaConsultarPlatos(t_list *platosEnviados) {
	printf("Platos enviados:"BREAK);
	log_info(logger, "Platos enviados:");
	for (int i = 0; i < list_size(platosEnviados); i++) {
		printf(TAB"Plato #%d: %s"BREAK, i, list_get(platosEnviados, i));
		log_info(logger, TAB"Plato #%d: %s", i, list_get(platosEnviados, i));
	}
}

/* Formatted logs */

void logInitializedProcess() {
	printf(BOLD"Proceso "RESET BOLDBLUE"%s"RESET BOLD" iniciado..."RESET BREAK, getStringKeyValue(process, PROCNKEYS));
	log_info(logger, "Proceso %s iniciado...", getStringKeyValue(process, PROCNKEYS));
}

void logConnectionAttempt(p_code process, char *ip, int puerto) {
	if (process != CLIENTE) {
		printf("Intentando conectarse a "BOLDBLUE"%s"RESET" en IP: %s, PUERTO: %d..."BREAK, getStringKeyValue(process, PROCNKEYS), ip, puerto);
		log_info(logger, "Intentando conectarse a %s en IP: %s, PUERTO: %d...", getStringKeyValue(process, PROCNKEYS), ip, puerto);
	} else {
		printf("Intentando conectarse a IP: %s, PUERTO: %d..."BREAK, ip, puerto);
		log_info(logger, "Intentando conectarse a IP: %s, PUERTO: %d...", ip, puerto);
	}
}

void logConnectionSuccess(p_code process, int puerto) {
	if (process != CLIENTE) {
		printf(BOLDGREEN"Conexión creada con "RESET BOLDBLUE"%s"RESET BOLDGREEN" en el puerto %d"RESET BREAK, getStringKeyValue(process, PROCNKEYS), puerto);
		log_info(logger, "Conexión creada con %s en el puerto %d", getStringKeyValue(process, PROCNKEYS), puerto);
	} else {
		printf(BOLDGREEN"Conexión creada con el puerto %d"RESET BREAK, puerto);
		log_info(logger, "Conexión creada con el puerto %d", puerto);
	}
}

void logConnectionCliente(p_code process) {
	printf("Conectado al proceso "BOLDBLUE"%s"RESET BREAK, getStringKeyValue(process, PROCNKEYS));
	log_info("Conectado al proceso %s", getStringKeyValue(process, PROCNKEYS));
}

void logAwaitingConnections(int puerto) {
	printf(BOLDBLUE"%s"RESET BOLDGREEN" escuchando conexiones en el puerto %d"RESET BREAK, getStringKeyValue(process, PROCNKEYS), puerto);
	log_info(logger, "%s escuchando conexiones en el puerto %d", getStringKeyValue(process, PROCNKEYS), puerto);
}

void logConsoleInput(char *input) {
	log_debug(logger, "Comando ingresado: %s", input);
}

void logClientDisconnection(int socketCliente) {
	printf(RED"El cliente %d se desconectó. Finalizando el hilo...（・∩・)"RESET BREAK, socketCliente);
	log_info(logger, "El cliente %d se desconectó. Finalizando el hilo...（・∩・)", socketCliente);
}

void logNewClientConnection(int socket) {
	printf(BOLDGREEN"Nuevo hilo para atender al cliente "RESET BOLDBLUE"%d"RESET BREAK, socket);
	log_info(logger, "Nuevo hilo para atender al cliente %d", socket);
}

/* Serialization logs */

void logListaRecetas(t_list *listaRecetas) {
	int cantidadRecetas = list_size(listaRecetas);
	for (int i = 0; i < cantidadRecetas; i++) {
		t_md_receta *recetaActual = list_get(listaRecetas, i);
		printf(TAB BOLD"[Receta #%d]"RESET BREAK);
		printf(TAB"Plato: "BOLD"[%s]"RESET BREAK, recetaActual->plato);
		printf(TAB"Precio: "BOLD"$%d"RESET BREAK, recetaActual->precio);
		log_info(logger, "Receta [%d] - Plato: [%s], Precio: $%d", i, recetaActual->plato, recetaActual->precio);
		free(recetaActual);
	}
}

/* CONSULTAR_PEDIDO: t_pedido CON nombre de restaurante */

void logConsultarPedido(t_pedido *pedido, int idPedido) {
	printf("Pedido #%d:"BREAK, idPedido);
	log_info(logger, "Pedido #%d:", idPedido);
	printf(TAB"Restaurante: %s, Estado: %s, Precio Total: $%d"BREAK, pedido->restaurante, getStringEstadoPedido(pedido->estado), pedido->precioTotal);
	log_info(logger, TAB"Restaurante: %s, Estado: %s, Precio Total: $%d", pedido->restaurante, getStringEstadoPedido(pedido->estado), pedido->precioTotal);
	mostrarListaPlatos(pedido->platos);
}

/* OBTENER_PEDIDO: t_pedido SIN nombre de restaurante */

void logObtenerPedido(t_pedido *pedido, int idPedido) {
	printf("Pedido #%d:"BREAK, idPedido);
	log_info(logger, "Pedido #%d:", idPedido);
	printf(TAB"Estado: %s, Precio Total: $%d"BREAK, getStringEstadoPedido(pedido->estado), pedido->precioTotal);
	log_info(logger, TAB"Estado: %s, Precio Total: $%d", getStringEstadoPedido(pedido->estado), pedido->precioTotal);
	mostrarListaPlatos(pedido->platos);
}

/* t_cliente */

void logInitDataCliente(t_cliente *cliente) {
	log_info("Cliente %s iniciado, PosX: %d, PosY: %d", cliente->idCliente, cliente->posCliente->posX, cliente->posCliente->posY);
}

/* t_md */

void logMetadata(t_md *md) {
	printf(BOLD"[Metadata del restaurante]:"RESET BREAK);
	log_info(logger, "Metadata del restaurante:");
	printf(TAB"Cantidad de cocineros: "BOLD"%d"RESET BREAK, md->cantidadCocineros);
	printf(TAB"Cantidad de hornos: "BOLD"%d"RESET BREAK, md->cantidadHornos);
	printf(TAB"Cantidad de pedidos: "BOLD"%d"RESET BREAK, md->cantidadPedidos);
	printf(TAB"Posición del restaurante: "BOLD"[%d,%d]"RESET BREAK, md->posX, md->posY);
	log_info(logger, TAB"Cantidad de cocineros: %d", md->cantidadCocineros);
	log_info(logger, TAB"Cantidad de hornos: %d", md->cantidadHornos);
	log_info(logger, TAB"Cantidad de pedidos: %d", md->cantidadPedidos);
	log_info(logger, TAB"Posición del restaurante: [%d,%d]", md->posX, md->posY);
	printf(TAB BOLD"Afinidades:"RESET BREAK);
	log_info(logger, TAB"Afinidades:");
	mostrarListaStrings(md->afinidades);
	printf(TAB BOLD"Recetas:"RESET BREAK);
	logListaRecetas(md->platos);
}

void logSeleccionarRestaurante(t_selecc_rest *seleccion) {
	printf("Se ha asociado al cliente "BOLD"%s"RESET" con el restaurante "BOLDMAGENTA"%s"RESET BREAK, seleccion->idCliente, seleccion->restauranteSeleccionado);
	log_info(logger, "Se ha asociado al cliente %s con el restaurante %s", seleccion->idCliente, seleccion->restauranteSeleccionado);
}

/* t_result */

void logTResult(t_result *result) {
	printf("Resultado "BOLD"[%s]"RESET": %s"BREAK, result->hasError ? "FAILED" : "SUCCESS", result->msg);
	log_info(logger, "Resultado [%s]: %s", result->hasError ? "FAILED" : "SUCCESS", result->msg);
}

void logHeader(m_code codigoOperacion, p_code procesoOrigen) {
	printf(BOLDYELLOW"[HEADER]"RESET" Received "YELLOW"%s"RESET" from "BOLDBLUE"%s"RESET BREAK,
		getStringKeyValue(codigoOperacion, COMMANDNKEYS),
		getStringKeyValue(procesoOrigen, PROCNKEYS));
	log_info(logger, "[HEADER] Received %s from %s",
		getStringKeyValue(codigoOperacion, COMMANDNKEYS),
		getStringKeyValue(procesoOrigen, PROCNKEYS));
}

void logMessageSent(m_code codigoOperacion) {
	printf("Message "YELLOW"%s"RESET" sent"BREAK, getStringKeyValue(codigoOperacion, COMMANDNKEYS));
	log_info(logger, "Message %s sent", getStringKeyValue(codigoOperacion, COMMANDNKEYS));
}

/* t_request */

void logReqRestaurante(t_request *reqRestaurante) {
	printf("Datos del request:"BREAK);
	if (!string_is_empty(reqRestaurante->nombre)) {
		printf(TAB"Restaurante: "BOLDMAGENTA"%s"RESET BREAK, reqRestaurante->nombre);
	}
	printf(TAB"Pedido: "BOLD"%d"RESET BREAK, reqRestaurante->idPedido);
	log_info(logger, "Datos del request:");
	if (!string_is_empty(reqRestaurante->nombre)) {
		log_info(logger, "Restaurante: %s, Pedido: %d", reqRestaurante->nombre, reqRestaurante->idPedido);
	} else {
		log_info(logger, "Pedido: %d", reqRestaurante->idPedido);
	}
}

void logReqPlato(t_request *reqPlato) {
	printf("Datos del request:"BREAK);
	printf(TAB"Plato: "BOLD"[%s]"RESET BREAK, reqPlato->nombre);
	printf(TAB"Pedido: "BOLD"%d"RESET BREAK, reqPlato->idPedido);
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
	printf("Datos del request del plato:"BREAK);
	printf(TAB"Restaurante: "BOLDMAGENTA"%s"RESET BREAK, plato->restaurante);
	printf(TAB"Pedido: "BOLD"%d"RESET BREAK, plato->idPedido);
	printf(TAB"Plato: ["BOLD"%s]"RESET BREAK, plato->plato);
	printf(TAB"Cantidad plato: "BOLD"%d"RESET BREAK, plato->cantidadPlato);
	log_info(logger, "Restaurante: %s, Pedido: %d, Plato: %s, Cantidad plato: %d", plato->restaurante, plato->idPedido, plato->plato, plato->cantidadPlato);
}

/* Sindicato */

// Atender conexiones

void log_metadata_request(char *nombreRestaurante) {
	printf(TAB"Metadata requested by "MAGENTA"%s"RESET BREAK, nombreRestaurante);
	log_info(logger, "Metadada requested by %s", nombreRestaurante);
}

void log_ConsultarPlatos(char *restaurante) { // Antes logConsultaPlatos
	printf(TAB"Restaurante: "BOLDMAGENTA"%s"RESET BREAK, restaurante);
	log_info(logger, TAB"Restaurante %s", restaurante);
}

// Consola e inicialización

void log_full_FS(int cantReq, int cantDisp) {
	printf(RED"[ERROR] No hay bloques suficientes para realizar la operación. "RESET BOLDRED "[Cant. requerida: %d] - [Cant. disponible: %d]"RESET BREAK, cantReq, cantDisp);
	log_error(logger, "No hay bloques suficientes para realizar la operación. Cant. requerida: %d. Cant. disponible: %d", cantReq, cantDisp);
}

void log_Info_AFIP(char *rest) {
	printf("Creando el archivo "BOLD"Info.AFIP"RESET" para el restaurante "BOLDMAGENTA"[%d]"RESET"..." BREAK, rest);
	log_debug(logger, "Creando el archivo Info.AFIP para el restaurante [%d]...", rest);
}

void log_Pedido_AFIP(int nroPedido) {
	printf("Creando el archivo "BOLD"Pedido%d.AFIP"RESET"..." BREAK, nroPedido);
	log_debug(logger, "Creando el archivo Pedido%d.AFIP...", nroPedido);
}

void log_Receta_AFIP(char *receta) {
	printf("Creando el archivo "BOLD"%s.AFIP"RESET"..." BREAK, receta);
	log_debug(logger, "Creando el archivo %s.AFIP...", receta);
}

// Bitmap

void logBitmapFileError() {
	printf(RED"[ERROR] Error al abrir el archivo Bitmap.bin"RESET BREAK);
	log_error(logger, "Error al abrir el archivo Bitmap.bin");
}

void logBitmapError() {
	printf(RED"[ERROR] Error al realizar mmap"RESET BREAK);
	log_error(logger, "Error al realizar mmap");
}

void logInitialBitarrayInfo(t_bitarray *bitarray) {
	int lastBit = bitarray_get_max_bit(bitarray);
	printf("Tamaño del bitmap: %d"BREAK, lastBit);
	printf("Valor del primer bit: %d"BREAK, bitarray_test_bit(bitarray, 0));
	printf("Valor del último bit: %d"BREAK, bitarray_test_bit(bitarray, lastBit));
	log_debug(logger, "Tamaño del bitmap: %d", lastBit);
	log_debug(logger, "Valor del primer bit: %d", bitarray_test_bit(bitarray, 0));
	log_debug(logger, "Valor del último bit: %d", bitarray_test_bit(bitarray, lastBit));
}

void logBitmapInit() {
	printf(BOLD"Creando Bitmap.bin..."RESET BREAK);
	log_debug(logger, "Creando Bitmap.bin...");
}

void logBitmapSuccess() {
	printf(BOLD"Archivo Bitmap.bin creado exitosamente"RESET BREAK);
	log_debug(logger, "Archivo Bitmap.bin creado exitosamente");
}

// Bitmap updates

void log_bit_state(int pos, int bit) {
	printf(BOLD"→"RESET" El bit [%d] se encuentra en estado %d"BREAK, pos, bit);
	log_debug(logger, "El bit [%d] se encuentra en estado %d", pos, bit);
}

void log_bit_update(int pos, t_bitarray *bitarray) {
	printf(BOLD"→"RESET" Ahora el bit %d se encuentra en estado %d"BREAK, pos, bitarray_test_bit(bitarray, pos));
	log_debug(logger, "Ahora el bit %d se encuentra en estado %d", pos, bitarray_test_bit(bitarray, pos));
}

void log_unavailable_bit(int pos) {
	printf(TAB BOLD"→"RESET" El bit [%d] está ocupado, buscando al siguiente..."BREAK, pos);
	log_debug(logger, "El bit [%d] está ocupado, buscando al siguiente...", pos);
}

// CREAR_RESTAURANTE

void log_CrearRestaurante_Data(char **params) {
	printf("Creando el archivo "BOLDYELLOW"Info.AFIP"RESET" para el restaurante "BOLDMAGENTA"[%s]"RESET" con los siguientes datos:"BREAK, params[1]);
	printf(TAB"Cant_cocineros: %s, Posicion: %s, Afinidades: %s"BREAK, params[2], params[3], params[4]);
	printf(TAB"Platos: %s, Precios: %s, Cant_hornos: %s, Cant_pedidos: %s"BREAK, params[5], params[6], params[7], params[8]);
	log_info(logger, "Creando el archivo Info.AFIP para el restaurante [%s] con los siguientes datos:", params[1]);
	log_info(logger,TAB "Cant_cocineros: %s, Posicion: %s, Afinidades: %s", params[2], params[3], params[4]);
	log_info(logger,TAB "Platos: %s, Precios: %s, Cant_hornos: %s, Cant_pedidos: %s", params[5], params[6], params[7], params[8]);
}

// CREAR_RECETA

void log_CrearReceta_Data(char **params) {
	printf("Creando el archivo "BOLDYELLOW"%s.AFIP"RESET" con los siguientes datos:"BREAK, params[1]);
	printf(TAB"Pasos: %s, Tiempo_pasos: %s"BREAK, params[2], params[3]);
	log_info(logger, "Creando el archivo %s.AFIP con los siguientes datos:", params[1]);
	log_info(logger, TAB"Pasos: %s, Tiempo_pasos: %s", params[2], params[3]);
}

// CREAR_PEDIDO