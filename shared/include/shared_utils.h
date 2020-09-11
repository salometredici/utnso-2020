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
	APP,
	CLIENTE,
	COMANDA,
	RESTAURANTE,
	SINDICATO
} p_code;

typedef enum{
    CONSULTAR_RESTAURANTES,
	SELECCIONAR_RESTAURANTE,
	OBTENER_RESTAURANTE,
	CONSULTAR_PLATOS,
	CREAR_PEDIDO,
	GUARDAR_PEDIDO,
	ANIADIR_PLATO,
	GUARDAR_PLATO,
	CONFIRMAR_PEDIDO,
	PLATO_LISTO,
	CONSULTAR_PEDIDO,
	OBTENER_PEDIDO,
	FINALIZAR_PEDIDO,
	TERMINAR_PEDIDO,
	OBTENER_RECETA,

	RTA_OBTENER_RESTAURANTE
} m_code;

// Commons

t_log *logger;
t_config *config;

void limpiarPantalla();
void inicializarProceso(p_code proceso);
void finalizarProceso();

// Config

int obtenerPuertoEscucha();

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