#include "../include/shared_logging.h"

/* Utils */

void log_lista_strings(t_list *lista_strings) {
	int cant_elementos = list_size(lista_strings);
	if (list_is_empty(lista_strings)) {
		printf(TAB"[]"BREAK);
		log_info(logger, TAB"[]");
	} else {
		for (int i = 0; i < cant_elementos; i++) {
			char *palabra = list_get(lista_strings, i);
			printf(TAB"[%d] - %s"BREAK, i, palabra);
			log_info(logger, TAB"[%d] - %s", i, palabra);
		}
	}
}

/* Commons */

void log_initialized_process() {
	printf(BOLD"Proceso "RESET BOLDBLUE"%s"RESET BOLD" iniciado..."RESET BREAK, getStringKeyValue(process, PROCNKEYS));
	log_info(logger, "Proceso %s iniciado...", getStringKeyValue(process, PROCNKEYS));
}

void log_connection_attempt(p_code process, char *ip, int puerto) {
	if (process != CLIENTE) {
		printf("--------------------------------------------------"BREAK);
		printf("Intentando conectarse a "BOLDBLUE"%s"RESET" - IP: %s, PUERTO: %d..."BREAK, getStringKeyValue(process, PROCNKEYS), ip, puerto);
		log_info(logger, "--------------------------------------------------");
		log_info(logger, "Intentando conectarse a %s - IP: %s, PUERTO: %d...", getStringKeyValue(process, PROCNKEYS), ip, puerto);
	} else {
		printf("--------------------------------------------------"BREAK);
		printf("Intentando conectarse a IP: %s, PUERTO: %d..."BREAK, ip, puerto);
		log_info(logger, "--------------------------------------------------");	
		log_info(logger, "Intentando conectarse a IP: %s, PUERTO: %d...", ip, puerto);
	}
}

void log_connection_success(p_code process, int puerto) {
	if (process != CLIENTE) {
		printf(BOLDGREEN"Conexión creada con "RESET BOLDBLUE"%s"RESET BOLDGREEN" en el puerto %d"RESET BREAK, getStringKeyValue(process, PROCNKEYS), puerto);
		printf("--------------------------------------------------"BREAK);
		log_info(logger, "Conexión creada con %s en el puerto %d", getStringKeyValue(process, PROCNKEYS), puerto);
		log_info(logger, "--------------------------------------------------");			
	} else {
		printf(BOLDGREEN"Conexión creada con el puerto %d"RESET BREAK, puerto);
		printf("--------------------------------------------------"BREAK);
		log_info(logger, "Conexión creada con el puerto %d", puerto);
		log_info(logger, "--------------------------------------------------");			
	}
}

void log_connection_failure(char *ip, int puerto) {
	printf(RED"[ERROR]"RESET BOLDRED" Fallo al realizar la conexión con IP: %s, PUERTO: %d"RESET BREAK, ip, puerto);
	printf("--------------------------------------------------"BREAK);
	log_error(logger, "Fallo al realizar la conexión con IP: %s, PUERTO: %d", ip, puerto);
	log_info(logger, "--------------------------------------------------");
}

void log_connection_as_cliente(p_code process) {
	printf(TAB BOLD"→"RESET" Conectado al proceso "BOLDBLUE"%s"RESET BREAK, getStringKeyValue(process, PROCNKEYS));
	log_info(TAB"→ Conectado al proceso %s", getStringKeyValue(process, PROCNKEYS));
}

void log_awaiting_connections(int puerto) {
	printf(BOLDBLUE"%s"RESET BOLDGREEN" escuchando conexiones en el puerto %d"RESET BREAK, getStringKeyValue(process, PROCNKEYS), puerto);
	log_info(logger, "%s escuchando conexiones en el puerto %d", getStringKeyValue(process, PROCNKEYS), puerto);
}

void log_console_input(char *input) {
	log_debug(logger, "Comando ingresado: %s", input);
}

void log_common_client_disconnection(int socketCliente) { // GENERAL
	printf(RED"El cliente "RESET BOLDRED"[Socket #%d]"RESET RED" se desconectó. Finalizando el hilo...（・∩・)"RESET BREAK, socketCliente);
	log_info(logger, "El cliente [Socket #%d] se desconectó. Finalizando el hilo...（・∩・)", socketCliente);
}

void log_TCliente_disconnection(char *idCliente, int socketCliente) { // Para APP y RESTAURANTE
	printf(RED"El cliente "RESET BOLDRED"[%s - Socket #%d]"RESET RED" se desconectó. Finalizando su hilo...（・∩・)"RESET BREAK, idCliente, socketCliente);
	log_info(logger, "El cliente [%s - Socket #%d] se desconectó. Finalizando su hilo...（・∩・)", idCliente, socketCliente);
}

// Para cuando revisa la lista de conectados y se encuentra alguno rip
void log_rmv_discconected_client(t_cliente *cliente) {
	printf(RED"El cliente "RESET BOLDRED"[Socket #%d - %s]"RESET RED" se desconectó"RESET BREAK, cliente->socketCliente, cliente->idCliente);
	log_debug(logger, "El cliente [Socket #%d - %s] se desconectó", cliente->socketCliente, cliente->idCliente);
}

