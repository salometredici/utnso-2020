#ifndef RESTAURANTE_PLANIFICATION_H
#define RESTAURANTE_PLANIFICATION_H

#include "../../shared/include/shared_commons.h"
#include "../../shared/include/shared_core.h"
#include "../../shared/include/shared_serialization.h"
#include "../include/restaurante_init.h"
#include "tests.h"

pthread_mutex_t mutexQB;

t_proceso *crearPcb(t_cliente *cliente, int idPedido, t_receta *receta);