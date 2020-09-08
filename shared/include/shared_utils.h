#ifndef SHARED_UTILS_H
#define SHARED_UTILS_H

#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>

#define BASE_PATH "/utnso/tp-2020-2c-death-code/"

char* mi_funcion_compartida();

typedef struct {
	int socketThread;
} pthread_data;

typedef enum
{
	APP,
	CLIENTE,
	COMANDA,
	RESTAURANTE,
	SINDICATO
} p_code;

// Commons

t_log *logger;
t_config *config;

void inicializarProceso(p_code proceso);
void finalizarProceso();

// Config methods

char* obtenerPuertoEscucha();
int obtenerMaxClientes();

// Conexiones

int conectarseA(p_code proceso);
int iniciarServidor();
int aceptarCliente(int socketServidor);
void liberarConexion(int conexion);

//TODO

typedef enum
{
	MENSAJE,
	PAQUETE
} op_code;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

void* recibir_buffer(int*, int);
t_list* recibir_paquete(int);
void recibir_mensaje(int);
int recibir_operacion(int);
void enviar_mensaje(char* mensaje, int socket_cliente);
t_paquete* crear_paquete(void);
t_paquete* crear_super_paquete(void);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void eliminar_paquete(t_paquete* paquete);
void* serializar_paquete(t_paquete* paquete, int bytes);

#endif