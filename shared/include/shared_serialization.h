#ifndef SHARED_SERIALIZATION_H
#define SHARED_SERIALIZATION_H

#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/socket.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include "../include/shared_commons.h"
#include "../include/shared_core.h"

#define ERROR -1

// Structs

typedef struct {
	int size;
	void *stream;
} t_buffer;

typedef struct {
	p_code procesoOrigen;
	m_code codigoOperacion;
} t_header;

typedef struct {
    int posX;
	int posY;
} t_posicion;

typedef struct {
    char *restaurante;
    int idPedido;
} t_req_pedido;

typedef struct {
	char *restaurante;
	int idPedido;
	char *plato;
	int cantidadPlato;
} t_req_plato;

typedef enum {
	PENDIENTE = 1,
	CONFIRMADO = 2,
	FINALIZADO = 3
} t_estado;

// typedef struct {
// 	char *plato;
// 	int cantidadPedida;
// 	int cantidadLista;
// 	int precio;
// } t_plato;

typedef struct { // Ir actualizando con erratas del TP! No debería tener el id también?
	t_estado estado;
	t_list *platos;
	int precioTotal; // Quizás después corresponda un float o double
} t_pedido;

// GetBytes

int getBytesHeader();
int getBytesAEnviarString(char *string);
int getBytesAEnviarListaStrings(t_list *listaStrings);
int getBytesAEnviarReqPedido(t_req_pedido *request);
int getBytesAEnviarReqPlato(t_req_plato *request);
int getBytesAEnviarEjemplo();
int getPayloadSize(m_code codigoOperacion, void *stream);
int getTamanioTotalPaquete(m_code codigoOperacion, void *stream);

// Métodos de envío y recepción de streams

void enviarPaquete(int socket, p_code procesoOrigen, m_code codigoOperacion, void *stream);
void *recibirBuffer(int *size, int socket);
t_header *recibirHeaderPaquete(int socket);
t_buffer *recibirPayloadPaquete(t_header *header, int socket);

// Serializar

void *serializar(m_code codigoOperacion, void *stream);
void serializarHeader(void *buffer, p_code procesoOrigen, m_code codigoOperacion);
void serializarPayload(void *buffer, m_code codigoOperacion, void *stream);
void *srlzString(char *mensaje); 
void *srlzListaStrings(t_list *listaStrings);
void *srlzReqPedido(t_req_pedido *request);
void *srlzReqPlato(t_req_plato *request);
void *srlzPedido(t_pedido *pedido);
void *srlzRtaObtenerRestaurante(t_posicion *posicion);

// Deserializar

t_buffer *dsrlzString(t_buffer *payload, void *buffer, int sizeString);
t_buffer *dsrlzListaStrings(t_buffer *payload, void *buffer, int sizeLista);
t_buffer *dsrlzReqPedido(t_buffer *payload, void *buffer);
t_buffer *dsrlzReqPedido(t_buffer *payload, void *buffer);
t_buffer *dsrlzPedido(t_buffer *payload, void *buffer, int size);
t_buffer *dsrlzRtaObtenerRestaurante(t_buffer *payload, void *buffer);

#endif