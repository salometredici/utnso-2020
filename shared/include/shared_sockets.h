#ifndef SHARED_SOCKETS_H
#define SHARED_SOCKETS_H

#include "../include/shared_commons.h"
#include "../include/shared_core.h"
#include "../include/shared_logging.h"
#include "../include/shared_objects.h"
#include "../include/shared_serialization.h"

// Inicialización

void inicializarProceso(p_code proceso);
void finalizarProceso();
char *crearLogRestaurante();
void crearLoggerProceso(char *log_path, char *program);

// Conexiones

int iniciarServidor();
int conectarseA(p_code proceso);
int aceptarCliente(int socketServidor);
void liberarConexion(int conexion);
int conectarse_a_cliente(char *ip, int puerto);

#endif