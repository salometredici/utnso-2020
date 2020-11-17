#ifndef SINDICATO_H
#define SINDICATO_H

#include <stdio.h>
#include <commons/string.h>
#include <readline/readline.h>
#include "../../shared/include/shared_commons.h"
#include "../../shared/include/shared_core.h"
#include "../../shared/include/shared_serialization.h"
#include "../include/sindicato_init.h"
#include "tests.h"

int socketServidor;
pthread_t threadConsola;
pthread_t threadConexiones;

void inicializarConsola();
void finalizarConsola();
void mostrarComandosValidos();

// Mensajes correspondientes a m_code que Sindicato puede recibir por socket

static t_keys diccionarioMsgs[] = {
    { "CONSULTAR_PLATOS", CONSULTAR_PLATOS },
    { "GUARDAR_PEDIDO", GUARDAR_PEDIDO },
    { "GUARDAR_PLATO", GUARDAR_PLATO },
    { "CONFIRMAR_PEDIDO", CONFIRMAR_PEDIDO },
    { "OBTENER_PEDIDO", OBTENER_PEDIDO },
    { "OBTENER_RESTAURANTE", OBTENER_RESTAURANTE },
    { "PLATO_LISTO", PLATO_LISTO },
    { "TERMINAR_PEDIDO", TERMINAR_PEDIDO },
    { "OBTENER_RECETA", OBTENER_RECETA }
};

#define MSGSNKEYS (sizeof(diccionarioMsgs)/sizeof(t_keys))

// Consola

#define OPT_CREAR_RESTAURANTE 1
#define OPT_CREAR_RECETA 2
#define OPT_AIUDA 3
#define OPT_BAI 4
#define OPT_CLEAR 5

static t_keys diccionarioConsola[] = {
    { "CREAR_RESTAURANTE", OPT_CREAR_RESTAURANTE },
    { "CREAR_RECETA", OPT_CREAR_RECETA },
    { "AIUDA", OPT_AIUDA },
    { "BAI", OPT_BAI },
    { "CLEAR", OPT_CLEAR }
};

#define CONSOLENKEYS (sizeof(diccionarioConsola)/sizeof(t_keys))

#endif