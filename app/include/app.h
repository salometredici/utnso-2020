#ifndef APP_H
#define APP_H

#include "../../shared/include/shared_commons.h"
#include "../../shared/include/shared_core.h"
#include "../../shared/include/shared_serialization.h"
#include "../include/app_init.h"
#include "tests.h"

int socketServidor;

t_list *clientesConectados;

pthread_t threadConexiones;
pthread_t threadPlanificacion;

pthread_mutex_t mutexQN;
pthread_mutex_t mutexQR;
pthread_mutex_t mutexQE;
pthread_mutex_t mutexQB;

#endif
