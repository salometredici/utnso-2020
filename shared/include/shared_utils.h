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
#define ERROR -1

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

// API Global

typedef enum {
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

static t_keys diccionarioComandos[] = {
    { "CONSULTAR_RESTAURANTES", CONSULTAR_RESTAURANTES },
    { "SELECCIONAR_RESTAURANTE", SELECCIONAR_RESTAURANTE },
    { "OBTENER_RESTAURANTE", OBTENER_RESTAURANTE },
    { "CONSULTAR_PLATOS", CONSULTAR_PLATOS },
    { "CREAR_PEDIDO", CREAR_PEDIDO },
    { "GUARDAR_PEDIDO", GUARDAR_PEDIDO },
    { "ANIADIR_PLATO", ANIADIR_PLATO },
    { "GUARDAR_PLATO", GUARDAR_PLATO },
    { "CONFIRMAR_PEDIDO", CONFIRMAR_PEDIDO },
    { "PLATO_LISTO", PLATO_LISTO },
    { "CONSULTAR_PEDIDO", CONSULTAR_PEDIDO },
    { "OBTENER_PEDIDO", OBTENER_PEDIDO },
    { "FINALIZAR_PEDIDO", FINALIZAR_PEDIDO },
    { "TERMINAR_PEDIDO", TERMINAR_PEDIDO },
    { "OBTENER_RECETA", OBTENER_RECETA },

	{ "RTA_OBTENER_RESTAURANTE", RTA_OBTENER_RESTAURANTE }
};

#define COMMANDNKEYS (sizeof(diccionarioComandos)/sizeof(t_keys))

int commandToString(char *key) {
    t_keys *diccionario = diccionarioComandos;
    for (int i = 0; i < COMMANDNKEYS; i++) {
        t_keys sym = diccionario[i];
        if (strcmp(sym.key, key) == 0) {
            return sym.valor;
        }
    }
    return ERROR;
}

// Conexiones

int iniciarServidor();
int conectarseA(p_code proceso);
int aceptarCliente(int socketServidor);
void liberarConexion(int conexion);

// Serialización

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	p_code procesoOrigen;
	m_code codigoOperacion;
	t_buffer* buffer;
} t_paquete;

t_paquete* crearPaquete(p_code procesoOrigen, m_code codigoOperacion, int size, void* stream);
void* serializarPaquete(t_paquete* paquete, int bytes);
void enviarPaquete(t_paquete* paquete, int socketCliente);
void eliminarPaquete(t_paquete* paquete);
t_paquete* recibirHeaderPaquete(int socket);
t_paquete* recibirPayloadPaquete(t_paquete* header, int socket);

// Para limpiar o refactorizar

void* recibir_buffer(int*, int);
t_list* recibir_paquete(int);
void recibir_mensaje(int);
int recibir_operacion(int);
void enviar_mensaje(char* mensaje, int socket_cliente);

#endif