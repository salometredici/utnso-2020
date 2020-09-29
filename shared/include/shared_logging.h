#ifndef SHARED_LOGGING_H
#define SHARED_LOGGING_H

#include "../include/shared_commons.h"
#include "../include/shared_core.h"

void logConsoleInput(char *comandoLeido);
void logClientDisconnection(int socketCliente);
void logMetadataRestaurante(t_posicion posicion); // Después va a ser un md_restaurante

#endif