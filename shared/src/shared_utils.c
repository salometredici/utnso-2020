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

// Métodos para la serialización

// Devuelve el tamaño en bytes de una lista de strings (bytes de cada palabra) más un int por cada uno, que representará a su longitud
// Copiado acá hasta que separemos las utils en varios files
int getBytesAEnviarListaStrings(t_list *listaStrings) {
	int cantidadElementos = list_size(listaStrings);
	int bytesAEnviar = cantidadElementos * sizeof(int);

	for (int i = 0; i < cantidadElementos; i++) {
		char *palabra = list_get(listaStrings, i);
		bytesAEnviar += strlen(palabra) + 1;
	}

	return bytesAEnviar;
}

int getBytesAEnviarString(char *string) {
	return strlen(string) + 1;
}

int getBytesAEnviarEjemplo() { // Ejemplo para una estructura custom que sólo se compone de dos ints (nada variable como un char*)
	return sizeof(t_posicion);
}

int getBytesHeader() {
	return sizeof(int) * 2;
}

void enviarPaquete(int socket, p_code procesoOrigen, m_code codigoOperacion, void *stream) {
	int tamanioTotal = getTamanioTotalPaquete(codigoOperacion, stream);

	void *buffer = malloc(tamanioTotal);

	serializarHeader(buffer, procesoOrigen, codigoOperacion);
	serializarPayload(buffer, codigoOperacion, stream);
	
	enviarPorSocket(socket, buffer, tamanioTotal);

	free(buffer);
}

void serializarHeader(void *buffer, p_code procesoOrigen, m_code codigoOperacion) {
	memcpy(buffer, &procesoOrigen, sizeof(int));
	memcpy(buffer + sizeof(int), &codigoOperacion, sizeof(int));
}

// Obtiene el tamaño total del paquete a enviar, es decir: header (dos ints) + payload (si existe: un int para el size + size del stream)
int getTamanioTotalPaquete(m_code codigoOperacion, void *stream) {
	int tamanioTotal = getBytesHeader();
	int payloadSize = getPayloadSize(codigoOperacion, stream);
	if (payloadSize != 0) {
		tamanioTotal += payloadSize + sizeof(int);
	}
	return tamanioTotal;
}

int getPayloadSize(m_code codigoOperacion, void *stream) {
	int payloadSize = 0;
	switch(codigoOperacion) {
		case GUARDAR_PEDIDO:
		case GUARDAR_PLATO:
		case PLATO_LISTO:
		case OBTENER_PEDIDO:
			payloadSize += getBytesAEnviarListaStrings(stream);
			break;
		
		case CONFIRMAR_PEDIDO:
		case RTA_GUARDAR_PEDIDO:
		case RTA_GUARDAR_PLATO:
		case RTA_CONFIRMAR_PEDIDO:
		case RTA_PLATO_LISTO:
		case RTA_OBTENER_PEDIDO:
			payloadSize += getBytesAEnviarString(stream);
			break;

		case SELECCIONAR_RESTAURANTE:
			//payloadSize+=getBytesAEnviarString(stream); // falta ver si serializa un string u otra cosa
			break;
		// Casos en los que se envíe un sólo string
		case OBTENER_RESTAURANTE:
			payloadSize += getBytesAEnviarString(stream);
			break;
		case CONSULTAR_PLATOS:
			// obtener size payload que depende de lo que sea STREAM
			break;
		case ANIADIR_PLATO:
			// obtener size payload que depende de lo que sea STREAM
			break;
		case CONSULTAR_PEDIDO:
			// obtener size payload que depende de lo que sea STREAM
			break;
		// Caso con estructura t_posicion de ejemplo
		case RTA_OBTENER_RESTAURANTE:
			payloadSize += getBytesAEnviarEjemplo();
			break;
		// Casos en los que se envíe una lista de strings
		case RTA_CONSULTAR_RESTAURANTES:
			payloadSize += getBytesAEnviarListaStrings(stream);
			break;
		// Si no tiene parámetros que serializar, queda en 0
		default:
			break; // Aca entran CONSULTAR_RESTAURANTES, CREAR_PEDIDO
	}
	return payloadSize;
}

/* Serialización */

void serializarPayload(void *buffer, m_code codigoOperacion, void *stream) {
	int desplazamiento = getBytesHeader();
	int payloadSize = getPayloadSize(codigoOperacion, stream);
	if (payloadSize != 0) { // Si tiene payload para serializar
		void *mensajeSerializado = serializar(codigoOperacion, stream);
		memcpy(buffer + desplazamiento, &payloadSize, sizeof(int));
		desplazamiento += sizeof(int);
		memcpy(buffer + desplazamiento, mensajeSerializado, payloadSize);
		free(mensajeSerializado);
	}
}

