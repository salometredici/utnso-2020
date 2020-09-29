#include "../include/shared_serialization.h"

/* Funciones que devuelven la cantidad de bytes de un payload */

int getBytesHeader() {
	return sizeof(int) * 2;
}

int getBytesString(char *string) {
	return strlen(string) + 1;
}

// Size de una lista de strings (bytes de cada palabra) más un int por cada uno, que representará a su longitud + 1
int getBytesListaStrings(t_list *listaStrings) {
	int cantidadElementos = list_size(listaStrings);
	int bytesAEnviar = cantidadElementos * sizeof(int);

	for (int i = 0; i < cantidadElementos; i++) {
		char *palabra = list_get(listaStrings, i);
		bytesAEnviar += getBytesString(palabra);
	}

	return bytesAEnviar;
}

// Size de dos ints (uno para idPedido y otro para el size de nombreRestaurante) más la longitud de nombreRestaurante en sí
int getBytesReqPedido(t_req_pedido *request) {
    return sizeof(int) * 2 + getBytesString(request->restaurante);
}

// Size de cuatro ints (idPedido, cantPlato, size de nombreRestaurante y size del plato) más la longitud de nombreRestaurante y la longitud del plato
int getBytesReqPlato(t_req_plato *request) {
	return sizeof(int) * 4 + getBytesString(request->restaurante) + getBytesString(request->plato);
}

// Size de 4 ints (precio, cantPedida, cantLista y size del plato) más la longitud del plato en sí
int getBytesPlato(t_plato *plato) {
	return sizeof(int) * 4 + getBytesString(plato->plato);
}

int getBytesListaPlatos(t_list *listaPlatos) {
	int bytesAEnviar = 0;
	int cantidadPlatos = list_size(listaPlatos);

	for (int i = 0; i < cantidadPlatos; i++) {
		int sizePlato = getBytesPlato(list_get(listaPlatos, i));
		bytesAEnviar += sizePlato;
	}

	return bytesAEnviar;
}

// Size de 2 ints (estado y precioTotal) + size de cada t_plato
int getBytesPedido(t_pedido *pedido) {
	return sizeof(int) * 2 + getBytesListaPlatos(pedido->platos);
}

int getBytesEjemplo() { // Ejemplo para una estructura custom que sólo se compone de dos ints (nada variable como un char*)
	return sizeof(t_posicion);
}

/* GetBytes de payload y buffer */

int getPayloadSize(m_code codigoOperacion, void *stream) {
	int payloadSize = 0;
	switch(codigoOperacion) {
        // Envío de t_req_pedido
		case GUARDAR_PEDIDO:
		case OBTENER_PEDIDO:
		case CONFIRMAR_PEDIDO:
            payloadSize += getBytesReqPedido(stream);
            break;
		// Envío de t_req_plato
		case GUARDAR_PLATO:
			payloadSize += getBytesReqPlato(stream);
			break;
		// Envío de t_pedido
		case RTA_OBTENER_PEDIDO:
			payloadSize += getBytesPedido(stream);
			break;
		case SELECCIONAR_RESTAURANTE:
			//payloadSize+=getBytesString(stream); // falta ver si serializa un string u otra cosa
			break;
		// Envío de un sólo string
		case OBTENER_RESTAURANTE:
        case CONSULTAR_PLATOS:
		case CONSULTAR_PEDIDO:
		case RTA_PLATO_LISTO:
		case RTA_CREAR_PEDIDO:
		case RTA_ANIADIR_PLATO:
		case RTA_GUARDAR_PLATO:
		case RTA_GUARDAR_PEDIDO:
		case RTA_CONFIRMAR_PEDIDO:
		case RTA_CONSULTAR_PEDIDO:
			payloadSize += getBytesString(stream);
			break;
		// Envío de t_posicion
		case RTA_OBTENER_RESTAURANTE:
			payloadSize += getBytesEjemplo();
			break;
		// Envío de una lista de strings
        case PLATO_LISTO:
		case ANIADIR_PLATO:
		case RTA_CONSULTAR_PLATOS:
		case RTA_CONSULTAR_RESTAURANTES:
			payloadSize += getBytesListaStrings(stream);
			break;
		// Si no tiene parámetros que serializar, queda en 0
		default:
			break; // Aca entran CONSULTAR_RESTAURANTES, CREAR_PEDIDO
	}
	return payloadSize;
}

