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
int getBytesString(char *string);
int getBytesListaStrings(t_list *listaStrings);
int getBytesReqPedido(t_request *request);
int getBytesReqPlato(t_req_plato *request);
int getBytesEjemplo();
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
void *srlzString(char *mensaje); 
void *srlzListaStrings(t_list *listaStrings);
void *srlzReqPedido(t_request *request);
void *srlzReqPlato(t_req_plato *request);
void *srlzPedido(t_pedido *pedido);
void *srlzRtaObtenerRestaurante(t_posicion *posicion);

// Deserializar

char *dsrlzString(void *buffer, int sizeString);
t_list *dsrlzListaStrings(void *buffer, int sizeLista);
t_request *dsrlzReqPedido(void *buffer);
t_req_plato *dsrlzReqPlato(void *buffer);
t_pedido *dsrlzPedido(void *buffer, int size);
t_posicion *dsrlzRtaObtenerRestaurante(void *buffer);

#endif