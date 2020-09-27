#include "../include/shared_core.h"

char *mi_funcion_compartida() {
    return "Hice uso de la shared!";
}

// Utils

// Devuelve el valor en string del proceso/comando
char* getStringKeyValue(int key, int option) {
	t_keys *diccionario; int size;
	switch (option) {
		case PROCNKEYS:
			diccionario = diccionarioProcesos; size = PROCNKEYS;
			break;
		case COMMANDNKEYS:
			diccionario = diccionarioComandos; size = COMMANDNKEYS;
			break;
		default:
			return ERROR;
	}
    for (int i = 0; i < size; i++) {
    	t_keys sym = diccionario[i];
    	if (key == sym.valor) {
        	return sym.key;
    	}
	}
    return ERROR;
}

// Devuelve el valor correspondiente al enum del comando para utilizarlo en un switch o similar
int commandToString(char *key) {
    t_keys *diccionario = diccionarioComandos;
    for (int i = 0; i < COMMANDNKEYS; i++) {
        t_keys sym = diccionario[i];
        if (strcmp(sym.key, key) == 0) {
            return sym.valor;
        }
    }
    return ERROR;
}

// Commons

void inicializarProceso(p_code proceso) {
	char *log_path;
	char *program;
	char *config_path;
	switch(proceso) {
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
			log_path = "restaurante.log";
			program = "restaurante";
			config_path = "restaurante.config";
			break;
		case SINDICATO:
			log_path = "sindicato.log";
			program = "sindicato";
			config_path = "sindicato.config";
			break;
	}
	logger = log_create(log_path, program, 1, LOG_LEVEL_INFO);
	config = config_create(config_path);
	log_info(logger, "Proceso iniciado...");
}

void finalizarProceso() {
	config_destroy(config);
	log_destroy(logger);
}

// Config

int obtenerPuertoEscucha() {
	return config_get_int_value(config, "PUERTO_ESCUCHA");
}

char* obtenerNombreRestaurante() {
	return config_get_string_value(config, "NOMBRE_RESTAURANTE");
}

// Conexiones

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
	log_info(logger, "Intentando conectarse a IP: %s, PUERTO: %d...", ip, puerto);
	return crearConexion(ip, puerto);
}

int crearConexion(char *ip, int puerto) {
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

	log_info(logger, "Conexión creada con el puerto %d", puerto);

	return socketCliente;
}

// Servidor

int iniciarServidor() {
	return abrirSocketEscucha(obtenerPuertoEscucha());
}

int abrirSocketEscucha(int puerto) {
	log_info(logger, "Iniciando servidor...");
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
	log_info(logger, "Escuchando conexiones en %d...", puerto);
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