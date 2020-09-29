#ifndef SHARED_CORE_H
#define SHARED_CORE_H

#include "../include/shared_commons.h"

char *mi_funcion_compartida();

typedef struct {
	int socketThread;
} pthread_data;

void inicializarProceso(p_code proceso);
void finalizarProceso();
char *crearLogRestaurante();

// Logs

void logConsoleInput(char *comandoLeido);
void logClientDisconnection(int socketCliente);
void logMetadataRestaurante(t_posicion posicion); // Después va a ser un md_restaurante

// Config

int obtenerPuertoEscucha();
char *obtenerNombreRestaurante();
bool obtenerActiveConsole();
int obtenerLogLevel();

// Conexiones

int iniciarServidor();
int conectarseA(p_code proceso);
int aceptarCliente(int socketServidor);
void liberarConexion(int conexion);

#endif