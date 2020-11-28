#include "../include/shared_serialization.h"

/* Funciones que devuelven la cantidad de bytes de un payload */

int getBytesHeader() {
	return sizeof(int) * 2;
}

int getBytesString(char *string) {
	return strlen(string) + 1;
}

// Size de una lista de insts (bytes de cada inst + int por c/u + int de qPaso
int getBytesListaInstrucciones(t_list *listaInstrucciones){
	int cantidadElementos = list_size(listaInstrucciones);
	int bytesAEnviar = cantidadElementos * sizeof(int);

	for (int i = 0; i < cantidadElementos; i++) {
		t_instrucciones_receta *instruccion = list_get(listaInstrucciones, i);

		bytesAEnviar += getBytesString(instruccion->paso);
		bytesAEnviar += sizeof(int);
	}

	return bytesAEnviar;
}

// Size del plato y el int de su tamaño + size de la lista de instrucciones e int de su tamaño
int getBytesTReceta(t_receta *receta) {
	return getBytesListaInstrucciones(receta->instrucciones) + getBytesString(receta->plato) + sizeof(int) * 2;
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
int getBytesReq(t_request *request) {
    return sizeof(int) * 2 + getBytesString(request->nombre);
}

// Size de cuatro ints (idPedido, cantPlato, size de nombreRestaurante y size del plato) más la longitud de nombreRestaurante y la longitud del plato
int getBytesReqPlato(t_req_plato *request) {
	return sizeof(int) * 4 + getBytesString(request->restaurante) + getBytesString(request->plato);
}

// Size de 4 ints (precio, cantPedida, cantLista y size del plato) más la longitud del plato en sí-- antes
// Size de 3 ints (cantPedida, cantLista y size del plato) más la longitud del plato en sí --ahora
int getBytesPlato(t_plato *plato) {
	return sizeof(int) * 3 + getBytesString(plato->plato);
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

// Size de 3 ints (idPedido, valor de longR y longPlato) + size del R + size del plato
int getBytesTPlatoListo(t_plato_listo *platoListo) {
	return sizeof(int) * 3 + getBytesString(platoListo->restaurante) + getBytesString(platoListo->plato);
}

// Size de 4 ints (estado, precioTotal, size de la lista de platos y long del R) + size de cada t_plato + size del R
int getBytesPedido(t_pedido *pedido) {
	return sizeof(int) * 4 + getBytesListaPlatos(pedido->platos) + getBytesString(pedido->restaurante);
}

// Size de un bool, de un string y un int que representa el size del string
int getBytesTResult(t_result *result) {
	return sizeof(bool) + getBytesString(result->msg) + sizeof(int);
}

// Size de 2 ints (precio e int que representa bytes del plato) + bytes del nombre del plato
int getBytesReceta(t_md_receta *receta) {
	return sizeof(int) * 2 + getBytesString(receta->plato);
}

int getBytesListaRecetas(t_list *listaRecetas) {
	int bytesAEnviar = 0;
	int cantidadRecetas = list_size(listaRecetas);

	for (int i = 0; i < cantidadRecetas; i++) {
		int sizeReceta = getBytesReceta(list_get(listaRecetas, i));
		bytesAEnviar += sizeReceta;
	}

	return bytesAEnviar;
}

int getBytesTPosicion() {
	return sizeof(t_posicion);
}

// Size de un bool + 2 t_posicion (4 ints) + un int + 2 strings y sus respectivos 2 ints de tamaño
int getBytesTCliente(t_cliente *cliente) {
	return sizeof(int) * 3 + sizeof(bool) + getBytesString(cliente->idCliente) +  getBytesString(cliente->restSeleccionado)
			+ getBytesTPosicion(cliente->posCliente) + getBytesTPosicion(cliente->posRest);
}

// 7 ints (posX, posY, QHornos, QPedidos, QCocineros, bytesListaRecetas, bytesListaAfinidades) + size de lista de platos + size de lista de afinidades
// size de lista de afinidades = bytes de cada palabra + un int que represente esos bytes
// size de lista de platos = suma de cada size de t_md_receta
int getBytesMd(t_md *md) {
	int bytesInts = sizeof(int) * 7;
	int bytesListaRecetas = getBytesListaRecetas(md->platos);
	int bytesListaAfinidades = getBytesListaStrings(md->afinidades);
	return bytesInts + bytesListaRecetas + bytesListaAfinidades;
}

// size de restauranteSeleccionado + size de idCliente + 2 ints para esos valores
int getBytesTSeleccRest(t_selecc_rest *seleccRest) {
	return sizeof(int) * 2 + getBytesString(seleccRest->idCliente) + getBytesString(seleccRest->restauranteSeleccionado);
}

/* GetBytes de payload y buffer */

int getPayloadSize(m_code codigoOperacion, void *stream) {
	int payloadSize = 0;
	switch(codigoOperacion) {
        // Envío de t_request
		case ANIADIR_PLATO:
		case GUARDAR_PEDIDO:
		case OBTENER_PEDIDO:
		case TERMINAR_PEDIDO:
		case CONFIRMAR_PEDIDO:
		case FINALIZAR_PEDIDO:
            payloadSize += getBytesReq(stream);
            break;
		// Envío de t_plato_listo
		case PLATO_LISTO:
			payloadSize += getBytesTPlatoListo(stream);
			break;	
		// Envío de t_req_plato
		case GUARDAR_PLATO:
			payloadSize += getBytesReqPlato(stream);
			break;
		// Envío de t_pedido
		case RTA_OBTENER_PEDIDO:
			payloadSize += getBytesPedido(stream);
			break;
		case ENVIAR_DATACLIENTE:
			payloadSize += getBytesTCliente(stream);
			break;
		// Envío de t_selecc_rest
		case SELECCIONAR_RESTAURANTE:
			payloadSize += getBytesTSeleccRest(stream);
			break;
		// Envío de int
		case RTA_CREAR_PEDIDO:
		case RTA_OBTENER_PROCESO:
			payloadSize += sizeof(int);
			break;
		// Envío de un sólo string
		case OBTENER_RECETA:
        case CONSULTAR_PLATOS:
		case CONSULTAR_PEDIDO: // recibe id pedido
		case OBTENER_RESTAURANTE:
		case RTA_CONSULTAR_PEDIDO:
			payloadSize += getBytesString(stream);
			break;
		// Envío de un t_result
		case RTA_PLATO_LISTO:
		case RTA_ANIADIR_PLATO:
		case RTA_GUARDAR_PLATO:
		case RTA_GUARDAR_PEDIDO:
		case RTA_TERMINAR_PEDIDO:
		case RTA_CONFIRMAR_PEDIDO:
		case RTA_FINALIZAR_PEDIDO:
		case RTA_SELECCIONAR_RESTAURANTE:
			payloadSize += getBytesTResult(stream);
			break;
		// Envío de t_md
		case RTA_OBTENER_RESTAURANTE:
			payloadSize += getBytesMd(stream);
			break;
		// Envío de una lista de strings
		case RTA_CONSULTAR_PLATOS:
		case RTA_CONSULTAR_RESTAURANTES:
			payloadSize += getBytesListaStrings(stream);
			break;
		// Envío de una receta
		case RTA_OBTENER_RECETA:
			payloadSize += getBytesListaInstrucciones(stream);
			// TODO
			break;
		// Envío de t_receta
		case RTA_OBTENER_RECETA_2:
			payloadSize += getBytesTReceta(stream);
			break;
		// Si no tiene parámetros que serializar, queda en 0
		case CREAR_PEDIDO:
		case OBTENER_PROCESO:
		case CONSULTAR_RESTAURANTES:
		default:
			break;
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
		case ANIADIR_PLATO:
		case GUARDAR_PEDIDO:
		case OBTENER_PEDIDO:
		case TERMINAR_PEDIDO:
		case CONFIRMAR_PEDIDO:
		case FINALIZAR_PEDIDO:
            buffer = srlzRequest(stream);
            break;
		case GUARDAR_PLATO:
			buffer = srlzReqPlato(stream);
			break;
		case ENVIAR_DATACLIENTE:
			buffer = srlzTCliente(stream);
			break;
		case SELECCIONAR_RESTAURANTE:
			buffer = srlzTSeleccRest(stream);
			break;
		case RTA_CREAR_PEDIDO:
		case RTA_OBTENER_PROCESO:
			buffer = srlzInt(stream);
			break;
		case OBTENER_RECETA:
        case CONSULTAR_PLATOS:
		case CONSULTAR_PEDIDO: // recibe id pedido
		case OBTENER_RESTAURANTE:
		case RTA_CONSULTAR_PEDIDO:
			buffer = srlzString(stream);
			break;
		case RTA_PLATO_LISTO:
		case RTA_ANIADIR_PLATO:
		case RTA_GUARDAR_PLATO:
		case RTA_GUARDAR_PEDIDO:
		case RTA_TERMINAR_PEDIDO:
		case RTA_CONFIRMAR_PEDIDO:
		case RTA_FINALIZAR_PEDIDO:
		case RTA_SELECCIONAR_RESTAURANTE:
			buffer = srlzTResult(stream);
			break;
		case RTA_OBTENER_RESTAURANTE:
			buffer = srlzMd(stream);
			break;
		case RTA_OBTENER_PEDIDO:
			buffer = srlzPedido(stream);
			break;
		case PLATO_LISTO:
			buffer = srlzTPlatoListo(stream);
			break;
		case RTA_OBTENER_RECETA_2:
			buffer = srlzTReceta(stream);
			break;
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

void *srlzInt(int valor){
	void *magic = malloc(sizeof(int));
	memcpy(magic, &valor, sizeof(int));
	return magic;
}

// Serializar un sólo string
void *srlzString(char *mensaje) {
	int size =  getBytesString(mensaje);  // Tamaño de la palabra
	void *magic = malloc(size);
	memcpy(magic, mensaje, size);
	return magic;
}

// Función para serializar un t_result
void *srlzTResult(t_result *result) {
	int desplazamiento = 0;
	char *msg = result->msg;
	int size = getBytesTResult(result);
	int bytesPalabra = getBytesString(result->msg);

	void *magic = malloc(size);
	memcpy(magic, &bytesPalabra, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, msg, bytesPalabra);
	desplazamiento += bytesPalabra;
	memcpy(magic + desplazamiento, &result->hasError, sizeof(bool));

	return magic;
}

// Función para serializar una lista de strings
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

// Función para serializar un t_request
void *srlzRequest(t_request *request) {
    int desplazamiento = 0;
    int size = getBytesReq(request);
    int longitudPalabra = getBytesString(request->nombre);
	char *palabra = request->nombre;

    void *magic = malloc(size);
    memcpy(magic, &longitudPalabra, sizeof(int));
    desplazamiento += sizeof(int);
    memcpy(magic + desplazamiento, palabra, longitudPalabra);
    desplazamiento += longitudPalabra;
    memcpy(magic + desplazamiento, &request->idPedido, sizeof(int));

    return magic;
}

// Función para serializar un t_selecc_rest
void *srlzTSeleccRest(t_selecc_rest *seleccRest) {
	int desplazamiento = 0;
	int size = getBytesTSeleccRest(seleccRest);
	char *idCliente = seleccRest->idCliente;
	char *restaurante = seleccRest->restauranteSeleccionado;
	int longitudIdCliente = getBytesString(seleccRest->idCliente);
	int longitudRestaurante = getBytesString(seleccRest->restauranteSeleccionado);

	void *magic = malloc(size);
	memcpy(magic, &longitudIdCliente, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &longitudRestaurante, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, idCliente, longitudIdCliente);
	desplazamiento += longitudIdCliente;
	memcpy(magic + desplazamiento, restaurante, longitudRestaurante);

	return magic;
}

// Función para serializar un t_req_plato
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

// Función para serializar un t_plato_listo
void *srlzTPlatoListo(t_plato_listo *platoListo) {
	int desplazamiento = 0;
	int size = getBytesTPlatoListo(platoListo);
	char *plato = platoListo->plato;
	char *restaurante = platoListo->restaurante;
	int longPlato = getBytesString(plato);
	int longRestaurante = getBytesString(restaurante);

	void *magic = malloc(size);

	memcpy(magic, &platoListo->idPedido, sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(magic + desplazamiento, &longPlato, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &longRestaurante, sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(magic + desplazamiento, plato, longPlato);
	desplazamiento += longPlato;
	memcpy(magic + desplazamiento, restaurante, longRestaurante);
	desplazamiento += longRestaurante;

	return magic;
}

// Función para serializar un t_pedido
void *srlzPedido(t_pedido *pedido) {
	int desplazamiento = 0;
	t_list *listaPlatos = (t_list*) pedido->platos;

	int longListaPlatos = list_size(listaPlatos);
	int bytesListaPlatos = getBytesListaPlatos(listaPlatos);

	int longRestaurante = getBytesString(pedido->restaurante);
	char *rest = pedido->restaurante;

	int size = getBytesPedido(pedido);
	void *magic = malloc(size);

	memcpy(magic, &pedido->estado, sizeof(int)); // Estado del pedido
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &pedido->precioTotal, sizeof(int)); // Precio
	desplazamiento += sizeof(int);

	memcpy(magic + desplazamiento, &longRestaurante, sizeof(int)); // Size del nombre del restaurante
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, rest, longRestaurante); // Nombre del restaurante
	desplazamiento += longRestaurante;

	// Size de la lista de platos a serializar
	memcpy(magic + desplazamiento, &bytesListaPlatos, sizeof(int));
	desplazamiento += sizeof(int);

	// Serializamos la lista de platos (t_plato)
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
	}

	return magic;
}

// Función para serializar un t_receta
void *srlzTReceta(t_receta *receta) {
	int desplazamiento = 0;
	t_list *instrucciones = (t_list*) receta->instrucciones;
	
	int longListaInstrucciones = list_size(instrucciones);
	int bytesListaInstrucciones = getBytesListaInstrucciones(instrucciones);

	int long_nombre_receta = getBytesString(receta->plato);
	char *plato = receta->plato;

	int size = getBytesTReceta(receta);
	void *magic = malloc(size);

	// Serializamos lo anterior a la lista: plato y su longitud + el tamaño de la lista
	memcpy(magic, &long_nombre_receta, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, plato, long_nombre_receta);
	desplazamiento += long_nombre_receta;
	memcpy(magic + desplazamiento, &bytesListaInstrucciones, sizeof(int));
	desplazamiento += sizeof(int);

	// Serializamos la lista de instrucciones (t_instruccion_receta)
	for (int i = 0; i < longListaInstrucciones; i++) {
		t_instrucciones_receta *inst = list_get(instrucciones, i);
		int long_paso = getBytesString(inst->paso);
		// Vamos a copiar en el stream el tamaño del nombre de la instrucción, la instrucción y el tiempo de la misma
		memcpy(magic + desplazamiento, &long_paso, sizeof(int));
		desplazamiento += sizeof(int);
		memcpy(magic + desplazamiento, inst->paso, long_paso);
		desplazamiento += long_paso;
		memcpy(magic + desplazamiento, &inst->qPaso, sizeof(int));
		desplazamiento += sizeof(int);
	}

	return magic;
}

// Función para serializar un t_md
void *srlzMd(t_md *md) {
	int desplazamiento = 0;
	t_list *recetas = (t_list*) md->platos;
	t_list *afinidades = (t_list*) md->afinidades;

	int longListaRecetas = list_size(recetas);
	int bytesListaRecetas = getBytesListaRecetas(recetas);
	int longListaAfinidades = list_size(afinidades);
	int bytesListaAfinidades = getBytesListaStrings(afinidades);

	int size = getBytesMd(md);
	void *magic = malloc(size);

	// Serializamos todos los elementos que no son listas
	memcpy(magic, &md->cantidadCocineros, sizeof(int)); // cantidadCocineros
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &md->cantidadHornos, sizeof(int)); // cantidadHornos
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &md->cantidadPedidos, sizeof(int)); // cantidadPedidos
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &md->posX, sizeof(int)); // posX
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &md->posY, sizeof(int)); // posY
	desplazamiento += sizeof(int);

	memcpy(magic + desplazamiento, &bytesListaAfinidades, sizeof(int)); // Size de lista de afinideades
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &bytesListaRecetas, sizeof(int)); // Size de lista de recetas
	desplazamiento += sizeof(int);

	// Serializamos la lista de afinidades (strings)
	for (int i = 0; i < longListaAfinidades; i++) {
		char *afinidad = list_get(afinidades, i);
		int longitudPalabra = getBytesString(afinidad);
		// Vamos a copiar en el stream el tamaño de la palabra y la palabra, para saber cada uno al deserializar
		memcpy(magic + desplazamiento, &longitudPalabra, sizeof(int));
		desplazamiento += sizeof(int);
		memcpy(magic + desplazamiento, afinidad, longitudPalabra);
		desplazamiento += longitudPalabra;
	}

	// Serializamos la lista de t_md_receta
	for (int i = 0; i < longListaRecetas; i++) {
		t_md_receta *receta = list_get(recetas, i);
		int longNombrePlato = getBytesString(receta->plato);
		// Vamos a copiar en el stream el tamaño del nombre del plato y el plato, más su precio
		memcpy(magic + desplazamiento, &longNombrePlato, sizeof(int));
		desplazamiento += sizeof(int);
		memcpy(magic + desplazamiento, receta->plato, longNombrePlato);
		desplazamiento += longNombrePlato;
		memcpy(magic + desplazamiento, &receta->precio, sizeof(int));
		desplazamiento += sizeof(int);
	}

	return magic;
}

