#include "../include/shared_utils.h"

char* mi_funcion_compartida() {
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
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
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
	
	if (connect(socketCliente, (struct sockaddr*)&dir, sizeof(struct sockaddr)) == -1) {
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

	if (setsockopt(socketEscucha, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int)) == -1) {
		log_error(logger, "Fallo en setsockopt");
		close(socketEscucha);
		exit(EXIT_FAILURE);
	}

	dirServidor.sin_family = AF_INET;
	dirServidor.sin_port = htons(puerto);
	dirServidor.sin_addr.s_addr = INADDR_ANY;
	memset(&(dirServidor.sin_zero), '\0', 8);

	if (bind(socketEscucha, (struct sockaddr*)&dirServidor, sizeof(struct sockaddr_in)) == -1) {
		log_error(logger, "Fallo al asignar el puerto al socket servidor");
		close(socketEscucha);
		exit(EXIT_FAILURE);
	}

	if (listen(socketEscucha, 5) == -1) {
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
	if (socketConexion == -1) {
		log_error(logger, "Fallo en la conexión del socket %d", socketServidor);
		log_error(logger, "Valor de errno: %d", errno);
	}
	return socketConexion;
}

// Finalizar proceso

void finalizarProceso() {
	config_destroy(config);
	log_destroy(logger);
}

// Método OK

void *recibirBuffer(int *size, int socket)
{
	void *buffer;

	recv(socket, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket, buffer, *size, MSG_WAITALL);

	return buffer;
}

// Serialización

void enviarPaquete(int socket, p_code procesoOrigen, m_code codigoOperacion, int size, void *stream){

	int desplazamiento = 0;

	void *mensajeSerializado = serializar(codigoOperacion, stream, &size);
	int tamanioTotal = sizeof(int) * 3 + size;

	void* buffer = malloc(tamanioTotal);
	memcpy(buffer+ desplazamiento, &procesoOrigen, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(buffer + desplazamiento, &codigoOperacion, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(buffer + desplazamiento, &size, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(buffer + desplazamiento, mensajeSerializado, size);

	enviarPorSocket(socket, buffer, tamanioTotal);

	free(buffer);
	free(mensajeSerializado);

}

void *serializar(m_code codigoOperacion, void *stream, int *sizeStream) {
	void *buffer;
	switch(codigoOperacion) {
		case OBTENER_RESTAURANTE:
			buffer = srlzString(stream, sizeStream);
			break;
		case RTA_OBTENER_RESTAURANTE:
			buffer = srlzRtaObtenerRestaurante(stream, sizeStream);
			break;
		case RTA_CONSULTAR_RESTAURANTES:
			buffer = srlzListaStrings(stream, sizeStream);
			break;
		default:
			buffer = NULL; //TODO: Excepciones
			break;
	}
	return buffer;
}

// Método para serializar un sólo string
void *srlzString(char *mensaje, int *size) { // Sacar int ?
	char *unMensaje = (char*) mensaje;

	*size =  strlen(mensaje) + 1;  // Tamaño del char

	void *magic = malloc(*size);
	memcpy(magic, mensaje, *size);
	return magic;
}

void *srlzRtaObtenerRestaurante(t_posicion* posicion, int* size) { // Es un ejemplo
	t_posicion* unaPosicion = (t_posicion*) posicion;
	int desplazamiento = 0;

	*size = sizeof(t_posicion); // Tamaño de la posición de un restaurante

	void *magic = malloc(*size);

	memcpy(magic + desplazamiento, &unaPosicion->posX, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &unaPosicion->posY, sizeof(int));

	return magic;
}

// Método para serializar una lista de strings
void *srlzListaStrings(t_list *listaStrings, int *sizeLista) {
	t_list *unaLista = (t_list*) listaStrings;
	int desplazamiento = 0;

	int longitudLista = list_size(listaStrings);
	void *magic = malloc(*sizeLista);

	for (int i = 0; i < longitudLista; i++) {
		char *palabra = list_get(listaStrings, i);
		int longitudPalabra = strlen(palabra) + 1;
		// Vamos a copiar en el stream el tamaño de la palabra y la palabra, para saber cada uno al deserializar
		memcpy(magic + desplazamiento, &longitudPalabra, sizeof(int));
		desplazamiento += sizeof(int);
		memcpy(magic + desplazamiento, palabra, longitudPalabra);
		desplazamiento += longitudPalabra;
	}

	return magic;
}

// Enviar paquete

int enviarPorSocket(int socket, const void *mensaje, int totalAEnviar) {	
	int bytesEnviados;
	int totalEnviado = 0;
	while (totalEnviado < totalAEnviar) {
		bytesEnviados = send(socket, mensaje + totalEnviado, totalAEnviar, 0);
		if (bytesEnviados == -1) {
			break;
		}	
		totalEnviado += bytesEnviados;
		totalAEnviar -= bytesEnviados;	
	}
	return bytesEnviados;
}

// Métodos de deserialización y recepción de paquetes

t_header *recibirHeaderPaquete(int socket) {
	int proceso, mensaje;
	t_header *header = malloc(sizeof(t_header));

	void *buffer = malloc(sizeof(int)*2);

	if (recv(socket, buffer, sizeof(int)*2, MSG_WAITALL) != 0) {
		memcpy(&proceso, buffer, sizeof(int));
		memcpy(&mensaje, buffer + sizeof(int), sizeof(int));
		header->procesoOrigen = proceso;
		header->codigoOperacion = mensaje;
	} else {
		close(socket);
		header->procesoOrigen = ERROR;
		header->codigoOperacion = ERROR;
	}

	free(buffer);
	return header;
}

t_buffer *recibirPayloadPaquete(t_header *header, int socket) {
	int size;
	t_buffer *payload;

	void *buffer = recibirBuffer(&size, socket);
	payload = malloc(sizeof(size));

	switch (header->codigoOperacion) {
		case OBTENER_RESTAURANTE:
			payload = dsrlzString(payload, buffer, size);
			break;
		case RTA_OBTENER_RESTAURANTE:
			payload = dsrlzRtaObtenerRestaurante(payload, buffer);			
			break;
		case RTA_CONSULTAR_RESTAURANTES:
			payload = dsrlzListaStrings(payload, buffer, size);
			break;
		default:
			printf("Qué ha pasao'?! џ(ºДºџ)\n");
			break;
	}

	payload->size = size;

	free(buffer);
	return payload;
}

t_buffer *dsrlzRtaObtenerRestaurante(t_buffer *payload, void *buffer) { // Por ahora
	t_posicion *posicion = malloc(sizeof(t_posicion));
	int desplazamiento = 0;

	memcpy(&posicion->posX, buffer, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(&posicion->posY, buffer + desplazamiento, sizeof(int));

	payload->stream = posicion;
	return payload;
}

t_buffer *dsrlzString(t_buffer *payload, void *buffer, int sizeString) {
	char *restaurante = malloc(sizeString);
	memcpy(restaurante, buffer, sizeString);
	payload->stream = restaurante;
	return payload;
}

// Método para deserializar una lista de strings, habiendo recibido en el buffer el tamaño de cada palabra inclusive
t_buffer *dsrlzListaStrings(t_buffer *payload, void *buffer, int sizeLista) {
	int longitudPalabra;
	int desplazamiento = 0;
	t_list *valores = list_create();

	while (desplazamiento < sizeLista) {
		memcpy(&longitudPalabra, buffer + desplazamiento, sizeof(int));
		desplazamiento += sizeof(int);
		char *palabra = malloc(longitudPalabra);
		memcpy(palabra, buffer + desplazamiento, longitudPalabra);
		desplazamiento += longitudPalabra;
		list_add(valores, palabra);
	}

	payload->stream = valores;
	return payload;
}