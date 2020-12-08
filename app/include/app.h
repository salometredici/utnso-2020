#ifndef APP_H
#define APP_H

#include "../../shared/include/shared_commons.h"
#include "../../shared/include/shared_core.h"
#include "../../shared/include/shared_serialization.h"
#include "../include/app_main_impl.h"
#include "../include/app_planification.h"
#include "tests.h"

#define REST_NO_CONECTADO "EL RESTAURANTE NO ESTÁ CONECTADO A LA APP"
#define REST_ASOCIADO "EL CLIENTE HA SIDO ASOCIADO AL RESTAURANTE"

int socketServidor;

pthread_t threadConexiones;
pthread_t threadPlanificacion;

#endif
