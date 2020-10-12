#ifndef SHARED_LOGGING_H
#define SHARED_LOGGING_H

#include <stdio.h>
#include "../include/shared_commons.h"
#include "../include/shared_core.h"

void logConsoleInput(char *comandoLeido);
void logClientDisconnection(int socketCliente);
void logMetadataRestaurante(t_posicion posicion); // Después va a ser un md_restaurante, es un response
void logHeader(m_code codigoOperacion, p_code procesoOrigen);
void logMessageSent(m_code codigoOperacion);
void logMetadataRequest(char *nombreRestaurante);
void logNewClientConnection(int socket);
void logConnectionCliente(p_code proceso);
void logRequest(t_request *request, m_code codigoOperacion);
void logConsultaPlatos(char *restaurante);

#endif