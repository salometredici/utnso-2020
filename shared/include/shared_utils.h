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
    // Ver si vamos a tomar la rta a un comando así o de otra forma
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

// Diccionario de procesos

static t_keys diccionarioProcesos[] = {
    { "APP", APP },
    { "CLIENTE", CLIENTE },
    { "COMANDA", COMANDA },
    { "RESTAURANTE", RESTAURANTE },
    { "SINDICATO", SINDICATO }
};

#define PROCNKEYS (sizeof(diccionarioProcesos)/sizeof(t_keys))

// Diccionario de todos los comandos definidos para la API Global, utiliza los valores de m_code

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

char* getStringKeyValue(int key, int option);
int commandToString(char *key);

// Config

int obtenerPuertoEscucha();
char* obtenerNombreRestaurante();

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

typedef struct {
	p_code procesoOrigen;
	m_code codigoOperacion;
} t_header;

typedef struct {
    int posX;
	int posY;
} t_posicion;

// Métodos de envío y recepción de streams

void *recibirBuffer(int *size, int socket);
t_header *recibirHeaderPaquete(int socket);
t_buffer* recibirPayloadPaquete(t_header* header, int socket);
void enviarPaquete(int socket, p_code procesoOrigen, m_code codigoOperacion, int size, void* stream);

// Serializar

void *serializar(m_code codigoOperacion, void *stream, int *sizeStream);
void *srlzObtenerRestaurante(char *mensaje, int *size); 
void *srlzRtaObtenerRestaurante(t_posicion* posicion, int* size);

// Deserializar

t_buffer *dsrlzObtenerRestaurante(t_buffer *payload, void *buffer, int size);
t_buffer *dsrlzRtaObtenerRestaurante(t_buffer *payload, void *buffer);

// Cursed

typedef enum {
    TROCEAR = 3000,
    EMPANAR = 3001,
    REPOSAR = 3002,
    HORNEAR = 3003
} t_pasoReceta;

typedef struct {
    int cocinero;
    char* afinidad; // Hasta que definamos bien estos datos
} t_cocinero;

typedef struct {
    t_pasoReceta** pasosReceta;
    int** tiempoPasos;
} t_receta;

typedef struct {
    //t_cocinero** cocineros; // Tiene que ser una lista
    //t_receta** recetas; // Tiene que ser una lista [Milanesa, Papa]
    int cantidadHornos;
    t_posicion *posicionRestaurante;
} md_restaurante;

#endif