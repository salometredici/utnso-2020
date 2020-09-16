#ifndef APP_H
#define APP_H

#include "../../shared/include/shared_utils.h"
#include "../../shared/include/shared_commons.h"
#include "tests.h"

int conexionComanda;
int socketServidor;
pthread_t threadConexiones;

t_list *restaurantes;

#endif
