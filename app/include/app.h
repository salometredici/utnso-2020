#ifndef APP_H
#define APP_H

#include "../../shared/include/shared_commons.h"
#include "../../shared/include/shared_core.h"
#include "../../shared/include/shared_serialization.h"
#include "../include/app_handler.h"
#include "../include/app_planification.h"
#include "tests.h"

int socketServidor;

pthread_t threadConexiones;
pthread_t threadPlanificacion;

#endif
