#include "../include/shared_serialization.h"

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
		case ANIADIR_PLATO:
		case GUARDAR_PLATO:
		case PLATO_LISTO:
		case OBTENER_PEDIDO:
			payloadSize += getBytesAEnviarListaStrings(stream);
			break;
		
		case CONSULTAR_PLATOS:
		case CONFIRMAR_PEDIDO:
		case CONSULTAR_PEDIDO:
		case RTA_CONSULTAR_PLATOS:
		case RTA_CREAR_PEDIDO:
		case RTA_GUARDAR_PEDIDO:
		case RTA_ANIADIR_PLATO:
		case RTA_GUARDAR_PLATO:
		case RTA_CONFIRMAR_PEDIDO:
		case RTA_PLATO_LISTO:
		case RTA_CONSULTAR_PEDIDO:
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
		case ANIADIR_PLATO:
		case GUARDAR_PLATO:
		case PLATO_LISTO:
		case OBTENER_PEDIDO:
			buffer = srlzListaStrings(stream);
			break;
		
		case CONSULTAR_PLATOS:
		case CONFIRMAR_PEDIDO:
		case CONSULTAR_PEDIDO:
		case RTA_CONSULTAR_PLATOS:
		case RTA_CREAR_PEDIDO:
		case RTA_GUARDAR_PEDIDO:
		case RTA_ANIADIR_PLATO:
		case RTA_GUARDAR_PLATO:
		case RTA_CONFIRMAR_PEDIDO:
		case RTA_PLATO_LISTO:
		case RTA_CONSULTAR_PEDIDO:
		case RTA_OBTENER_PEDIDO:
			buffer = srlzString(stream);
			break;

		case SELECCIONAR_RESTAURANTE:
			//buffer = srlzString(stream); // hay que ver que serializar si string u otra cosa
			break;
		case OBTENER_RESTAURANTE:
			buffer = srlzString(stream);
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
		case ANIADIR_PLATO:
		case GUARDAR_PLATO:
		case PLATO_LISTO:
		case OBTENER_PEDIDO:
			payload = dsrlzListaStrings(payload, buffer, size);
			break;
		
		case CONSULTAR_PLATOS:
		case CONFIRMAR_PEDIDO:
		case CONSULTAR_PEDIDO:
		case RTA_CONSULTAR_PLATOS:
		case RTA_CREAR_PEDIDO:
		case RTA_GUARDAR_PEDIDO:
		case RTA_ANIADIR_PLATO:
		case RTA_GUARDAR_PLATO:
		case RTA_CONFIRMAR_PEDIDO:
		case RTA_PLATO_LISTO:
		case RTA_CONSULTAR_PEDIDO:
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