void *srlzTPosicion(t_posicion* posicion) {
	t_posicion* unaPosicion = (t_posicion*) posicion;
	int desplazamiento = 0;

	int size = getBytesTPosicion();

	void *magic = malloc(size);

	memcpy(magic + desplazamiento, &unaPosicion->posX, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &unaPosicion->posY, sizeof(int));

	return magic;
}

void *srlzTCliente(t_cliente *cliente) {
	int desplazamiento = 0;
	int size = getBytesTCliente(cliente);
	int longId = getBytesString(cliente->idCliente);
	int longRest = getBytesString(cliente->restSeleccionado);

	void *magic = malloc(size);
	
	memcpy(magic + desplazamiento, &longId, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &longRest, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, cliente->idCliente, longId);
	desplazamiento += longId;
	memcpy(magic + desplazamiento, cliente->restSeleccionado, longRest);
	desplazamiento += longRest;

	memcpy(magic + desplazamiento, &cliente->esRestaurante, sizeof(bool));
	desplazamiento += sizeof(bool);
	memcpy(magic + desplazamiento, &cliente->posCliente->posX, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &cliente->posCliente->posY, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &cliente->posRest->posX, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &cliente->posRest->posY, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &cliente->socketCliente, sizeof(int));
	desplazamiento += sizeof(int);

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

/* Funciones de envío de header/payload */

void enviarPaquete(int socket, p_code procesoOrigen, m_code codigoOperacion, void *stream) {
	int tamanioTotal = getTamanioTotalPaquete(codigoOperacion, stream);

	void *buffer = malloc(tamanioTotal);

	serializarHeader(buffer, procesoOrigen, codigoOperacion);
	serializarPayload(buffer, codigoOperacion, stream);
	
	enviarPorSocket(socket, buffer, tamanioTotal);
	logMessageSent(codigoOperacion);

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

// Función para deserializar un único string
char *dsrlzString(void *buffer, int sizeString) {
	char *cadena = malloc(sizeString);
	memcpy(cadena, buffer, sizeString);
	return cadena;
}

t_list *dsrlzListaInstrucciones(void *buffer, int sizeLista) {
	int longitudInstruccion;
	int desplazamiento = 0;
	t_list *valores = list_create();

	while (desplazamiento < sizeLista) {
		memcpy(&longitudInstruccion, buffer + desplazamiento, sizeof(int));
		desplazamiento += sizeof(int);
		t_instrucciones_receta *instruccion = malloc(sizeof(t_instrucciones_receta));
		memcpy(instruccion->paso, buffer + desplazamiento, longitudInstruccion);
		desplazamiento += longitudInstruccion;
		memcpy(instruccion->qPaso, buffer + desplazamiento, sizeof(int));
		desplazamiento += sizeof(int);
		list_add(valores, instruccion);
	}

	return valores;
}

// Función para deserializar una lista de strings, habiendo recibido en el buffer el tamaño de cada palabra inclusive
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

t_request *dsrlzRequest(void *buffer) {
	int longitudPalabra;
	int desplazamiento = 0;
	
	memcpy(&longitudPalabra, buffer, sizeof(int));
	desplazamiento += sizeof(int);

	char *restaurante = malloc(longitudPalabra);
	t_request *request = malloc(sizeof(t_request));

	memcpy(restaurante, buffer + desplazamiento, longitudPalabra);
	desplazamiento += longitudPalabra;
	memcpy(&request->idPedido, buffer + desplazamiento, sizeof(int));

	request->nombre = restaurante;
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
	
	free(buffer);
	
	return request;
}

t_plato_listo *dsrlzTPlatoListo(void *buffer) {
	int longPlato;
	int longRestaurante;
	int desplazamiento = 0;
	t_plato_listo *platoListo = malloc(sizeof(t_plato_listo));

	memcpy(&platoListo->idPedido, buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(&longPlato, buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(&longRestaurante, buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	char *plato = malloc(longPlato);
	char *restaurante = malloc(longRestaurante);
	memcpy(plato, buffer + desplazamiento, longPlato);
	desplazamiento += longPlato;
	memcpy(restaurante, buffer + desplazamiento, longRestaurante);
	desplazamiento += longRestaurante;
	
	platoListo->plato = plato;
	platoListo->restaurante = restaurante;
	return platoListo;
}

t_pedido *dsrlzPedido(void *buffer, int size) {
	int desplazamiento = 0;
	t_pedido *pedido = malloc(sizeof(t_pedido));
	t_list *platos = list_create();

	int longRestaurante = 0;
	int size_lista = 0;

	memcpy(&pedido->estado, buffer, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(&pedido->precioTotal, buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(&longRestaurante, buffer + desplazamiento, sizeof(int)); // Tamaño del nombre del restaurante
	desplazamiento += sizeof(int);
	
	char *rest = malloc(longRestaurante);
	memcpy(rest, buffer + desplazamiento, longRestaurante); // Nombre del restaurante
	desplazamiento += longRestaurante;
	pedido->restaurante = rest;

	// Size de la lista de platos a deserializar
	memcpy(&size_lista, buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	while (desplazamiento < size) {
		int longPlatoActual =0;
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
		// Por último, agregamos el plato a la lista
		list_add(platos, platoActual);
	}

	pedido->platos = platos;
	return pedido;
}

t_receta *dsrlzTReceta(void *buffer, int size) {
	int desplazamiento = 0;
	t_receta *receta = malloc(sizeof(t_receta));
	t_list *instrucciones = list_create();

	int long_nombre_receta = 0;
	int sizeListaInstrucciones = 0;

	// Deserializamos lo anterior a la lista de instrucciones
	memcpy(&long_nombre_receta, buffer, sizeof(int));
	desplazamiento += sizeof(int);
	
	char *plato = malloc(long_nombre_receta);
	memcpy(plato, buffer + desplazamiento, long_nombre_receta);
	desplazamiento += long_nombre_receta;
	receta->plato = plato;

	// Tamaño de la lista de instrucciones
	memcpy(&sizeListaInstrucciones, buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	while (desplazamiento < size) {
		int long_inst_actual = 0;
		t_instrucciones_receta *inst_actual = malloc(sizeof(t_instrucciones_receta));
		memcpy(&long_inst_actual, buffer + desplazamiento, sizeof(int)); // Tamaño del step
		desplazamiento += sizeof(int);
		char *paso = malloc(long_inst_actual);
		memcpy(paso, buffer + desplazamiento, long_inst_actual); // Step
		desplazamiento += long_inst_actual;
		inst_actual->paso = paso;
		memcpy(&inst_actual->qPaso, buffer + desplazamiento, sizeof(int)); // Tiempo del step
		desplazamiento += sizeof(int);
		// Agregamos el paso a la lista
		list_add(instrucciones, inst_actual);
	}

	receta->instrucciones = instrucciones;
	return receta;
}

t_md *dsrlzMd(void *buffer, int size) {
	int desplazamiento = 0;
	t_md *md = malloc(sizeof(t_md));
	t_list *platos = list_create();
	t_list *afinidades = list_create();

	int sizeListaRecetas = 0;
	int sizeListaAfinidades = 0;

	// Deserializamos lo que no está relacionado con las listas
	memcpy(&md->cantidadCocineros, buffer, sizeof(int)); // cantidadCocineros
	desplazamiento += sizeof(int);
	memcpy(&md->cantidadHornos, buffer + desplazamiento, sizeof(int)); // cantidadHornos
	desplazamiento += sizeof(int);
	memcpy(&md->cantidadPedidos, buffer + desplazamiento, sizeof(int)); // cantidadPedidos
	desplazamiento += sizeof(int);
	memcpy(&md->posX, buffer + desplazamiento, sizeof(int)); // posX
	desplazamiento += sizeof(int);
	memcpy(&md->posY, buffer + desplazamiento, sizeof(int)); // posY
	desplazamiento += sizeof(int);

	// Tamaño de la lista de afinidades
	memcpy(&sizeListaAfinidades, buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);
	// Tamaño de la lista de recetas
	memcpy(&sizeListaRecetas, buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	int dsplzListaRecetas = size - sizeListaRecetas;
	int dsplzListaAfinidades = size - sizeListaRecetas - sizeListaAfinidades;

	while (desplazamiento < dsplzListaRecetas) {
		int longAfinidadActual = 0;
		memcpy(&longAfinidadActual, buffer + desplazamiento, sizeof(int)); // Tamaño de la palabra
		desplazamiento += sizeof(int);
		char *afinidadActual = malloc(longAfinidadActual);
		memcpy(afinidadActual, buffer + desplazamiento, longAfinidadActual); // Afinidad actual
		desplazamiento += longAfinidadActual;
		// Agregamos la afinidad a la lista
		list_add(afinidades, afinidadActual);
	}
	
	while (desplazamiento < size) {
		int longRecetaActual = 0;
		t_md_receta *recetaActual = malloc(sizeof(t_md_receta));
		memcpy(&longRecetaActual, buffer + desplazamiento, sizeof(int)); // Tamaño de la palabra
		desplazamiento += sizeof(int);
		char *plato = malloc(longRecetaActual);
		memcpy(plato, buffer + desplazamiento, longRecetaActual); // Receta actual
		desplazamiento += longRecetaActual;
		recetaActual->plato = plato;
		memcpy(&recetaActual->precio, buffer + desplazamiento, sizeof(int)); // Precio de la receta
		desplazamiento += sizeof(int);
		// Agregamos la receta a la lista
		list_add(platos, recetaActual);
	}

	md->platos = platos;
	md->afinidades = afinidades;
	return md;
}

t_posicion *dsrlzTPosicion(void *buffer) {
	t_posicion *posicion = malloc(sizeof(t_posicion));
	int desplazamiento = 0;

	memcpy(&posicion->posX, buffer, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(&posicion->posY, buffer + desplazamiento, sizeof(int));

	return posicion;
}

t_cliente *dsrlzTCliente(void *buffer) {
	t_cliente *cliente = malloc(sizeof(t_cliente));
	cliente->posCliente = malloc(sizeof(t_posicion));
	cliente->posRest = malloc(sizeof(t_posicion));
	int desplazamiento = 0;
	int longId; int longRest;

	memcpy(&longId, buffer, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(&longRest, buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	char *id = malloc(longId);
	char *rest = malloc(longRest);

	memcpy(id, buffer + desplazamiento, longId);
	desplazamiento += longId;
	memcpy(rest, buffer + desplazamiento, longRest);
	desplazamiento += longRest;

	memcpy(&cliente->esRestaurante, buffer + desplazamiento, sizeof(bool));
	desplazamiento += sizeof(bool);

	memcpy(&cliente->posCliente->posX, buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(&cliente->posCliente->posY, buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(&cliente->posRest->posX, buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(&cliente->posRest->posY, buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(&cliente->socketCliente, buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	cliente->idCliente = id; cliente->restSeleccionado = rest;
	return cliente;
}

t_result *dsrlzTResult(void *buffer) {
	int longitudPalabra;
	int desplazamiento = 0;

	memcpy(&longitudPalabra, buffer, sizeof(int));
	desplazamiento += sizeof(int);

	char *msg = malloc(longitudPalabra);
	t_result *result = malloc(sizeof(t_result));

	memcpy(msg, buffer + desplazamiento, longitudPalabra);
	desplazamiento += longitudPalabra;
	memcpy(&result->hasError, buffer + desplazamiento, sizeof(bool));

	result->msg = msg;
	return result;
}

t_selecc_rest *dsrlzTSeleccRest(void *buffer) {
	int desplazamiento = 0;
	int longIdCliente = 0;
	int longRestaurante = 0;
	t_selecc_rest *seleccRest = malloc(sizeof(t_selecc_rest));

	memcpy(&longIdCliente, buffer, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(&longRestaurante, buffer + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	char *idCliente = malloc(longIdCliente);
	char *restaurante = malloc(longRestaurante);

	memcpy(idCliente, buffer + desplazamiento, longIdCliente);
	desplazamiento += longIdCliente;
	memcpy(restaurante, buffer + desplazamiento, longRestaurante);

	seleccRest->idCliente = idCliente;
	seleccRest->restauranteSeleccionado = restaurante;

	return seleccRest;
}

int dsrlzInt(void *buffer) {
	int valor;
	memcpy(&valor, buffer, sizeof(int));
	return valor;
}

/* Funciones de recepción de paquetes */

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
			logHeader(header->codigoOperacion, header->procesoOrigen);
		}
	} else {
		close(socket);
		header->procesoOrigen = ERROR;
		header->codigoOperacion = ERROR;
	}

	free(buffer);
	return header;
}

void *recibirBuffer(int *size, int socket){
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
		case RTA_CREAR_PEDIDO:
		case RTA_OBTENER_PROCESO:
			buffer = dsrlzInt(buffer);
			break;
		case OBTENER_RECETA:
        case CONSULTAR_PLATOS:
		case CONSULTAR_PEDIDO: // recibe id pedido
		case OBTENER_RESTAURANTE:
		case RTA_CONSULTAR_PEDIDO:
			buffer = dsrlzString(buffer, size);
			break;
		case RTA_PLATO_LISTO:
		case RTA_ANIADIR_PLATO:
		case RTA_GUARDAR_PLATO:
		case RTA_GUARDAR_PEDIDO:
		case RTA_TERMINAR_PEDIDO:
		case RTA_CONFIRMAR_PEDIDO:
		case RTA_FINALIZAR_PEDIDO:
		case RTA_SELECCIONAR_RESTAURANTE:
			buffer = dsrlzTResult(buffer);
			break;
		case RTA_OBTENER_RESTAURANTE:
			buffer = dsrlzMd(buffer, size);			
			break;
		case RTA_OBTENER_PEDIDO:
			buffer = dsrlzPedido(buffer, size);
			break;
		case SELECCIONAR_RESTAURANTE:
			buffer = dsrlzTSeleccRest(buffer);
			break;
		case ANIADIR_PLATO:
		case GUARDAR_PEDIDO:
		case OBTENER_PEDIDO:
		case TERMINAR_PEDIDO:
		case CONFIRMAR_PEDIDO:
		case FINALIZAR_PEDIDO:
            buffer = dsrlzRequest(buffer);
            break;
		case GUARDAR_PLATO:
			buffer = dsrlzReqPlato(buffer);
			break;
		case ENVIAR_DATACLIENTE:
			buffer = dsrlzTCliente(buffer);
			break;
		case PLATO_LISTO:
			buffer = dsrlzTPlatoListo(buffer);
			break;
		case RTA_CONSULTAR_PLATOS:
		case RTA_CONSULTAR_RESTAURANTES:
			buffer = dsrlzListaStrings(buffer, size);
			break;
		case RTA_OBTENER_RECETA:
			buffer = dsrlzListaInstrucciones(buffer, size);
			break;
		case RTA_OBTENER_RECETA_2:
			buffer = dsrlzTReceta(buffer, size);
			break;
		default:
			printf("Qué ha pasao'?! џ(ºДºџ)\n");
			break;
	}

	log_info(logger, "Payload size: %d", size);
	return buffer;
}

/***************FREE SERIALIZATION**********************/

void free_t_request(t_request* request){
	free(request->nombre);
	free(request);
}

void free_t_result(t_result* result){
	//free(result->msg);
	free(result);
}

void free_t_req_plato(t_req_plato* req){
	free(req->restaurante);
	free(req->plato);
	free(req);
}
