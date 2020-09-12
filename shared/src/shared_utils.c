#include "../include/shared_utils.h"

char* mi_funcion_compartida() {
    return "Hice uso de la shared!";
}

// Utils

void limpiarPantalla() {
	system("clear");
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
	log_info(logger, "Proceso iniciado ..."); // Info
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

// Para revisar

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) != 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return ERROR;
	}
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}

//podemos usar la lista de valores para poder hablar del for y de como recorrer la lista
t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
	return NULL;
}

// Serialización

t_paquete* crearPaquete(p_code procesoOrigen, m_code codigoOperacion, int size, void* stream)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));

	paquete->buffer->size = size;
	paquete->buffer->stream = stream;
	paquete->procesoOrigen = procesoOrigen;
	paquete->codigoOperacion = codigoOperacion;

	return paquete;
}

void* serializarPaquete(t_paquete* paquete, int bytes)
{
	void *magic = malloc(bytes);
	int desplazamiento = 0;

	// Memcpy de atributos propios de t_paquete
	memcpy(magic + desplazamiento, &(paquete->procesoOrigen), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->codigoOperacion), sizeof(int));
	desplazamiento+= sizeof(int);
	// Memcpy de atributos de t_buffer
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}

void enviarPaquete(t_paquete* paquete, int socket_cliente)
{
	// Vamos a enviar algo con una cantidad de bytes igual al tamaño del buffer más 3 ints (procesoOrigen, codigoOperacion, size)
	int bytes = paquete->buffer->size + 3*sizeof(int);
	void *a_enviar = serializarPaquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

void eliminarPaquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

t_paquete *recibirHeaderPaquete(int socket){
	int proceso, mensaje;
	t_paquete* header = malloc(sizeof(t_paquete));
	header->buffer = malloc(sizeof(t_buffer));
	header->buffer->size = 0;
	header->buffer->stream = NULL;

	void* buffer = malloc(sizeof(int)*2);

	if (recv(socket, buffer, sizeof(int)*2, MSG_WAITALL) != 0) {
		memcpy(&proceso,buffer,sizeof(int));
		memcpy(&mensaje,buffer+sizeof(int),sizeof(int));
		header->procesoOrigen = proceso;
		header->codigoOperacion = mensaje;
	} else {
		close(socket);
		header->procesoOrigen = ERROR;
	}

	free(buffer);
	return header;
}

t_paquete* recibirPayloadPaquete(t_paquete* header, int socket){
	int size;
	void* buffer = recibir_buffer(&size, socket);
	char* valor = malloc(size);

	memcpy(valor, buffer, size);
	header->buffer->size = size;
	header->buffer->stream = valor;

	free(buffer);
	return header;
}