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

typedef enum {
    HACIA_CLIENTE = 1,
    HACIA_RESTAURANTE = 2
} tour_code;

/* Generales */

t_pcb *crearPcb(t_cliente *cliente, int idPedido);
void agregarAQN();
void actualizarQRconQN();
void actualizarQRconQB();

/* FIFO */

void actualizarQEconQR_FIFO();
void ejecutarCiclosFIFO();

/* HRRN */

void actualizarQEconQR_HRRN();
void ejecutarCiclosHRRN();

#endif
