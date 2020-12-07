#ifndef RESTAURANTE_PLANIFICATION_H
#define RESTAURANTE_PLANIFICATION_H

#include "../../shared/include/shared_commons.h"
#include "../../shared/include/shared_core.h"
#include "../../shared/include/shared_serialization.h"
#include "../include/restaurante_init.h"
#include "tests.h"

t_proceso *crearPcb(t_cliente *cliente, int idPedido, t_receta *receta);

void ejecutarCiclosFIFO();
void ejecutarCiclosRR();

void ejecutarCicloIO(t_queue_obj *currentCPU);

// void actualizarQE();
void actualizarQRaQE(t_queue_obj *currentCPU);
void actualizarQB(t_queue_obj *currentCPU);
void actualizarIO(t_queue_obj *currentCPU);

void aReadyPorAfinidad(t_proceso *pcb);

#endif