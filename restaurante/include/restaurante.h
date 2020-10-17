#ifndef RESTAURANTE_H_
#define RESTAURANTE_H_

#include "../../shared/include/shared_commons.h"
#include "../../shared/include/shared_core.h"
#include "../../shared/include/shared_serialization.h"
#include "../include/restaurante_init.h"
#include "tests.h"

int socketServidor;
int conexionSindicato;
char *nombreRestaurante;
pthread_t threadConexiones;

char* respuesta;

#endif
