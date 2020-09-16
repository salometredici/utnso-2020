#ifndef RESTAURANTE_H_
#define RESTAURANTE_H_

#include "../../shared/include/shared_utils.h"
#include "../../shared/include/shared_commons.h"
#include "tests.h"

int socketServidor;
int conexionSindicato;
char *nombreRestaurante;
md_restaurante *mdRestaurante;
pthread_t threadConexiones;

#endif
