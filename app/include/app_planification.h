#ifndef APP_H
#define APP_H

#include "../../shared/include/shared_commons.h"
#include "../../shared/include/shared_core.h"
#include "../../shared/include/shared_serialization.h"
#include "../include/app_init.h"
#include "tests.h"

pthread_mutex_t mutexQN;
pthread_mutex_t mutexQR;
pthread_mutex_t mutexQE;
pthread_mutex_t mutexQB;

/* Funciones */

t_repartidor *getRepartidorMasCercano(t_posicion *posRest);
double getDistancia(t_posicion *posRepartidor, t_posicion *posRestaurante);

/* Generales */

t_pcb *crear_pcb(t_cliente *cliente, int idPedido);
void agregar_a_QN();
void pasar_a_QB(t_pcb *pcb, t_estado estado);
void desbloquear_PCB(int idPedido);
void update_QR_con_QN();
void update_QR_con_QB();
void ejecutar_ciclos();

/* FIFO */

void update_QE_con_QR_FIFO();

/* HRRN */

void update_QE_con_QR_HRRN();

/* SJF */

void update_QE_con_QR_SJF();

#endif
