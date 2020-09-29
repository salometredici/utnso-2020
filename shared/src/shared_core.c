#include "../include/shared_core.h"

char *mi_funcion_compartida() {
    return "Hice uso de la shared!";
}

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

/* Inicialización */

void inicializarProceso(p_code proceso) {
	char *log_path;
	char *program;
	char *config_path;
	switch (proceso) {
		case APP:
			log_path = "app.log";
			program = "app";
			config_path = "app.config";
			break;
		case CLIENTE:
			log_path = "cliente.log";
			program = "cliente";
			config_path = "cliente.config";
			break;
		case COMANDA:
			log_path = "comanda.log";
			program = "comanda";
			config_path = "comanda.config";
			break;
		case RESTAURANTE:
			program = "restaurante";
			config_path = "restaurante.config";
			break;
		case SINDICATO:
			log_path = "sindicato.log";
			program = "sindicato";
			config_path = "sindicato.config";
			break;
	}
	process = proceso;
	config = config_create(config_path);
	if (proceso == RESTAURANTE) { log_path = crearLogRestaurante(); }
	crearLoggerProceso(log_path, program);
	logInitializedProcess();
}

char *crearLogRestaurante() {
	char *path = obtenerNombreRestaurante();
	char *logFile = strcat(path, ".log");

	/* Comentado hasta que nos den una manera de usar log_create enviando una ruta completa y no un nombre de file */
	//int fullPathLength = strlen(path) + strlen(LOGS_PATH);
	//char fullPathArray[fullPathLength];
	//strcpy(fullPathArray, LOGS_PATH);
	//char *fullPath = strcat(fullPathArray, path);

	// Creamos el archivo si no existe
	FILE *f = fopen(logFile, "w");
	fclose(f);
	return logFile;
}

void crearLoggerProceso(char *log_path, char *program) {
	bool activeConsole = obtenerActiveConsole();
	int logLevel = obtenerLogLevel();
	logger = log_create(log_path, program, activeConsole, logLevel);
}

void finalizarProceso() {
	config_destroy(config);
	log_destroy(logger);
}

/* Config */

int obtenerPuertoEscucha() {
	return config_get_int_value(config, "PUERTO_ESCUCHA");
}

char* obtenerNombreRestaurante() {
	return config_get_string_value(config, "NOMBRE_RESTAURANTE");
}

bool obtenerActiveConsole() { 
	return config_get_int_value(config, "ACTIVE_CONSOLE");
}

int obtenerLogLevel() {
	return config_get_int_value(config, "LOG_LEVEL");
}

/* Conexiones */

int crearSocket(int puerto) {
	int fd;
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == ERROR) {
		log_error(logger, "Fallo al crear el socket servidor en el puerto %d", puerto);
		exit(EXIT_FAILURE);
	}
	return fd;
}

void liberarConexion(int socket) {
	close(socket);
}

// Cliente

int conectarseA(p_code proceso) {
	char *ip;
	int puerto;
	switch(proceso) {
		case APP:
			ip = config_get_string_value(config, "IP_APP");
			puerto = config_get_int_value(config, "PUERTO_APP");
			break;
		case CLIENTE:
			ip = config_get_string_value(config, "IP_CLIENTE");
			puerto = config_get_int_value(config, "PUERTO_CLIENTE");
			break;
		case COMANDA:
			ip = config_get_string_value(config, "IP_COMANDA");
			puerto = config_get_int_value(config, "PUERTO_COMANDA");
			break;
		case RESTAURANTE:
			ip = config_get_string_value(config, "IP_RESTAURANTE");
			puerto = config_get_int_value(config, "PUERTO_RESTAURANTE");
			break;
		case SINDICATO:
			ip = config_get_string_value(config, "IP_SINDICATO");
			puerto = config_get_int_value(config, "PUERTO_SINDICATO");
			break;
	}
	logConnectionAttempt(proceso, ip, puerto);
	return crearConexion(proceso, ip, puerto);
}

int crearConexion(p_code proceso, char *ip, int puerto) {
	int socketCliente;
	struct sockaddr_in dir;

	socketCliente = crearSocket(puerto);

	dir.sin_family = AF_INET;
	dir.sin_port = htons(puerto);
	dir.sin_addr.s_addr = inet_addr(ip);
	memset(&(dir.sin_zero), '\0', 8);
	
	if (connect(socketCliente, (struct sockaddr*)&dir, sizeof(struct sockaddr)) == ERROR) {
		log_error(logger, "Fallo al realizar la conexión con IP: %s, PUERTO: %d", ip, puerto);
		close(socketCliente);
		exit(EXIT_FAILURE);
	}

	logConnectionSuccess(proceso, puerto);

	return socketCliente;
}

// Servidor

int iniciarServidor() {
	return abrirSocketEscucha(obtenerPuertoEscucha());
}

int abrirSocketEscucha(int puerto) {
	logInitializedProcess(puerto);
	int option = 1;
	int socketEscucha;
	struct sockaddr_in dirServidor;

	socketEscucha = crearSocket(puerto);

	if (setsockopt(socketEscucha, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int)) == ERROR) {
		log_error(logger, "Fallo en setsockopt");
		close(socketEscucha);
		exit(EXIT_FAILURE);
	}

	dirServidor.sin_family = AF_INET;
	dirServidor.sin_port = htons(puerto);
	dirServidor.sin_addr.s_addr = INADDR_ANY;
	memset(&(dirServidor.sin_zero), '\0', 8);

	if (bind(socketEscucha, (struct sockaddr*)&dirServidor, sizeof(struct sockaddr_in)) == ERROR) {
		log_error(logger, "Fallo al asignar el puerto al socket servidor");
		close(socketEscucha);
		exit(EXIT_FAILURE);
	}

	if (listen(socketEscucha, 5) == ERROR) {
		log_error(logger, "Fallo en listen");
		exit(EXIT_FAILURE);
	}
	logAwaitingConnections(puerto);
    return socketEscucha;
}

int aceptarCliente(int socketServidor) {
	int socketConexion;
	struct sockaddr_in dirCliente;
	int dirSize = sizeof(struct sockaddr_in);
	socketConexion = accept(socketServidor, (struct sockaddr*)&dirCliente, &dirSize);
	if (socketConexion == ERROR) {
		log_error(logger, "Fallo en la conexión del socket %d", socketServidor);
		log_error(logger, "Valor de errno: %d", errno);
	}
	return socketConexion;
}