// Obtiene el size total del paquete a enviar, es decir: header (dos ints) + payload (si existe: un int para el size + size del stream)
int getTamanioTotalPaquete(m_code codigoOperacion, void *stream) {
	int tamanioTotal = getBytesHeader();
	int payloadSize = getPayloadSize(codigoOperacion, stream);
	if (payloadSize != 0) {
		tamanioTotal += payloadSize + sizeof(int);
	}
	return tamanioTotal;
}

/* Serialización */

void serializarHeader(void *buffer, p_code procesoOrigen, m_code codigoOperacion) {
	memcpy(buffer, &procesoOrigen, sizeof(int));
	memcpy(buffer + sizeof(int), &codigoOperacion, sizeof(int));
}

void *serializar(m_code codigoOperacion, void *stream) {
	void *buffer;
	switch(codigoOperacion) {
        case GUARDAR_PEDIDO:
		case OBTENER_PEDIDO:
		case CONFIRMAR_PEDIDO:
            buffer = srlzReqPedido(stream);
            break;
		case GUARDAR_PLATO:
			buffer = srlzReqPlato(stream);
			break;
		case SELECCIONAR_RESTAURANTE:
			//buffer = srlzString(stream); // hay que ver que serializar si string u otra cosa
			break;
		case RTA_PLATO_LISTO:
        case CONSULTAR_PLATOS:
		case CONSULTAR_PEDIDO:
		case RTA_CREAR_PEDIDO:
		case RTA_ANIADIR_PLATO:
		case RTA_GUARDAR_PLATO:
		case RTA_GUARDAR_PEDIDO:
		case RTA_CONFIRMAR_PEDIDO:
		case RTA_CONSULTAR_PEDIDO:
		case OBTENER_RESTAURANTE:
			buffer = srlzString(stream);
			break;
		case RTA_OBTENER_RESTAURANTE:
			buffer = srlzRtaObtenerRestaurante(stream);
			break;
		case RTA_OBTENER_PEDIDO:
			buffer = srlzPedido(stream);
			break;
		case PLATO_LISTO:
		case ANIADIR_PLATO:
		case RTA_CONSULTAR_PLATOS:
		case RTA_CONSULTAR_RESTAURANTES:
			buffer = srlzListaStrings(stream);
			break;
		default:
			buffer = NULL; //TODO: Excepciones
			break;
	}
	return buffer;
}

// Serializar un sólo string
void *srlzString(char *mensaje) {
	int size =  getBytesString(mensaje);  // Tamaño de la palabra
	void *magic = malloc(size);
	memcpy(magic, mensaje, size);
	return magic;
}

// Método para serializar una lista de strings
void *srlzListaStrings(t_list *listaStrings) {
	int desplazamiento = 0;
	int lengthLista = list_size(listaStrings);
	void *magic = malloc(getBytesListaStrings(listaStrings));

	for (int i = 0; i < lengthLista; i++) {
		char *palabra = list_get(listaStrings, i);
		int longitudPalabra = getBytesString(palabra);
		// Vamos a copiar en el stream el tamaño de la palabra y la palabra, para saber cada uno al deserializar
		memcpy(magic + desplazamiento, &longitudPalabra, sizeof(int));
		desplazamiento += sizeof(int);
		memcpy(magic + desplazamiento, palabra, longitudPalabra);
		desplazamiento += longitudPalabra;
	}

	return magic;
}

// Método para serializar un t_req_pedido
void *srlzReqPedido(t_req_pedido *request) {
    int desplazamiento = 0;
    int size = getBytesReqPedido(request);
    int longitudPalabra = getBytesString(request->restaurante);
	char *palabra = request->restaurante;

    void *magic = malloc(size);
    memcpy(magic, &longitudPalabra, sizeof(int));
    desplazamiento += sizeof(int);
    memcpy(magic + desplazamiento, palabra, longitudPalabra);
    desplazamiento += longitudPalabra;
    memcpy(magic + desplazamiento, &request->idPedido, sizeof(int));

    return magic;
}

// Método para serializar un t_req_plato
void *srlzReqPlato(t_req_plato *request) {
	int desplazamiento = 0;
	int size = getBytesReqPlato(request);
	char *plato = request->plato;
	char *restaurante = request->restaurante;
	int longPlato = getBytesString(plato);
	int longRestaurante = getBytesString(restaurante);

	void *magic = malloc(size);
	memcpy(magic, &longPlato, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, plato, longPlato);
	desplazamiento += longPlato;
	memcpy(magic + desplazamiento, &longRestaurante, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, restaurante, longRestaurante);
	desplazamiento += longRestaurante;
	memcpy(magic + desplazamiento, &request->idPedido, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &request->cantidadPlato, sizeof(int));

	return magic;
}