void log_comanda_client_disconnection(p_code process, int socketCliente, char *idCliente) { // Para COMANDA
	if (process != ERROR) {
		if (process == CLIENTE) {
			log_TCliente_disconnection(idCliente, socketCliente);
		} else {
			printf(RED"El "RESET BOLDRED"[Socket #%d]"RESET RED" de "RESET BOLDBLUE"%s"RESET RED" se desconectó. Finalizando su hilo de peticiones...（・∩・)"RESET BREAK, socketCliente, getStringKeyValue(process, PROCNKEYS));
			log_info(logger, "El [Socket #%d] de %s se desconectó. Finalizando su hilo de peticiones...（・∩・)", socketCliente, getStringKeyValue(process, PROCNKEYS));
		}
	} else {
		log_common_client_disconnection(socketCliente);
	}
}

void log_disconnections_cliente(p_code process, int socket) { // Para CLIENTE
	if (process != ERROR) {
		printf(RED"El "RESET BOLDRED"[Socket #%d]"RESET RED" de "RESET BOLDBLUE"%s"RESET RED" se desconectó. Finalizando su hilo de notificaciones...（・∩・)"RESET BREAK, socket, getStringKeyValue(process, PROCNKEYS));
		log_info(logger, "El [Socket #%d] de %s se desconectó. Finalizando su hilo de notificaciones...（・∩・)", socket, getStringKeyValue(process, PROCNKEYS));
	} else {
		log_common_client_disconnection(socket);
	}
}

void log_new_client_connection(int socket) {
	printf(BOLDGREEN"Nuevo hilo para atender al cliente "RESET BOLDBLUE"#%d"RESET BREAK, socket);
	log_info(logger, "Nuevo hilo para atender al cliente #%d", socket);
}

void log_header(m_code codigoOperacion, p_code procesoOrigen, int socket) {
	printf(BOLDYELLOW"[HEADER]"RESET" Received "YELLOW"%s"RESET" from "BOLDBLUE"%s"RESET" - Socket: %d"BREAK,
		getStringKeyValue(codigoOperacion, COMMANDNKEYS),
		getStringKeyValue(procesoOrigen, PROCNKEYS),
		socket);
	// log_info(logger, "[HEADER] Received %s from %s - Socket: %d",
	// 	getStringKeyValue(codigoOperacion, COMMANDNKEYS),
	// 	getStringKeyValue(procesoOrigen, PROCNKEYS),
	// 	socket);
}

void log_message_sent(m_code codigoOperacion, int socket) {
	printf("Message "YELLOW"%s"RESET" sent to [Socket #%d]"BREAK, getStringKeyValue(codigoOperacion, COMMANDNKEYS), socket);
	log_info(logger, "Message %s sent to [Socket #%d]", getStringKeyValue(codigoOperacion, COMMANDNKEYS), socket);
}

/* CLIENTE */

void log_init_data_cliente(t_cliente *cliente) {
	log_info("Cliente %s iniciado, PosX: %d, PosY: %d", cliente->idCliente, cliente->posCliente->posX, cliente->posCliente->posY);
}

/* Serialization logs */

void logListaRecetas(t_list *listaRecetas) {
	int cantidadRecetas = list_size(listaRecetas);
	for (int i = 0; i < cantidadRecetas; i++) {
		t_md_receta *recetaActual = list_get(listaRecetas, i);
		printf(TAB BOLD"[Receta #%d]"RESET BREAK, i);
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
		printf(TAB"Resultado: "BOLDRED"[%s]"RESET" - "RED"%s"RESET BREAK, "FAILED", result->msg);
	} else {
		printf(TAB"Resultado: "BOLDGREEN"[%s]"RESET" - "BOLD"%s"RESET BREAK, "SUCCESS", result->msg);
	}
	log_info(logger, TAB"Resultado: [%s] - %s", result->hasError ? "FAILED" : "SUCCESS", result->msg);
}

/* t_request */

void logReqRestaurante(t_request *reqRestaurante) {
	printf("Datos del request:"BREAK);
	if (!string_is_empty(reqRestaurante->nombre)) {
		printf(TAB"Restaurante: "BOLDMAGENTA"%s"RESET BREAK, reqRestaurante->nombre);
	}
	printf(TAB"Pedido: "BOLD"%d"RESET BREAK, reqRestaurante->idPedido);
	if (!string_is_empty(reqRestaurante->nombre)) {
		log_info(logger, TAB"Datos del request: Restaurante: %s, Pedido: %d", reqRestaurante->nombre, reqRestaurante->idPedido);
	} else {
		log_info(logger, TAB"Datos del request: Pedido: %d", reqRestaurante->idPedido);
	}
}

