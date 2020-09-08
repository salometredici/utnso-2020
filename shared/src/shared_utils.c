#include "../include/shared_utils.h"

char* mi_funcion_compartida(){
    return "Hice uso de la shared!";
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
}

// Conexiones

int conectarseA(p_code proceso)
{
	char *ip;
	char *puerto;
	switch(proceso) {
		case APP:
			ip = config_get_string_value(config, "IP_APP");
			puerto = config_get_string_value(config, "PUERTO_APP");
		case CLIENTE:
			ip = config_get_string_value(config, "IP_CLIENTE");
			puerto = config_get_string_value(config, "PUERTO_CLIENTE");
		case COMANDA:
			ip = config_get_string_value(config, "IP_COMANDA");
			puerto = config_get_string_value(config, "PUERTO_COMANDA");
		case RESTAURANTE:
			ip = config_get_string_value(config, "IP_RESTAURANTE");
			puerto = config_get_string_value(config, "PUERTO_RESTAURANTE");
		case SINDICATO:
			ip = config_get_string_value(config, "IP_SINDICATO");
			puerto = config_get_string_value(config, "PUERTO_SINDICATO");
	}
	return crearConexion(ip, puerto);
}

int crearConexion(char *ip, char *puerto)
{
	int socketCliente;
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	if ((socketCliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol)) == -1) {
		log_error(logger, "No se pudo crear el socket. IP: %s, PUERTO: %s", ip, puerto);
		exit(EXIT_FAILURE);
	}

	if (connect(socketCliente, server_info->ai_addr, server_info->ai_addrlen) == -1) {
		log_error(logger, "No se pudo realizar la conexión. IP: %s, PUERTO: %s", ip, puerto);
		close(socketCliente);
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(server_info);

	return socketCliente;
}

// Config

char* obtenerPuertoEscucha() {
	return config_get_string_value(config, "PUERTO_ESCUCHA");
}

int obtenerMaxClientes() {
	return config_get_int_value(config, "MAX_CLIENTES");
}

// Servidor

int iniciarServidor() {
	return abrirSocketEscucha(obtenerPuertoEscucha());
}

int abrirSocketEscucha(char *puerto)
{
	log_debug(logger, "Iniciando servidor...");
	int option = 1;
	int socketEscucha;
	struct sockaddr_in dirServidor;

	if ((socketEscucha = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		log_error(logger, "Fallo al crear el socket servidor en el puerto %s", puerto);
		exit(EXIT_FAILURE);
	}

	if (setsockopt(socketEscucha, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int)) == -1)
	{
		log_error(logger, "Fallo en setsockopt");
		close(socketEscucha);
		exit(EXIT_FAILURE);
	}

	memset(&dirServidor, 0, sizeof(struct sockaddr_in));
	dirServidor.sin_family = AF_INET;
	dirServidor.sin_addr.s_addr = INADDR_ANY;
	dirServidor.sin_port = htons(puerto);

	if (bind(socketEscucha, (struct sockaddr*)&dirServidor, sizeof(struct sockaddr_in)) == -1) {
		log_error(logger, "Fallo al asignar el puerto al socket servidor");
		close(socketEscucha);
		exit(EXIT_FAILURE);
	}

	listen(socketEscucha, obtenerMaxClientes());
	log_debug(logger, "Escuchando conexiones en %s ...", puerto);
    return socketEscucha;
}

int aceptarCliente(int socketServidor)
{
	struct sockaddr_in dirCliente;
	return accept(socketServidor, (void*)&dirCliente, sizeof(struct sockaddr_in));
}

// Finalizar proceso

void finalizarProceso(int conexion) {
	liberarConexion(conexion);
	config_destroy(config);
	log_destroy(logger);
}

void liberarConexion(int socket)
{
	close(socket);
}

//

void enviarMensaje(char* mensaje, int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}

void crear_buffer(t_paquete* paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

t_paquete* crear_super_paquete(void)
{
	//me falta un malloc!
	t_paquete* paquete;

	//descomentar despues de arreglar
	//paquete->codigo_operacion = PAQUETE;
	//crear_buffer(paquete);
	return paquete;
}

t_paquete* crear_paquete(void)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = PAQUETE;
	crear_buffer(paquete);
	return paquete;
}

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete(t_paquete* paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) != 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
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