void *srlzPedido(t_pedido *pedido) {
	int desplazamiento = 0;
	t_list *listaPlatos = (t_list*) pedido->platos;

	int longListaPlatos = list_size(listaPlatos);

	int size = getBytesPedido(pedido);

	void *magic = malloc(size);
	memcpy(magic, &pedido->estado, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &pedido->precioTotal, sizeof(int));
	desplazamiento += sizeof(int);
	for (int i = 0; i < longListaPlatos; i++) {
		t_plato *plato = list_get(listaPlatos, i);
		int longNombrePlato = getBytesString(plato->plato);
		// Vamos a copiar en el stream el tamaño del nombre del plato y el plato, más todos sus otros campos
		memcpy(magic + desplazamiento, &longNombrePlato, sizeof(int));
		desplazamiento += sizeof(int);
		memcpy(magic + desplazamiento, plato->plato, longNombrePlato);
		desplazamiento += longNombrePlato;
		memcpy(magic + desplazamiento, &plato->cantidadPedida, sizeof(int));
		desplazamiento += sizeof(int);
		memcpy(magic + desplazamiento, &plato->cantidadLista, sizeof(int));
		desplazamiento += sizeof(int);
		memcpy(magic + desplazamiento, &plato->precio, sizeof(int));
		desplazamiento += sizeof(int);
	}

	return magic;
}

void *srlzRtaObtenerRestaurante(t_posicion* posicion) { // Es un ejemplo
	t_posicion* unaPosicion = (t_posicion*) posicion;
	int desplazamiento = 0;

	int size = getBytesEjemplo(); // Tamaño de la posición de un restaurante

	void *magic = malloc(size);

	memcpy(magic + desplazamiento, &unaPosicion->posX, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &unaPosicion->posY, sizeof(int));

	return magic;
}

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

/* Métodos de envío de header/payload */

void enviarPaquete(int socket, p_code procesoOrigen, m_code codigoOperacion, void *stream) {
	int tamanioTotal = getTamanioTotalPaquete(codigoOperacion, stream);

	void *buffer = malloc(tamanioTotal);

	serializarHeader(buffer, procesoOrigen, codigoOperacion);
	serializarPayload(buffer, codigoOperacion, stream);
	
	enviarPorSocket(socket, buffer, tamanioTotal);
	log_info(logger, "Message %s sent", getStringKeyValue(codigoOperacion, COMMANDNKEYS));

	free(buffer);
}

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

/* Deserialización */

// Método para deserializar un único string
char *dsrlzString(void *buffer, int sizeString) {
	char *cadena = malloc(sizeString);
	memcpy(cadena, buffer, sizeString);
	return cadena;
}

// Método para deserializar una lista de strings, habiendo recibido en el buffer el tamaño de cada palabra inclusive
t_list *dsrlzListaStrings(void *buffer, int sizeLista) {
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

	return valores;
}

t_req_pedido *dsrlzReqPedido(void *buffer) {
	int longitudPalabra;
	int desplazamiento = 0;
	
	memcpy(&longitudPalabra, buffer, sizeof(int));
	desplazamiento += sizeof(int);

	char *restaurante = malloc(longitudPalabra);
	t_req_pedido *request = malloc(sizeof(t_req_pedido));

	memcpy(restaurante, buffer + desplazamiento, longitudPalabra);
	desplazamiento += longitudPalabra;
	memcpy(&request->idPedido, buffer + desplazamiento, sizeof(int));

	request->restaurante = restaurante;
	return request;
}

