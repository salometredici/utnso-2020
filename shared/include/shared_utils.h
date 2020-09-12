#ifndef SHARED_UTILS_H
#define SHARED_UTILS_H

#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/socket.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>

#define BASE_PATH "/utnso/tp-2020-2c-death-code/"

char* mi_funcion_compartida();

typedef struct {
	int socketThread;
} pthread_data;

typedef enum {
	APP = 0,
	CLIENTE = 1,
	COMANDA = 2,
	RESTAURANTE = 3,
	SINDICATO = 4
} p_code;

// API Global

typedef enum{
    CONSULTAR_RESTAURANTES = 100,
	SELECCIONAR_RESTAURANTE = 101,
	OBTENER_RESTAURANTE = 102,
	CONSULTAR_PLATOS = 103,
	CREAR_PEDIDO = 104,
	GUARDAR_PEDIDO = 105,
	ANIADIR_PLATO = 106,
	GUARDAR_PLATO = 107,
	CONFIRMAR_PEDIDO = 108,
	PLATO_LISTO = 109,
	CONSULTAR_PEDIDO = 110,
	OBTENER_PEDIDO = 111,
	FINALIZAR_PEDIDO = 112,
	TERMINAR_PEDIDO = 113,
	OBTENER_RECETA = 114,

	RTA_OBTENER_RESTAURANTE = 115
} m_code;

// Commons

t_log *logger;
t_config *config;

typedef struct {
    char *key;
    int valor;
} t_keys;

void limpiarPantalla();
void inicializarProceso(p_code proceso);
void finalizarProceso();

// Config

int obtenerPuertoEscucha();
char* obtenerNombreRestaurante();

// Conexiones

int iniciarServidor();
int conectarseA(p_code proceso);
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

typedef struct
{
	p_code proceso_origen;
	m_code codigo_operacion;
	t_buffer* buffer;
} t_paquete_v2;

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

t_paquete_v2* crear_paquete_v2(p_code proc_org, m_code cod_op, int size, void* stream);
void* serializar_paquete_v2(t_paquete_v2* paquete, int bytes);
void enviar_paquete_v2(t_paquete_v2* paquete, int socket_cliente);
void eliminar_paquete_v2(t_paquete_v2* paquete);
t_paquete_v2* recibir_header_paquete(int socket);
t_paquete_v2* recibir_payload_paquete(t_paquete_v2* header, int socket);

#endif