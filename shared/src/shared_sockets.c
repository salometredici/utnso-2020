#include "../include/shared_sockets.h"

/* Inicialización */

char *crearLogRestaurante() {
	char *restaurante = obtenerNombreRestaurante();
	int filenameLength = strlen(restaurante) + 4;
	char fullPathArray[filenameLength];
	strcpy(fullPathArray,restaurante);
	strcat(fullPathArray, ".log");
	char *file = malloc(filenameLength);
	file = fullPathArray;

	/* Comentado hasta que nos den una manera de usar log_create enviando una ruta completa y no un nombre de file */
	//int fullPathLength = strlen(path) + strlen(LOGS_PATH);
	//char fullPathArray[fullPathLength];
	//strcpy(fullPathArray, LOGS_PATH);
	//char *fullPath = strcat(fullPathArray, path);

	return file;
}

char *getLogPath(p_code process) {
	char *fileName = "";
	int fullPathLegth = 0;

	if (process == CLIENTE) {
		fileName = obtenerCliente();
		fullPathLegth = strlen(LOGS_PATH) + strlen(fileName) + strlen(".log") + 1;
	} else {
		fileName = obtenerLogFileName();
		fullPathLegth = strlen(fileName) + 1;
	}

	char fullPath[fullPathLegth];
	if (process == CLIENTE) {
		strcpy(fullPath, LOGS_PATH); strcat(fullPath, fileName); strcat(fullPath, ".log");
	} else {
		strcpy(fullPath, fileName);
	}

	char *logPath = malloc(fullPathLegth);
	for (int i = 0; i < fullPathLegth; i++) {
		logPath[i] = fullPath[i];
	}
	return logPath;
}

void crearLoggerProceso(char *log_path, char *program) {
	bool activeConsole = obtenerActiveConsole();
	int logLevel = obtenerLogLevel();
	logger = log_create(log_path, program, activeConsole, logLevel);
}

void inicializarProceso(p_code proceso) {
	createDirectory(LOGS_PATH);
	char *log_path = "";
	char *program;
	char *config_path;
	switch (proceso) {
		case APP:
			program = "app";
			config_path = "app.config";
			break;
		case CLIENTE:
			program = "cliente";
			config_path = "cliente.config";
			break;
		case COMANDA:
			program = "comanda";
			config_path = "comanda.config";
			break;
		case RESTAURANTE:
			program = "restaurante";
			config_path = "restaurante.config";
			break;
		case SINDICATO:
			log_path = "/home/utnso/logs/sindicato.log";
			program = "sindicato";
			config_path = "sindicato.config";
			break;
	}
	process = proceso;
	config = config_create(config_path);

	if (proceso != SINDICATO) {
		log_path = getLogPath(proceso);
		crearLoggerProceso(log_path, program);
	} else {
		crearLoggerProceso(log_path, program);
	}

	logInitializedProcess();
}

void finalizarProceso() {
	config_destroy(config);
	log_destroy(logger);
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
			ip = config_get_string_value(config, "IP");
			puerto = config_get_int_value(config, "PUERTO");
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