t_req_plato *dsrlzReqPlato(void *buffer) {
	int longPlato;
	int longRestaurante;
	int desplazamiento = 0;
	t_req_plato *request = malloc(sizeof(t_req_plato));

	memcpy(&longPlato, buffer, sizeof(int));
	desplazamiento += sizeof(int);

	char *plato = malloc(longPlato);
	memcpy(plato, buffer + desplazamiento, longPlato);
	desplazamiento += longPlato;
	memcpy(&longRestaurante, buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	char *restaurante = malloc(longRestaurante);
	memcpy(restaurante, buffer + desplazamiento, longRestaurante);
	desplazamiento += longRestaurante;
	memcpy(&request->idPedido, buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(&request->cantidadPlato, buffer + desplazamiento, sizeof(int));

	request->plato = plato;
	request->restaurante = restaurante;
	return request;
}

t_pedido *dsrlzPedido(void *buffer, int size) {
	int desplazamiento = 0;
	t_pedido *pedido = malloc(sizeof(t_pedido));
	t_list *platos = list_create();
	int sizeListaPlatos = size - sizeof(int) * 2;

	memcpy(&pedido->estado, buffer, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(&pedido->precioTotal, buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	while (desplazamiento < sizeListaPlatos) {
		int longPlatoActual;
		t_plato *platoActual = malloc(sizeof(t_plato));
		// Copiamos el tamaño del nombre del plato en la variable longPlatoActual
		memcpy(&longPlatoActual, buffer + desplazamiento, sizeof(int));
		desplazamiento += sizeof(int);
		// Guardamos el nombre del plato actual en una variable
		char *plato = malloc(longPlatoActual);
		memcpy(plato, buffer + desplazamiento, longPlatoActual);
		desplazamiento += longPlatoActual;
		platoActual->plato = plato;
		// Copiamos el resto de propiedades del plato
		memcpy(&platoActual->cantidadPedida, buffer + desplazamiento, sizeof(int));
		desplazamiento += sizeof(int);
		memcpy(&platoActual->cantidadLista, buffer + desplazamiento, sizeof(int));
		desplazamiento += sizeof(int);
		memcpy(&platoActual->precio, buffer + desplazamiento, sizeof(int));
		desplazamiento += sizeof(int);
		// Por último, agregamos el plato a la lista
		list_add(platos, platoActual);
	}

	pedido->platos = platos;
	return pedido;
}

t_posicion *dsrlzRtaObtenerRestaurante(void *buffer) { // Por ahora
	t_posicion *posicion = malloc(sizeof(t_posicion));
	int desplazamiento = 0;

	memcpy(&posicion->posX, buffer, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(&posicion->posY, buffer + desplazamiento, sizeof(int));

	return posicion;
}

/* Métodos de recepción de paquetes */

t_header *recibirHeaderPaquete(int socket) {
	int proceso, mensaje;
	t_header *header = malloc(sizeof(t_header));

	void *buffer = malloc(sizeof(int)*2);

	if (recv(socket, buffer, sizeof(int)*2, MSG_WAITALL) != 0) {
		memcpy(&proceso, buffer, sizeof(int));
		memcpy(&mensaje, buffer + sizeof(int), sizeof(int));
		header->procesoOrigen = proceso;
		header->codigoOperacion = mensaje;
		if (proceso < 200) {
		log_info(logger, "[HEADER] Received \033[0;33m%s\033[0m from %s",
			getStringKeyValue(header->codigoOperacion, COMMANDNKEYS),
			getStringKeyValue(header->procesoOrigen, PROCNKEYS));
		}
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

void *recibirPayloadPaquete(t_header *header, int socket) {
	int size;

	void *buffer = recibirBuffer(&size, socket);

	switch (header->codigoOperacion) {
		case RTA_PLATO_LISTO:
		case CONSULTAR_PLATOS:
		case CONSULTAR_PEDIDO:
		case RTA_CREAR_PEDIDO:
		case RTA_ANIADIR_PLATO:
		case RTA_GUARDAR_PLATO:		
		case RTA_GUARDAR_PEDIDO:
		case RTA_CONFIRMAR_PEDIDO:
		case RTA_CONSULTAR_PEDIDO:
		case OBTENER_RESTAURANTE:
			buffer = dsrlzString(buffer, size);
			break;
		case RTA_OBTENER_RESTAURANTE:
			buffer = dsrlzRtaObtenerRestaurante(buffer);			
			break;
		case RTA_OBTENER_PEDIDO:
			buffer = dsrlzPedido(buffer, size);
			break;
        case GUARDAR_PEDIDO:
		case OBTENER_PEDIDO:
		case CONFIRMAR_PEDIDO:
            buffer = dsrlzReqPedido(buffer);
            break;
		case GUARDAR_PLATO:
			buffer = dsrlzReqPlato(buffer);
			break;
		case PLATO_LISTO:
		case ANIADIR_PLATO:
		case RTA_CONSULTAR_PLATOS:
		case RTA_CONSULTAR_RESTAURANTES:
			buffer = dsrlzListaStrings(buffer, size);
			break;
		default:
			printf("Qué ha pasao'?! џ(ºДºџ)\n");
			break;
	}

	log_info(logger, "Payload size: %d", size);
	return buffer;
}