void *serializar(m_code codigoOperacion, void *stream) {
	void *buffer;
	switch(codigoOperacion) {
		case GUARDAR_PEDIDO:
		case GUARDAR_PLATO:
		case PLATO_LISTO:
		case OBTENER_PEDIDO:
			buffer = srlzListaStrings(stream);
			break;
		
		case CONFIRMAR_PEDIDO:
		case RTA_GUARDAR_PEDIDO:
		case RTA_GUARDAR_PLATO:
		case RTA_CONFIRMAR_PEDIDO:
		case RTA_PLATO_LISTO:
		case RTA_OBTENER_PEDIDO:
			buffer = srlzString(stream);
			break;

		case SELECCIONAR_RESTAURANTE:
			//buffer = srlzString(stream); // hay que ver que serializar si string u otra cosa
			break;
		case OBTENER_RESTAURANTE:
			buffer = srlzString(stream);
			break;
		case CONSULTAR_PLATOS:
			//buffer = srlzString(stream); // hay que ver que serializar si string u otra cosa
			break;
		case ANIADIR_PLATO:
			//buffer = srlzString(stream); // hay que ver que serializar si string u otra cosa
			break;
		case CONSULTAR_PEDIDO:
			//buffer = srlzString(stream); // hay que ver que serializar si string u otra cosa
			break;
		case RTA_OBTENER_RESTAURANTE:
			buffer = srlzRtaObtenerRestaurante(stream);
			break;
		case RTA_CONSULTAR_RESTAURANTES:
			buffer = srlzListaStrings(stream);
			break;
		default:
			buffer = NULL; //TODO: Excepciones
			break;
	}
	return buffer;
}

// Método para serializar un sólo string
void *srlzString(char *mensaje) {
	char *unMensaje = (char*) mensaje;

	int size =  getBytesAEnviarString(mensaje);  // Tamaño de la palabra

	void *magic = malloc(size);
	memcpy(magic, mensaje, size);
	return magic;
}

void *srlzRtaObtenerRestaurante(t_posicion* posicion) { // Es un ejemplo
	t_posicion* unaPosicion = (t_posicion*) posicion;
	int desplazamiento = 0;

	int size = getBytesAEnviarEjemplo(); // Tamaño de la posición de un restaurante

	void *magic = malloc(size);

	memcpy(magic + desplazamiento, &unaPosicion->posX, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &unaPosicion->posY, sizeof(int));

	return magic;
}

// Método para serializar una lista de strings
void *srlzListaStrings(t_list *listaStrings) {
	int desplazamiento = 0;
	t_list *unaLista = (t_list*) listaStrings;

	int longitudLista = list_size(listaStrings);
	int sizeLista = getBytesAEnviarListaStrings(listaStrings);
	void *magic = malloc(sizeLista);

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

/* Métodos de envío y recepción de header/payload */

int enviarPorSocket(int socket, const void *mensaje, int totalAEnviar) {	
	int bytesEnviados;
	int totalEnviado = 0;
	while (totalEnviado < totalAEnviar) {
		bytesEnviados = send(socket, mensaje + totalEnviado, totalAEnviar, 0);
		if (bytesEnviados == ERROR) {
			break;
		}	
		totalEnviado += bytesEnviados;
		totalAEnviar -= bytesEnviados;	
	}
	return bytesEnviados;
}

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

void *recibirBuffer(int *size, int socket)
{
	void *buffer;

	recv(socket, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket, buffer, *size, MSG_WAITALL);

	return buffer;
}

t_buffer *recibirPayloadPaquete(t_header *header, int socket) {
	int size;
	t_buffer *payload = malloc(sizeof(t_buffer));

	void *buffer = recibirBuffer(&size, socket);

	// payload = malloc(sizeof(size));

	switch (header->codigoOperacion) {
		case GUARDAR_PEDIDO:
		case GUARDAR_PLATO:
		case PLATO_LISTO:
		case OBTENER_PEDIDO:
			payload = dsrlzListaStrings(payload, buffer, size);
			break;
		
		case CONFIRMAR_PEDIDO:
		case RTA_GUARDAR_PEDIDO:
		case RTA_GUARDAR_PLATO:
		case RTA_CONFIRMAR_PEDIDO:
		case RTA_PLATO_LISTO:
		case RTA_OBTENER_PEDIDO:
			payload = dsrlzString(payload, buffer, size);
			break;
		
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

/* Deserialización */

t_buffer *dsrlzRtaObtenerRestaurante(t_buffer *payload, void *buffer) { // Por ahora
	t_posicion *posicion = malloc(sizeof(t_posicion));
	int desplazamiento = 0;

	memcpy(&posicion->posX, buffer, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(&posicion->posY, buffer + desplazamiento, sizeof(int));

	payload->stream = posicion;
	return payload;
}

// Método para deserializar un único string
t_buffer *dsrlzString(t_buffer *payload, void *buffer, int sizeString) {
	char *cadena = malloc(sizeString);
	memcpy(cadena, buffer, sizeString);
	payload->stream = cadena;
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