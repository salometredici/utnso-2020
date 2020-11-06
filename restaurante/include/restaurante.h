#ifndef RESTAURANTE_H
#define RESTAURANTE_H

#include "../../shared/include/shared_commons.h"
#include "../../shared/include/shared_core.h"
#include "../../shared/include/shared_serialization.h"
#include "../include/restaurante_init.h"
#include "../include/restaurante_planification.h"
#include "tests.h"

int conexion;
p_code procesoConectado;
int conexionSindicato;
char *nombreRestaurante;

pthread_t threadConexiones;
pthread_t threadPlanificacion;

char* respuesta;

#endif