void logReqPlato(t_request *reqPlato) {
	printf("Datos del request:"BREAK);
	printf(TAB"Plato: "BOLD"[%s]"RESET BREAK, reqPlato->nombre);
	printf(TAB"Pedido: "BOLD"%d"RESET BREAK, reqPlato->idPedido);
	log_info(logger, TAB"Datos del request: Plato: %s, Pedido: %d", reqPlato->nombre, reqPlato->idPedido);
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

/* Planificación APP */

/* APP NEW QUEUE */

void log_app_adding_to_new(int pid, char *rest) {
	printf(TAB CYAN"[PLANIFICATION]"RESET" Agregando el PCB ["BOLDCYAN"#%d"RESET"] de "BOLDMAGENTA"%s"RESET" a "BOLD"NEW"RESET" por haber sido confirmado..."BREAK, pid, rest);
	log_debug(logger, TAB"[PLANIFICATION] Agregando el PCB [#%d] de %s a NEW por haber sido confirmado...", pid, rest);
}

void log_app_added_to_new(int pid, char *rest) {
	printf(TAB CYAN"[PLANIFICATION] PCB ["RESET BOLDCYAN"#%d"RESET CYAN"] added from "RESET BOLDMAGENTA"%s"RESET CYAN" to NEW queue"RESET BREAK, pid, rest);
	log_debug(logger, TAB"[PLANIFICATION] PCB [#%d] added from %s to NEW queue", pid, rest);
}

void log_app_removed_from_new(char *algoritmo, int pid, char *rest) {
	printf(TAB CYAN"[PLANIFICATION - %s] PCB ["RESET BOLDCYAN"#%d"RESET CYAN"] from "RESET BOLDMAGENTA"%s"RESET CYAN" removed from NEW queue"RESET BREAK, algoritmo, pid, rest);
	log_debug(logger, TAB"[PLANIFICATION - %s] PCB [#%d] from %s removed from NEW queue", algoritmo, pid, rest);
}

void log_app_asignando_repartidores(int repartidores_disp, int exec_disp) {
	//printf(TAB CYAN"[PLANIFICATION]"RESET" Asignando repartidores para pasar PCBs a "BOLD"READY"RESET"..."BREAK); // Debug
	//log_debug(logger, TAB"[PLANIFICATION] Asignando repartidores para pasar PCBs a READY..."); // Debug
	printf(TAB CYAN"[PLANIFICATION]"RESET" Repartidores disponibles: %d, Espacio en EXEC: %d"BREAK, repartidores_disp, exec_disp);
	log_debug(logger, TAB"[PLANIFICATION] Repartidores disponibles: %d, Espacio en EXEC: %d", repartidores_disp, exec_disp);
}

void log_app_repartidor_asignado(int idRepartidor, int pid, char *rest) {
	//printf(TAB CYAN"[PLANIFICATION]"RESET" Repartidor #%d assigned to PCB ["BOLDCYAN"#%d"RESET"]"BREAK, idRepartidor, pid); // Debug
	log_debug(logger, TAB"[PLANIFICATION] Repartidor #%d assigned to PCB [#%d] from %s", idRepartidor, pid, rest);
}

/* APP READY QUEUE */

void log_app_added_to_ready(int pid, char *rest) {
	printf(TAB CYAN"[PLANIFICATION] PCB ["RESET BOLDCYAN"#%d"RESET CYAN"] from "RESET BOLDMAGENTA"%s"RESET CYAN" added to READY queue"RESET BREAK, pid, rest);
	log_debug(logger, TAB"[PLANIFICATION] PCB [#%d] from %s added to NEW queue", pid, rest);
}

void log_app_removed_from_ready(char *algoritmo, int pid, char *rest) {
	printf(TAB CYAN"[PLANIFICATION - %s] PCB ["RESET BOLDCYAN"#%d"RESET CYAN"] from "RESET BOLDMAGENTA"%s"RESET CYAN" removed from READY queue"RESET BREAK, algoritmo, pid, rest);
	log_debug(logger, TAB"[PLANIFICATION - %s] PCB [#%d] from %s removed from READY queue", algoritmo, pid, rest);
}

void log_app_ready_to_exec(char *algoritmo, int grado_multiprocesamiento, int size_qE) {
	printf(TAB BOLDCYAN"[PLANIFICATION - %s]"RESET BOLD" Agregando PCBs de READY a EXEC"RESET" - Espacio en EXEC: %d"BREAK, algoritmo,  grado_multiprocesamiento - size_qE);
	log_debug(logger, TAB"[PLANIFICATION - %s] Agregando PCBs de READY a EXEC - Espacio en EXEC: %d", algoritmo, grado_multiprocesamiento - size_qE);
}

/* APP EXEC QUEUE */

void log_app_running_exec_cycle(char *algoritmo) {
	printf(BOLDCYAN"[PLANIFICATION - %s] Running execution..."RESET BREAK, algoritmo);
	log_debug(logger, "[PLANIFICATION - %s] Running execution...", algoritmo);
}

void log_app_added_to_exec(char *algoritmo, int pid, char *rest) {
	printf(TAB CYAN"[PLANIFICATION - %s] PCB ["RESET BOLDCYAN"#%d"RESET CYAN"] from "RESET BOLDMAGENTA"%s"RESET CYAN" added to EXEC queue"RESET BREAK, algoritmo, pid, rest);
	log_debug(logger, TAB"[PLANIFICATION - %s] PCB [#%d] from %s added to EXEC queue", algoritmo, pid, rest);
}

/* APP BLOCKED QUEUE */

void log_app_updating_QB_times() {
	//printf(TAB CYAN"[PLANIFICATION]"RESET" Incrementando tiempos de descanso en "BOLD"BLOCKED"RESET"..."BREAK);
	log_debug(logger, TAB"[PLANIFICATION] Incrementando tiempos de descanso en BLOCKED...");
}

void log_app_QB_times_increased(char *algoritmo) {
	printf(TAB CYAN"[PLANIFICATION]"RESET" Tiempos de descanso incrementados en "BOLD"BLOCKED"RESET BREAK);
	log_debug(logger, TAB"[PLANIFICATION] Tiempos de descanso incrementados en BLOCKED");
}

void log_app_pasar_a_QB(char *algoritmo, int pid, bool llego_al_rest, char *rest) {
	printf(TAB CYAN"[PLANIFICATION - %s]"RESET BOLD" PCB ["RESET BOLDCYAN"#%d"RESET BOLD"] from "BOLDMAGENTA"%s"RESET" added to BLOCKED queue in %s state"RESET BREAK, algoritmo, pid, rest, llego_al_rest ? "ESPERANDO_PLATO" : "REPARTIDOR_DESCANSANDO");
	log_debug(logger, TAB"[PLANIFICATION - %s] PCB [#%d] from %s added to BLOCKED queue in %s state", algoritmo, pid, rest, llego_al_rest ? "ESPERANDO_PLATO" : "REPARTIDOR_DESCANSANDO");
}

void log_app_unblocking_pcb(char *algoritmo, int idPedido, char *rest) {
	printf(TAB CYAN"[PLANIFICATION - %s]"RESET BOLD" Desbloqueando al PCB ["RESET BOLDCYAN"#%d"RESET BOLD"] de "RESET BOLDMAGENTA"%s"RESET BOLD"..."RESET BREAK, algoritmo, idPedido, rest);
	log_debug(logger, TAB"[PLANIFICATION - %s] Desbloqueando al PCB [#%d] de %s...", algoritmo, idPedido, rest);
}

void log_app_blocked_to_ready(int pid, char *rest) {
	printf(TAB CYAN"[PLANIFICATION]"RESET" PCB ["BOLDCYAN"#%d"RESET"] from "BOLDMAGENTA"%s"RESET" has returned from "BOLD"BLOCKED"RESET" state to "BOLD"READY"RESET BREAK, pid, rest);
	log_debug(logger, TAB"[PLANIFICATION] PCB [#%d] from %s has returned from BLOCKED state to READY", pid, rest);
}

void log_app_pcb_rest_end(int pid, int qDescansado, char *rest) {
	printf(TAB CYAN"[PLANIFICATION]"RESET" PCB ["BOLDCYAN"#%d"RESET"] from "BOLDMAGENTA"%s"RESET" finished its timeout. Cycles resting: %d"BREAK, pid, rest, qDescansado);
	log_debug(logger, TAB"[PLANIFICATION] PCB [#%d] from %s finished its timeout. Cycles resting: %d", pid, rest, qDescansado);
}

void log_app_repartidor_cansado(int pid, char *rest) {
	printf(TAB CYAN"[PLANIFICATION]"RESET" El repartidor del PCB ["BOLDCYAN"#%d"RESET"] de "BOLDMAGENTA"%s"RESET" debía descansar, el PCB pasó al estado "BOLD"BLOCKED"RESET BREAK, pid, rest);
	log_debug(logger, TAB"[PLANIFICATION] El repartidor del PCB [#%d] de %s debía descansar, el PCB pasó al estado BLOCKED", pid, rest);
}

void log_app_platos_pendientes(int pid, char *rest) {
	printf(TAB CYAN"[PLANIFICATION]"RESET" El PCB ["BOLDCYAN"#%d"RESET"] de "BOLDMAGENTA"%s"RESET" tiene platos pendientes, pasa a BLOCKED"BREAK, pid, rest);
	log_debug(logger, TAB"[PLANIFICATION] El PCB [#%d] de %s tiene platos pendientes, pasa a BLOCKED", pid, rest);			
}

/* APP FINISHED QUEUE */

void log_app_added_to_finished(int pid, char *rest) {
	printf(TAB CYAN"[PLANIFICATION] PCB ["RESET BOLDCYAN"#%d"RESET CYAN"] from "BOLDMAGENTA"%s"RESET" added to FINISHED queue"RESET BREAK, pid, rest);
	log_debug(logger, TAB"[PLANIFICATION] PCB [#%d] from %s added to FINISHED queue", pid, rest);
}

/* Updates */

void log_app_pcb_llego_al_cliente(int pid, char *idCliente, char *rest) {
	printf(TAB BOLDCYAN"[PLANIFICATION]"RESET BOLDMAGENTA" El PCB ["RESET BOLDCYAN"#%d"RESET BOLDMAGENTA"] de %s llegó al cliente, finalizando pedido..."RESET BREAK, pid, rest);
	log_debug(logger, TAB"[PLANIFICATION] El PCB [#%d] de %s llegó al cliente, finalizando pedido...", pid, rest);			
}

void log_app_pcb_entregado_al_cliente(int pid, char *idCliente, char *rest, int idRepartidor) {
	//printf(TAB CYAN"[PLANIFICATION]"RESET" El PCB ["BOLDCYAN"#%d"RESET"] fue entregado a "BOLD"%s"RESET" por el repartidor "BOLD"#%d"RESET", pasa a "BOLD"FINISHED"RESET BREAK, pid, idCliente, idRepartidor); // Debug
	log_debug(logger, TAB"[PLANIFICATION] El PCB [#%d] de %s fue entregado a %s por el repartidor #%d, pasa a FINISHED", pid, rest, idCliente, idRepartidor);
}

void log_app_continua_hacia_cliente(int pid, char *rest) {
	printf(TAB CYAN"[PLANIFICATION]"RESET" El PCB ["BOLDCYAN"#%d"RESET"] de "BOLDMAGENTA"%s"RESET" continúa su viaje hacia el cliente..."BREAK, pid, rest);
	log_debug(logger, TAB"[PLANIFICATION] El PCB [#%d] de %s continúa su viaje hacia el cliente...", pid, rest);			
}

void log_app_pcb_llego_al_rest(int pid, char *rest) {
	printf(TAB BOLDCYAN"[PLANIFICATION]"RESET BOLDMAGENTA" El PCB ["RESET BOLDCYAN"#%d"RESET BOLDMAGENTA"] de %s llegó al restaurante"RESET BREAK, pid, rest);
	log_debug(logger, TAB"[PLANIFICATION] El PCB [#%d] llegó al restaurante %s", pid, rest);
}

void log_app_repartidor_en_camino(int pid, char *rest, t_posicion *pos_destino, tour_code code) {
	//printf(TAB CYAN"[PLANIFICATION]"RESET" El PCB ["BOLDCYAN"#%d"RESET"] va en camino al %s. Destino: "BOLD"[%d,%d]"RESET BREAK, pid, code == HACIA_CLIENTE ? "cliente" : "restaurante", pos_destino->posX, pos_destino->posY); // Debug
	log_debug(logger, TAB"[PLANIFICATION] El PCB [#%d] de %s va en camino al %s. Destino: [%d,%d]", pid, rest, code == HACIA_CLIENTE ? "cliente" : "restaurante", pos_destino->posX, pos_destino->posY);
}

void log_app_traslado_repartidor(int pid, char *rest, int old_posX, int old_posY, int new_posX, int new_posY) {
	printf(TAB CYAN"[PLANIFICATION]"RESET BOLD" El repartidor del PCB ["RESET BOLDCYAN"#%d"RESET BOLD"] de %s se trasladó de [%d,%d] a [%d,%d]"RESET BREAK, pid, rest, old_posX, old_posY, new_posX, new_posY);
	log_debug(logger, TAB"[PLANIFICATION] El repartidor del PCB [#%d] de %s se trasladó de [%d,%d] a [%d,%d]", pid, rest, old_posX, old_posY, new_posX, new_posY);
}

void log_app_repartidor_libre(int idRepartidor, int cant_disp) {
	//printf(TAB CYAN"[PLANIFICATION]"RESET" Repartidor "BOLD"#%d"RESET" is now available. Total available now: %d"BREAK, idRepartidor, cant_disp); // Debug
	log_debug(logger, TAB"[PLANIFICATION] Repartidor #%d is now available. Total available now: %d", idRepartidor, cant_disp);
}

void log_checking_all_platos_listos(int pid, char *rest) {
	printf(TAB CYAN"[PLANIFICATION]"RESET" Revisando si todos los platos del PCB ["BOLDCYAN"#%d"RESET"] de %s están listos..."BREAK, pid, rest);
	log_debug(logger, TAB"[PLANIFICATION] Revisando si todos los platos del PCB [#%d] de %s están listos...", pid, rest);
}

void log_app_FinalizarPedido(int pid, char *rest) {
	//printf(TAB CYAN"[PLANIFICATION]"RESET" Informando la finalización del PCB ["BOLDCYAN"#%d"RESET"] a "BOLDBLUE"COMANDA"RESET"..."BREAK, pid); // Debug
	log_debug(logger, TAB"[PLANIFICATION] Informando la finalización del PCB [#%d] de %s a COMANDA...", pid, rest);
}

void log_app_entrega_a_cliente(int pid, char *cliente, char *rest) {
	printf(TAB CYAN"[PLANIFICATION]"RESET" Informando la finalización del PCB ["BOLDCYAN"#%d"RESET"] de %s al cliente "BOLD"%s"RESET"..."BREAK, pid, rest, cliente);
	log_debug(logger, TAB"[PLANIFICATION] Informando la finalización del PCB [#%d] de %s al cliente %s...", pid, rest, cliente);
}

/* HRRN */

void log_app_increasing_tiempos_espera() {
	printf(TAB BOLDCYAN"[PLANIFICATION - HRRN]"RESET BOLD" Incrementando tiempos de espera en READY..."RESET BREAK);
	log_debug(logger, TAB"[PLANIFICATION - HRRN] Incrementando tiempos de espera en READY...");
}

void log_app_tiempos_espera_increased() {
	printf(TAB BOLDCYAN"[PLANIFICATION - HRRN]"RESET BOLD" Tiempos de espera incrementados en READY"RESET BREAK);
	log_debug(logger, TAB"[PLANIFICATION - HRRN] Tiempos de espera incrementados en READY");
}

void log_app_next_pcb_HRRN() {
	printf(TAB BOLDCYAN"[PLANIFICATION - HRRN]"RESET BOLD" Obteniendo el próximo PCB a pasar a EXEC..."RESET BREAK);
	log_debug(logger, TAB"[PLANIFICATION - HRRN] Obteniendo el próximo PCB a pasar a EXEC...");
}

/* SJF */

void log_app_next_pcb_SJF() {
	printf(TAB BOLDCYAN"[PLANIFICATION - SJF]"RESET BOLD" Obteniendo el próximo PCB a pasar a EXEC..."RESET BREAK);
	log_debug(logger, TAB"[PLANIFICATION - SJF] Obteniendo el próximo PCB a pasar a EXEC...");
}

/* Mensajes */

// ENVIAR_NOMBRE

void log_EnviarNombre(char *nombre) {
	printf(TAB"Se ha conectado el cliente "BOLDMAGENTA"%s"RESET BREAK, nombre);
	log_info(logger, TAB"Se ha conectado el cliente %s", nombre);
}

// ENVIAR_DATACLIENTE

void log_DataCliente(t_cliente *cliente) {
	printf(TAB"Cliente: "BOLD"[%s]"RESET", Socket: "BOLD"[%d]"RESET", Pos.: [%d,%d], [Puerto_escucha]: "BOLD"[%d]"RESET BREAK,
			cliente->idCliente, cliente->socketCliente,
			cliente->posCliente->posX, cliente->posCliente->posY,
			cliente->puerto_cliente);
	log_info(logger, TAB"Cliente: [%s], Socket: [%d], Pos.: [%d,%d], [Puerto_escucha]: [%d]", cliente->idCliente, cliente->socketCliente,
			cliente->posCliente->posX, cliente->posCliente->posY, cliente->puerto_cliente);
}

void log_rta_EnviarDataCliente(t_cliente *cliente) {
	printf(TAB"Cliente: "BOLD"[%s]"RESET", Socket: "BOLD"[%d]"RESET", ¿Es un restaurante?: %s" BREAK, cliente->idCliente, cliente->socketCliente, cliente->esRestaurante ? "Sí" : "No");
	log_info(logger, TAB"Cliente: %s, Socket: %d, ¿Es un restaurante?: %s", cliente->idCliente, cliente->socketCliente, cliente->esRestaurante ? "Sí" : "No");
	if(cliente->esRestaurante){
		printf(TAB"Posición: [%d,%d], Puerto_escucha: "BOLD"[%d]"RESET BREAK, cliente->posRest->posX, cliente->posRest->posY, cliente->puerto_cliente);
		log_info(logger, TAB"Posición: [%d,%d], Puerto_escucha: [%d]", cliente->posRest->posX, cliente->posRest->posY, cliente->puerto_cliente);
	} else{
		printf(TAB"Posición: [%d,%d], Puerto_escucha: "BOLD"[%d]"RESET BREAK, cliente->posCliente->posX, cliente->posCliente->posY, cliente->puerto_cliente);
		log_info(logger, TAB"Posición: [%d,%d], Puerto_escucha: [%d]", cliente->posCliente->posX, cliente->posCliente->posY, cliente->puerto_cliente);
	}
}

// TERMINAR_PEDIDO

void log_TerminarPedido(t_request *request, m_code codigo_operacion) {
	logRequest(request, codigo_operacion);
}

void log_rta_TerminarPedido(t_result *result) {
	logTResult(result);
}

// FINALIZAR_PEDIDO

void log_FinalizarPedido(t_request *request, m_code codigo_operacion) {
	logRequest(request, codigo_operacion);
}

void log_rta_FinalizarPedido(t_result *result) {
	logTResult(result);
}

// CONFIRMAR_PEDIDO

void log_ConfirmarPedido(t_request *request, m_code codigo_operacion) {
	logRequest(request, codigo_operacion);
}

void log_rta_ConfirmarPedido(t_result *result) {
	logTResult(result);
}

// SELECCIONAR_RESTAURANTE

void log_SeleccionarRestaurante(t_selecc_rest *seleccion) {
	printf(TAB"Se ha asociado al cliente "BOLD"%s"RESET" con el restaurante "BOLDMAGENTA"%s"RESET BREAK, seleccion->idCliente, seleccion->restSelecc);
	log_info(logger, TAB"Se ha asociado al cliente %s con el restaurante %s", seleccion->idCliente, seleccion->restSelecc);
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

void log_obtener_pedido_comanda(t_pedido *pedido, t_request *request){
	switch (pedido->estado) {
		case CONFIRMADO:
		case TERMINADO:
		case FINALIZADO:
			printf("["BOLDCYAN"Pedido #%d"RESET"] - Estado: "BOLD"%s"RESET", Precio Total: "BOLD"$%d"RESET BREAK, request->idPedido, getStringEstadoPedido(pedido->estado), pedido->precioTotal);
			log_info(logger, "[Pedido #%d] - Estado: %s, Precio Total: $%d", request->idPedido, getStringEstadoPedido(pedido->estado), pedido->precioTotal);
			log_t_plato_list(pedido->platos);
		break;
		case PENDIENTE:
			if (list_is_empty(pedido->platos)) {
				printf(TAB YELLOW"[WARNING]"RESET BOLD" PENDIENTE"RESET" - %s - [%s, Pedido #%d]"BREAK, NO_HAY_PLATOS, request->nombre, request->idPedido);
				log_error(logger, "Pedido %d, Restaurante %s - PENDIENTE - %s", request->idPedido, request->nombre, NO_HAY_PLATOS);
			} else {
				printf("["BOLDCYAN"Pedido #%d"RESET"] - Estado: "BOLD"%s"RESET BREAK, request->idPedido, getStringEstadoPedido(pedido->estado));
				log_info(logger, "[Pedido #%d] - Estado: %s, Precio Total: $%d", request->idPedido, getStringEstadoPedido(pedido->estado));
				log_t_plato_list(pedido->platos);
			}
			break;
		default:
			break;
	}
	
}

// PLATO_LISTO

void log_PlatoListo(t_plato_listo *req_plato) {
	printf("Datos del plato finalizado:"BREAK);
	printf(TAB"Restaurante: "BOLDMAGENTA"[%s]"RESET BREAK, req_plato->restaurante);
	printf(TAB"Pedido: "BOLD"[%d]"RESET BREAK, req_plato->idPedido);
	printf(TAB"Plato: "BOLD"[%s]"RESET BREAK, req_plato->plato); 
	log_info(logger, "Restaurante: %s, Pedido: %d, Plato: %s", req_plato->restaurante, req_plato->idPedido, req_plato->plato);
}

void log_rta_PlatoListo(t_result *result) {
	logTResult(result);
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

void log_ConsultarPlatos_default(t_list *platos_rest_default) {
	printf("No hay restaurantes conectados, se enviarán los platos del "BOLDMAGENTA"RESTAURANTEDEFAULT"RESET"..."BREAK);
	log_info(logger, "No hay restaurantes conectados, se enviarán los platos del RESTAURANTEDEFAULT...");
	log_rta_ConsultarPlatos(platos_rest_default);
}

void log_ConsultarPlatos_a_rest(char *rest) {
	printf("Consultando los platos del restaurante "BOLDMAGENTA"%s"RESET"..."BREAK, rest);
	log_info(logger, "Consultando los platos del restaurante %s...", rest);
}

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

void log_bitarray_info(t_bitarray *bitarray, int blocks_quantity, int available_blocks) {
	int lastBit = bitarray_get_max_bit(bitarray);
	printf("→ Cant. total de bloques: %d"BREAK, blocks_quantity);
	printf("→ Cant. de bloques disponibles: "BOLD"%d"RESET BREAK, available_blocks);
	printf(TAB BOLD"→"RESET" Valor del primer bit: %d"BREAK, bitarray_test_bit(bitarray, 0));
	printf(TAB BOLD"→"RESET" Valor del último bit: %d"BREAK, bitarray_test_bit(bitarray, lastBit));
	log_debug(logger, "→ Cant. total de bloques: %d", blocks_quantity);
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

void log_CrearPedido_app(char *idCliente, char *rest) {
	printf("Solicitando un nuevo idPedido para el cliente "BOLD"%s"RESET" al RESTAURANTE "BOLDMAGENTA"%s"RESET"..."BREAK, idCliente, rest);
	log_info(logger, "Solicitando un nuevo idPedido para el cliente %d a RESTAURANTE %s...", idCliente, rest);				
}

void log_CrearPedido_Data(t_request *request) {
	printf("Creando el archivo "BOLDYELLOW"Pedido%d.AFIP"RESET" para el restaurante %s"BREAK, request->idPedido, request->nombre);
	log_info(logger, "Creando el archivo Pedido%d.AFIP para el restaurante %s", request->idPedido, request->nombre);
}

void log_rta_CrearPedido(int new_id_pedido) {
	if (new_id_pedido == ERROR) {
		printf(TAB BOLDRED"[ERROR]"RESET RED" No fue posible obtener un número de pedido nuevo"RESET BREAK);
		log_error(logger, TAB"No fue posible obtener un número de pedido nuevo");
	} else {	
		printf(TAB"Se ha creado el pedido #%d"BREAK, new_id_pedido);
		log_info(logger, TAB"Se ha creado el pedido #%d", new_id_pedido);
	}
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

void log_planif_step(char *step, char *desc){
	printf(TAB TAB TAB TAB TAB MAGENTA"[Planif] - "BOLD"[%s] [%s]"RESET BREAK, step, desc);
    log_info(logger, "                        [PLANIF] - [%s] [%s]", step, desc);
}

void log_ConsultarPedido(int idPedido) {
    printf("Consulta pedido #%d"BREAK, idPedido);
    log_info(logger, "Consulta pedido #%d", idPedido);
}

void log_rta_ConsultarPedido(t_pedido *pedido, int idPedido) {
	switch (pedido->estado) {
        case CONFIRMADO:
        case TERMINADO:
        case FINALIZADO:
            printf("["BOLDCYAN"Pedido #%d"RESET"] - Estado: "BOLD"%s"RESET", Precio Total: "BOLD"$%d"RESET BREAK, idPedido, getStringEstadoPedido(pedido->estado), pedido->precioTotal);
            log_info(logger, "[Pedido #%d] - Estado: %s, Precio Total: $%d", idPedido, getStringEstadoPedido(pedido->estado), pedido->precioTotal);
            log_t_plato_list(pedido->platos);
        break;
		case REST_INEXISTENTE:
            printf(TAB BOLDRED"[ERROR]"RESET RED" %s -  [ Pedido #%d]"RESET BREAK, REST_NO_EXISTE, pedido->restaurante, idPedido);
            log_error(logger, "Pedido %d - %s", idPedido, REST_NO_EXISTE);
            break;
        case PEDIDO_INEXISTENTE:
            printf(TAB BOLDRED"[ERROR]"RESET RED" %s - [Pedido #%d]"RESET BREAK, PEDIDO_NO_EXISTE, idPedido);
            log_error(logger, "Pedido %d - %s", idPedido, PEDIDO_NO_EXISTE);
            break;
		case PENDIENTE:
            if (list_is_empty(pedido->platos)) {
                printf(TAB YELLOW"[WARNING]"RESET BOLD" PENDIENTE"RESET" - %s - [Pedido #%d]"BREAK, BLOQUES_NO_ASIGNADOS,  idPedido);
                log_error(logger, "Pedido %d - PENDIENTE - %s", idPedido, BLOQUES_NO_ASIGNADOS);
            } else {
                printf("["BOLDCYAN"Pedido #%d"RESET"] - Estado: "BOLD"%s"RESET", Precio Total: "BOLD"$%d"RESET BREAK, idPedido, getStringEstadoPedido(pedido->estado), pedido->precioTotal);
                log_info(logger, "[Pedido #%d] - Estado: %s, Precio Total: $%d", idPedido, getStringEstadoPedido(pedido->estado), pedido->precioTotal);
                log_t_plato_list(pedido->platos);
            }
            break;
		case SIN_PLATOS: // Revisar
            printf(TAB YELLOW"[WARNING]"RESET" %s - [Pedido #%d]"BREAK, BLOQUES_NO_ASIGNADOS, idPedido);
            log_error(logger, "Pedido %d - %s", idPedido, BLOQUES_NO_ASIGNADOS);
            break;
        default:
            break;
    }
}

void log_victim_founded(int frame){
	printf(TAB BOLDBLUE"Vissstima encontrada [frame]: %d....\n"RESET, frame);
	log_info(logger, "*****Vissstima encontrada [frame]: %d....*****", frame);
	log_info(logger,"---------------------------------------------------");	
}

void log_is_in_mp(int frame){
	printf(TAB BOLDGREEN"\nEl pedido se encuentra en MP [frame] %d \n" RESET, frame);
	log_info(logger,"El pedido se encuentra en MP frame %d", frame);	
}

void log_is_not_in_mp(){
	printf(TAB BOLDRED"El pedido no se encuentra en MP. Se tiene que buscar en swap...\n"BREAK RESET);
	log_info(logger, "El pedido no se encuentra en MP. Se tiene que buscar en swap...");
}

void log_find_victim(){
	log_info(logger,"---------------------------------------------------");	
	printf(TAB BOLDBLUE"Se busca la vissstima.... \n" BREAK RESET);
	log_info(logger, "Se busca la vissstima....");
}

void log_mp_full(){
	log_info(logger,"---------------------------------------------------");	
	log_info(logger, "No hay espacio en la memoria.... hacer swap");
	printf(TAB BOLDRED "No hay espacio en la memoria.... hacer swap\n" BREAK RESET);	
}

void log_swap_full(){
	log_info(logger,"---------------------------------------------------");	
	log_info(logger, "No hay espacio en swap ...");
	printf(TAB BOLDRED"No hay espacio en swap ..." BREAK RESET);	
}

void log_find_victim_0_0(){
	printf(TAB BOLDBLUE"Se busca la vissstima 0_0.... \n" BREAK RESET);
	log_info(logger, "Se busca la vissstima 0_0 ....");	
}

void log_find_victim_0_1(){
	printf(TAB BOLDBLUE"Se busca la vissstima 0_1 .... \n" BREAK RESET);
	log_info(logger, "Se busca la vissstima 0_1 ....");		
}

void log_victim_0_0_not_founded(){
	printf(TAB BLUE"No se encontro la vistima 0_0....\n" BREAK RESET);	
	log_info(logger, "No se encontro la vistima 0_0....");		
}

void log_victim_0_1_not_founded(){
	printf(TAB BLUE"No se encontro la vistima 0_1....\n" BREAK RESET);	
	log_info(logger, "No se encontro la vistima 0_1....");		
}

