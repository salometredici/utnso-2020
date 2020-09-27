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
    char *restaurante; // A futuro puede pasar a ser un mensaje general
    int idPedido;
} t_req_pedido;

// Métodos de envío y recepción de streams

void *recibirBuffer(int *size, int socket);
t_header *recibirHeaderPaquete(int socket);
t_buffer *recibirPayloadPaquete(t_header *header, int socket);
void enviarPaquete(int socket, p_code procesoOrigen, m_code codigoOperacion, void *stream);

// Serializar

void *serializar(m_code codigoOperacion, void *stream);
void *srlzString(char *mensaje); 
void *srlzListaStrings(t_list *listaStrings);
void *srlzReqPedido(t_req_pedido *request);
void *srlzRtaObtenerRestaurante(t_posicion *posicion);

// Deserializar

t_buffer *dsrlzString(t_buffer *payload, void *buffer, int sizeString);
t_buffer *dsrlzRtaObtenerRestaurante(t_buffer *payload, void *buffer);
t_buffer *dsrlzListaStrings(t_buffer *payload, void *buffer, int sizeLista);

#endif