#ifndef SHARED_SERIALIZATION_H
#define SHARED_SERIALIZATION_H

#include "../include/shared_commons.h"
#include "../include/shared_core.h"
#include "../include/shared_logging.h"
#include "../include/shared_objects.h"

typedef struct {
	int size;
	void *stream;
} t_buffer;

typedef struct {
	p_code procesoOrigen;
	m_code codigoOperacion;
} t_header;

// GetBytes

int getBytesHeader();
int getBytesEjemplo();
int getBytesString(char *string);
int getBytesTResult(t_result *result);
int getBytesReq(t_request *request);
int getBytesReqPlato(t_req_plato *request);
int getBytesListaStrings(t_list *listaStrings);
int getBytesTPlatoListo(t_plato_listo *platoListo);
int getBytesTCliente(t_cliente *cliente);
int getBytesTReceta(t_receta *receta);
int getBytesTPosicion();
int getBytesTSeleccRest(t_selecc_rest *seleccRest);
int getBytesMd();
int getPayloadSize(m_code codigoOperacion, void *stream);
int getTamanioTotalPaquete(m_code codigoOperacion, void *stream);

// Métodos de envío y recepción de streams

void enviarPaquete(int socket, p_code procesoOrigen, m_code codigoOperacion, void *stream);
void *recibirBuffer(int *size, int socket);
t_header *recibirHeaderPaquete(int socket);
void *recibirPayloadPaquete(t_header *header, int socket);

// Serializar

void *serializar(m_code codigoOperacion, void *stream);
void serializarHeader(void *buffer, p_code procesoOrigen, m_code codigoOperacion);
void serializarPayload(void *buffer, m_code codigoOperacion, void *stream);
void *srlzInt(int valor);
void *srlzString(char *mensaje);
void *srlzMd(t_md *md);
void *srlzPedido(t_pedido *pedido);
void *srlzTCliente(t_cliente *cliente);
void *srlzTPlatoListo(t_plato_listo *platoListo);
void *srlzTResult(t_result *result);
void *srlzRequest(t_request *request);
void *srlzReqPlato(t_req_plato *request);
void *srlzListaStrings(t_list *listaStrings);
void *srlzTPosicion(t_posicion *posicion);
void *srlzTReceta(t_receta *receta);
void *srlzTSeleccRest(t_selecc_rest *seleccRest);

// Deserializar

char *dsrlzString(void *buffer, int sizeString);
t_list *dsrlzListaStrings(void *buffer, int sizeLista);
t_md *dsrlzMd(void *buffer, int size);
t_request *dsrlzRequest(void *buffer);
t_req_plato *dsrlzReqPlato(void *buffer);
t_pedido *dsrlzPedido(void *buffer, int size);
t_cliente *dsrlzTCliente(void *buffer);
t_result *dsrlzTResult(void *buffer);
t_posicion *dsrlzTPosicion(void *buffer);
t_selecc_rest *dsrlzTSeleccRest(void *buffer);
t_plato_listo *dsrlzTPlatoListo(void *buffer);
t_receta *dsrlzTReceta(void *buffer, int size);

#endif