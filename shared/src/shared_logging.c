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
	log_info(logger, "El cliente %d se desconectó. Finalizando el hilo...（・∩・)\n", socketCliente);
}

void logMetadata(t_posicion *posicion) {
	printf("Ubicación del restaurante: posX: %d, posY: %d", posicion->posX, posicion->posY);
	log_info(logger, "Ubicación del restaurante: posX: %d, posY: %d", posicion->posX, posicion->posY);
}