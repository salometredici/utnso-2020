#ifndef RESTAURANTE_H_
#define RESTAURANTE_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "../../shared/include/shared_utils.h"
#include "tests.h"

int conexionSindicato;
int socketServidor;
pthread_t threadConexiones;

char *nombre;

#endif
