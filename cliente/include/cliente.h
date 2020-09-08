#ifndef CLIENTE_H_
#define CLIENTE_H_

#include <stdlib.h>
#include <pthread.h>
#include <commons/string.h>
#include <readline/readline.h>
#include "../../shared/include/shared_utils.h"
#include "tests.h"

int conexionApp;
int conexionComanda;
int conexionRestaurante;
int conexionSindicato;

pthread_t threadConsola;

// Interfaz Cliente
void inicializarConsola();
void finalizarConsola();
void enviarConsultarRestaurantesAApp();
void enviarConsultarPlatosARestaurante();
void mostrarComandosValidos();

#endif