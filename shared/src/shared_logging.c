#include "../include/shared_logging.h"

/* Utils */

void log_lista_strings(t_list *lista_strings) {
	int cant_elementos = list_size(lista_strings);
	for (int i = 0; i < cant_elementos; i++) {
		char *palabra = list_get(lista_strings, i);
		printf(TAB"[%d] - %s"BREAK, i, palabra);
		log_info(logger, TAB"[%d] - %s", i, palabra);
	}
}

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

/* Commons */

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

void logHeader(m_code codigoOperacion, p_code procesoOrigen, int socket) {
	printf(BOLDYELLOW"[HEADER]"RESET" Received "YELLOW"%s"RESET" from "BOLDBLUE"%s"RESET" - Socket: %d" BREAK,
		getStringKeyValue(codigoOperacion, COMMANDNKEYS),
		getStringKeyValue(procesoOrigen, PROCNKEYS),
		socket);
	log_info(logger, "[HEADER] Received %s from %s - Socket: %d",
		getStringKeyValue(codigoOperacion, COMMANDNKEYS),
		getStringKeyValue(procesoOrigen, PROCNKEYS),
		socket);
}

void logMessageSent(m_code codigoOperacion) {
	printf("Message "YELLOW"%s"RESET" sent"BREAK, getStringKeyValue(codigoOperacion, COMMANDNKEYS));
	log_info(logger, "Message %s sent", getStringKeyValue(codigoOperacion, COMMANDNKEYS));
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

/* Lista de t_md_receta */

void log_list_t_md_receta(t_list *lista_platos) {
	int cant_platos = list_size(lista_platos);
	for (int i = 0; i < cant_platos; i++) {
		t_md_receta *plato_actual = list_get(lista_platos, i);
		printf(TAB "[Plato #%d] -"BOLD"[%s]"RESET", Precio: "BOLD"$%d"RESET BREAK, i, plato_actual->plato, plato_actual->precio);
		log_info(logger, "[Plato #%d] - [%s], Precio: $%d", i, plato_actual->plato, plato_actual->precio);
		free(plato_actual);
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

/* t_md */ //!!! 

void logMetadata(t_md *md) { // ahora log_rta_obtenerrestaurante
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

/* Lista de t_plato */

void log_t_plato_list(t_list *list) {
	int cant_platos = list_size(list);
	for (int i = 0; i < cant_platos; i++) {
		t_plato *plato_actual = list_get(list, i);
		printf("Plato: "BOLD"[%s]"RESET BREAK, plato_actual->plato);
		printf(TAB"Cantidad pedida: %d"BREAK, plato_actual->cantidadPedida);
		printf(TAB"Cantidad lista: %d"BREAK, plato_actual->cantidadLista);
		log_info(logger, "Plato %d: [%s], Pedido: %d, Listo: %d", i, plato_actual->plato, plato_actual->cantidadPedida, plato_actual->cantidadLista);
		free(plato_actual);
	}
}

/* t_result */

void logTResult(t_result *result) {
	if (result->hasError) {
		printf("Resultado: "BOLDRED"[%s]"RESET" - "RED"%s"RESET BREAK, "FAILED", result->msg);
	} else {
		printf("Resultado: "BOLDGREEN"[%s]"RESET" - "BOLD"%s"RESET BREAK, "SUCCESS", result->msg);
	}
	log_info(logger, "Resultado: [%s] - %s", result->hasError ? "FAILED" : "SUCCESS", result->msg);
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

/* Mensajes */

// TERMINAR_PEDIDO

void log_TerminarPedido(t_request *request, m_code codigo_operacion) {
	logRequest(request, codigo_operacion);
}

void log_rta_TerminarPedido(t_result *result) {
	logTResult(result);
}

// CONFIRMAR_PEDIDO

void log_ConfirmarPedido(t_request *request, m_code codigo_operacion) {
	logRequest(request, codigo_operacion);
}

void log_rta_ConfirmarPedido(t_result *result) {
	logTResult(result);
}

// OBTENER_PEDIDO

void log_ObtenerPedido(t_request *request, m_code codigo_operacion) {
	logRequest(request, codigo_operacion);
}

void log_rta_ObtenerPedido(t_pedido *pedido, t_request *request) {
	switch (pedido->estado) {
		case CONFIRMADO:
		case TERMINADO:
		case FINALIZADO:
			printf("["BOLDCYAN"Pedido #%d"RESET"] - Estado: "BOLD"%s"RESET", Precio Total: "BOLD"$%d"RESET BREAK, request->idPedido, getStringEstadoPedido(pedido->estado), pedido->precioTotal);
			log_info(logger, "[Pedido #%d] - Estado: %s, Precio Total: $%d", request->idPedido, getStringEstadoPedido(pedido->estado), pedido->precioTotal);
			log_t_plato_list(pedido->platos);
		break;
		case REST_INEXISTENTE:
			printf(TAB BOLDRED"[ERROR]"RESET RED" %s - [%s, Pedido #%d]"RESET BREAK, REST_NO_EXISTE, request->nombre, request->idPedido);
			log_error(logger, "Pedido %d, Restaurante %s - %s", request->idPedido, request->nombre, REST_NO_EXISTE);
			break;
		case PEDIDO_INEXISTENTE:
			printf(TAB BOLDRED"[ERROR]"RESET RED" %s - [%s, Pedido #%d]"RESET BREAK, PEDIDO_NO_EXISTE, request->nombre, request->idPedido);
			log_error(logger, "Pedido %d, Restaurante %s - %s", request->idPedido, request->nombre, PEDIDO_NO_EXISTE);
			break;
		case PENDIENTE:
			if (list_is_empty(pedido->platos)) {
				printf(TAB YELLOW"[WARNING]"RESET BOLD" PENDIENTE"RESET" - %s - [%s, Pedido #%d]"BREAK, BLOQUES_NO_ASIGNADOS, request->nombre, request->idPedido);
				log_error(logger, "Pedido %d, Restaurante %s - PENDIENTE - %s", request->idPedido, request->nombre, BLOQUES_NO_ASIGNADOS);
			} else {
				printf("["BOLDCYAN"Pedido #%d"RESET"] - Estado: "BOLD"%s"RESET", Precio Total: "BOLD"$%d"RESET BREAK, request->idPedido, getStringEstadoPedido(pedido->estado), pedido->precioTotal);
				log_info(logger, "[Pedido #%d] - Estado: %s, Precio Total: $%d", request->idPedido, getStringEstadoPedido(pedido->estado), pedido->precioTotal);
				log_t_plato_list(pedido->platos);
			}
			break;
		case SIN_PLATOS: // Revisar
			printf(TAB YELLOW"[WARNING]"RESET" %s - [%s, Pedido #%d]"BREAK, BLOQUES_NO_ASIGNADOS, request->nombre, request->idPedido);
			log_error(logger, "Pedido %d, Restaurante %s - %s", request->idPedido, request->nombre, BLOQUES_NO_ASIGNADOS);
			break;
		default:
			break;
	}
}

// GUARDAR_PLATO

void log_GuardarPlato(t_req_plato *req_plato) {
	printf("Datos del request del plato:"BREAK);
	printf(TAB"Restaurante: "BOLDMAGENTA"[%s]"RESET", Pedido: "BOLD"%d"RESET BREAK, req_plato->restaurante, req_plato->idPedido);
	printf(TAB"Plato: "BOLD"[%s]"RESET", Cant. a añadir: "BOLD"%d"RESET BREAK, req_plato->plato, req_plato->cantidadPlato); 
	log_info(logger, "Restaurante: %s, Pedido: %d, Plato: %s, Cant. a añadir: %d", req_plato->restaurante, req_plato->idPedido, req_plato->plato, req_plato->cantidadPlato);
}

void log_rta_GuardarPlato(t_result *result) {
	logTResult(result);
}

// OBTENER_RESTAURANTE

void log_ObtenerRestaurante(char *restaurante) {
	printf(TAB"Metadata requested by "BOLDMAGENTA"[%s]"RESET BREAK, restaurante);
	log_info(logger, "Metadata requested by %s", restaurante);
}

void log_rta_ObtenerRestaurante(t_md *md) {
	if (md->cantidadCocineros == ERROR){
		printf(TAB BOLDRED"[ERROR]"RESET RED" %s"RESET BREAK, REST_NO_EXISTE);
		log_error(logger, "%s", REST_NO_EXISTE);
	} else {
		printf(BOLD"[Metadata del restaurante]:"RESET BREAK);
		log_info(logger, "Metadata del restaurante:");
		printf(TAB"Cantidad de cocineros: "BOLD"%d"RESET BREAK, md->cantidadCocineros);
		printf(TAB"Cantidad de hornos: "BOLD"%d"RESET BREAK, md->cantidadHornos);
		printf(TAB"Cantidad de pedidos: "BOLD"%d"RESET BREAK, md->cantidadPedidos);
		printf(TAB"Posición del restaurante: "BOLD"[%d,%d]"RESET BREAK, md->posX, md->posY);
		log_info(logger, TAB"Cant. de cocineros: %d, Cant. de hornos: %d, Cant. de pedidos: %d", md->cantidadCocineros, md->cantidadHornos, md->cantidadPedidos);
		log_info(logger, TAB"Posición del restaurante: [%d,%d]", md->posX, md->posY);
		printf(TAB BOLD"Afinidades:"RESET BREAK);
		log_info(logger, TAB"Afinidades:");
		log_lista_strings(md->afinidades);
		printf(TAB BOLD"Recetas:"RESET BREAK);
		log_list_t_md_receta(md->platos);
	}
}

// CONSULTAR_PLATOS

void log_ConsultarPlatos(char *restaurante) { // Antes logConsultaPlatos
	printf(TAB"Restaurante: "BOLDMAGENTA"%s"RESET BREAK, restaurante);
	log_info(logger, TAB"Restaurante %s", restaurante);
}

void log_rta_ConsultarPlatos(t_list *platos) {
	if (list_is_empty(platos)) {
		printf(BOLD"El restaurante no posee platos"RESET BREAK);
		log_info(logger, "El restaurante no posee platos");
	} else if (list_size(platos) == 1 &&
		string_equals_ignore_case(list_get(platos, 0), REST_NO_EXISTE)) {
		printf(TAB BOLDRED"[ERROR]"RESET RED" %s"RESET BREAK, REST_NO_EXISTE);
		log_error(logger, "%s", REST_NO_EXISTE);
	} else {
		log_lista_strings(platos);
	}
}

// GUARDAR_PEDIDO

void log_rta_GuardarPedido(t_result *result) {
	logTResult(result);
}

// OBTENER_RECETA

void log_ObtenerReceta(char *receta_solicitada) {
	printf(TAB"Receta solicitada: "BOLDMAGENTA"%s"RESET BREAK, receta_solicitada);
	log_info(logger, TAB"Receta solicitada %s", receta_solicitada);
}

void log_receta_e_instrucciones(t_receta *receta) {
	t_list *instrucciones = list_create();
	list_add_all(instrucciones, receta->instrucciones);
	int size = list_size(instrucciones);
	printf("Receta: "BOLDMAGENTA"[%s]"RESET BREAK, receta->plato);
	log_info(logger, "Receta: [%s]", receta->plato);
	for (int i = 0; i < size; i++) {
		t_instrucciones_receta *current_inst = list_get(instrucciones, i);
		printf("[Paso #%d]: "BOLD"%s"RESET", Tiempo: %d" BREAK, i, current_inst->paso, current_inst->qPaso);
		log_info(logger, "[Paso #%d]: %s, Tiempo: %d", i, current_inst->paso, current_inst->qPaso);
	}
}

void log_rta_ObtenerReceta(t_receta *receta) {
	if (string_equals_ignore_case(receta->plato, RECETA_NO_EXISTE)) {
		printf(TAB BOLDRED"[ERROR]"RESET RED" %s"RESET BREAK, RECETA_NO_EXISTE);
		log_error(logger, "%s", RECETA_NO_EXISTE);
	} else {
		log_receta_e_instrucciones(receta);
	}
}

/* Sindicato */

// Atender conexiones

void log_metadata_request(char *nombreRestaurante) { // ahora log_obtener_restaurante
	printf(TAB"Metadata requested by "MAGENTA"%s"RESET BREAK, nombreRestaurante);
	log_info(logger, "Metadada requested by %s", nombreRestaurante);
}

void log_AFIP_file_line(ssize_t line_size, size_t line_buf_size, char *current_line) { // Revisar
	log_debug(logger, "Chars=%03d, Buf_size=%03zu, Contenido: %s", line_size, line_buf_size, current_line);
}

// Bloques e implementación interna

void log_full_FS(int cantReq, int cantDisp) {
	printf(RED"[ERROR] No hay bloques suficientes para realizar la operación. "RESET BOLDRED "[Cant. requerida: %d] - [Cant. disponible: %d]"RESET BREAK, cantReq, cantDisp);
	log_error(logger, "No hay bloques suficientes para realizar la operación. Cant. requerida: %d. Cant. disponible: %d", cantReq, cantDisp);
}

void log_blocks_assignment() {
	printf("Iniciando la reserva de bloques..."BREAK);
	log_debug(logger, "Iniciando la reserva de bloques...");
}

void log_assigned_blocks(int *assigned_blocks, int size) {
	printf("Hay "BOLD"%d"RESET" bloques asignados:"BREAK, size);
	log_debug(logger, "Hay %d bloques asignados:", size);
	for (int i = 0; i < size; i++) {
		printf(TAB"[Bloque #%d] - %d.AFIP"BREAK, i+1, assigned_blocks[i]);
		log_debug(logger, TAB"[Bloque #%d] - %d.AFIP", i+1, assigned_blocks[i]);
	}
}

// File content

void log_full_blocks_content(char *content) {
	log_debug(logger, "El contenido del archivo leído es:");
	//log_debug(logger, "--------------------------------------------------");
	log_debug(logger, BREAK"%s", content);
	//log_debug(logger, "--------------------------------------------------");
}

// .AFIP files

void log_no_AFIP_content() {
	printf(BOLDRED"[ERROR]"RESET RED" El archivo .AFIP no tenía contenido. No es posible recuperar la información de los bloques."RESET BREAK);
	log_error(logger, "El archivo .AFIP no tenía contenido. No es posible recuperar la información de los bloques.");
}

void log_Info_AFIP(char *rest) {
	printf("Creando el archivo "BOLD"Info.AFIP"RESET" para el restaurante "BOLDMAGENTA"[%s]"RESET"..." BREAK, rest);
	log_debug(logger, "Creando el archivo Info.AFIP para el restaurante [%s]...", rest);
}

void log_Pedido_AFIP(int nroPedido) {
	printf("Creando el archivo "BOLD".AFIP"RESET"..." BREAK, nroPedido);
	log_debug(logger, "Creando el archivo Pedido%d.AFIP...", nroPedido);
}

void log_Receta_AFIP(char *receta) {
	printf("Creando el archivo "BOLD"%s.AFIP"RESET"..." BREAK, receta);
	log_debug(logger, "Creando el archivo %s.AFIP...", receta);
}

// Bitmap

void log_bitmap_file_error() {
	printf(BOLDRED"[ERROR]"RESET RED" Error al abrir el archivo Bitmap.bin"RESET BREAK);
	log_error(logger, "Error al abrir el archivo Bitmap.bin");
}

void log_bitmap_error() {
	printf(BOLDRED"[ERROR]"RESET RED" Error al realizar mmap"RESET BREAK);
	log_error(logger, "Error al realizar mmap");
}

void log_bitarray_info(t_bitarray *bitarray, int available_blocks) {
	int lastBit = bitarray_get_max_bit(bitarray);
	printf("→ Tamaño del bitmap: %d"BREAK, lastBit);
	printf("→ Cant. de bloques disponibles: "BOLD"%d"RESET BREAK, available_blocks);
	printf(TAB BOLD"→"RESET" Valor del primer bit: %d"BREAK, bitarray_test_bit(bitarray, 0));
	printf(TAB BOLD"→"RESET" Valor del último bit: %d"BREAK, bitarray_test_bit(bitarray, lastBit));
	log_debug(logger, "→ Tamaño del bitmap: %d", lastBit);
	log_debug(logger, "→ Cant. de bloques disponibles: %d", available_blocks);
	log_debug(logger, TAB"→ Valor del primer bit: %d", bitarray_test_bit(bitarray, 0));
	log_debug(logger, TAB"→ Valor del último bit: %d", bitarray_test_bit(bitarray, lastBit));
}

void log_bitmap_init() {
	printf(BOLD"Creando Bitmap.bin..."RESET BREAK);
	log_debug(logger, "Creando Bitmap.bin...");
}

void log_bitmap_success() {
	printf(BOLD"Archivo Bitmap.bin creado exitosamente"RESET BREAK);
	log_debug(logger, "Archivo Bitmap.bin creado exitosamente");
}

void log_bitmap_reload() {
	printf(BOLD"Cargando el archivo Bitmap.bin ya existente en el FS..."RESET BREAK);
	log_debug(logger, "Cargando el archivo Bitmap.bin ya existente en el FS...");
}

void log_bitmap_reload_success() {
	printf(BOLD"Archivo Bitmap.bin cargado exitosamente"RESET BREAK);
	log_debug(logger, "Archivo Bitmap.bin cargado exitosamente");
}

// Bitmap updates

void log_bit_state(int pos, int bit) {
	//printf(TAB BOLD"→"RESET" El bit [%d] se encuentra en estado %d"BREAK, pos, bit);
	log_debug(logger, "→ El bit [%d] se encuentra en estado %d", pos, bit);
}

void log_bit_update(int pos, t_bitarray *bitarray) {
	int estado = bitarray_test_bit(bitarray, pos);
	char *mensaje = string_new(); string_append_with_format(&mensaje, "%s%d%s", "El bloque ", pos, estado ? " ha sido asignado -" : "ha sido desasignado -");
	printf(TAB BOLD"→"RESET" %s El bit %d ha cambiado al estado %d"BREAK, mensaje, pos, estado);
	log_info(logger, TAB"→ %s El bit %d ha cambiado al estado %d", mensaje, pos, estado);
	free(mensaje);
}

void log_unavailable_bit(int pos) {
	//printf(TAB TAB BOLD"→"RESET" El bit [%d] está ocupado, buscando al siguiente..."BREAK, pos);
	log_debug(logger, TAB TAB"El bit [%d] está ocupado, buscando al siguiente...", pos);
}

void log_bit_already_free(int bit_pos, int estado) {
	printf("El bit %d ya se encontraba en estado %d, no se actualizará"BREAK, bit_pos, estado);
	log_info(logger, "El bit %d ya se encontraba en estado %d, no se actualizará", bit_pos, estado);
}

// CREAR_RESTAURANTE

void log_CrearRestaurante_Data(char **params) {
	printf("Iniciando la creación del archivo "BOLDYELLOW"Info.AFIP"RESET" para "BOLDMAGENTA"[%s]"RESET" con los siguientes datos:"BREAK, params[1]);
	printf(TAB"Cant_cocineros: %s, Posicion: %s, Afinidades: %s"BREAK, params[2], params[3], params[4]);
	printf(TAB"Platos: %s, Precios: %s"BREAK, params[5], params[6]);
	printf(TAB"Cant_hornos: %s, Cant_pedidos: %s"BREAK, params[7], params[8]);
	log_info(logger, "Iniciando la creación del archivo Info.AFIP para [%s] con los siguientes datos:", params[1]);
	log_info(logger,TAB "Cant_cocineros: %s, Posicion: %s, Afinidades: %s", params[2], params[3], params[4]);
	log_info(logger,TAB "Platos: %s, Precios: %s, Cant_hornos: %s, Cant_pedidos: %s", params[5], params[6], params[7], params[8]);
}

// CREAR_RECETA

void log_CrearReceta_Data(char **params) {
	printf("Iniciando la creación del archivo "BOLDYELLOW"%s.AFIP"RESET" con los siguientes datos:"BREAK, params[1]);
	printf(TAB"Pasos: %s, Tiempo_pasos: %s"BREAK, params[2], params[3]);
	log_info(logger, "Iniciando la creación del archivo %s.AFIP con los siguientes datos:", params[1]);
	log_info(logger, TAB"Pasos: %s, Tiempo_pasos: %s", params[2], params[3]);
}

// CREAR_PEDIDO

void log_CrearPedido_Data(t_request *request) {
	printf("Creando el archivo "BOLDYELLOW"Pedido%d.AFIP"RESET" para el restaurante %s"BREAK, request->idPedido, request->nombre);
	log_info(logger, "Creando el archivo Pedido%d.AFIP para el restaurante %s", request->idPedido, request->nombre);
}

// INICIAR PEDIDO

void log_IniciarPedido_Data(t_req_plato *request) {
	printf("Inicializando el archivo "BOLDYELLOW"Pedido%d.AFIP"RESET" para el restaurante %s:"BREAK, request->idPedido, request->restaurante);
	log_info(logger, "Inicializando el archivo Pedido%d.AFIP para el restaurante %s:", request->idPedido, request->restaurante);
}

/***********************MENSAJE CONSOLA GENERAL***************************/
void show_NaN_error_msg() {
	printf("-----------------------[ERROR]------------------------"BREAK);
	printf(BOLDRED"Se ingresó un valor no numérico en algún/algunos parámetro/s"RESET BREAK);
	log_error(logger, "Se ingresó un valor no numérico en algún/algunos parámetro/s");
}