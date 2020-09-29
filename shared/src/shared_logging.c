#include "../include/shared_logging.h"

/* Formatted logs */

void logInitializedProcess() {
	printf("Proceso \033[1;34m%s\033[0m iniciado...\n", getStringKeyValue(process, PROCNKEYS));
	log_info(logger, "Proceso %s iniciado...", getStringKeyValue(process, PROCNKEYS));
}

void logConnectionAttempt(p_code process, char *ip, int puerto) {
	printf("Intentando conectarse a \033[1;34m%s\033[0m en IP: %s, PUERTO: %d...\n", getStringKeyValue(process, PROCNKEYS), ip, puerto);
	log_info(logger, "Intentando conectarse a %s en IP: %s, PUERTO: %d...", getStringKeyValue(process, PROCNKEYS), ip, puerto);
}

void logConnectionSuccess(p_code process, int puerto) {
	printf("\033[1;32mConexión creada con \033[0m\033[1;34m%s\033[0m\033[1;32m en el puerto %d\033[0m\n", getStringKeyValue(process, PROCNKEYS), puerto);
	log_info(logger, "Conexión creada con %s en el puerto %d", getStringKeyValue(process, PROCNKEYS), puerto);
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

void logMetadata(t_posicion *posicion) {
	printf("\tUbicación del restaurante: posX: %d, posY: %d\n", posicion->posX, posicion->posY);
	log_info(logger, "Ubicación del restaurante: posX: %d, posY: %d", posicion->posX, posicion->posY);
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