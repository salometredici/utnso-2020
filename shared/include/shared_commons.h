#ifndef SHARED_COMMONS_H
#define SHARED_COMMONS_H

#include <math.h>
#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <readline/readline.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>

#define BASE_PATH "/home/utnso/tp-2020-2c-death-code/"
#define LOGS_PATH "/home/utnso/logs/"
#define ERROR -1

#define REST_NO_EXISTE "EL RESTAURANTE NO EXISTE EN EL FS"
#define RECETA_NO_EXISTE "LA RECETA NO EXISTE EN EL FS"
#define PEDIDO_NO_EXISTE "EL PEDIDO NO EXISTE EN EL FS"
#define YA_EXISTE_PEDIDO "EL PEDIDO YA EXISTE PARA EL RESTAURANTE SOLICITADO"
#define PEDIDO_CREADO "EL PEDIDO FUE CREADO CON ÉXITO"
#define ESTADO_AVANZADO "EL PEDIDO YA FUE CONFIRMADO/TERMINADO, NO SE PUEDE ACTUALIZAR"
#define PEDIDO_ACTUALIZADO "EL PEDIDO HA SIDO ACTUALIZADO"
#define BLOQUES_NO_ASIGNADOS "SOLICITUD SIN BLOQUES ASIGNADOS"

typedef enum {
	APP = 1,
	CLIENTE = 2,
	COMANDA = 3,
	RESTAURANTE = 4,
	SINDICATO = 5
} p_code;

// API Global

typedef enum {
    // Mensajes
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
	OBTENER_PROCESO = 115,
	ENVIAR_DATACLIENTE = 116,
    // Respuestas
    RTA_CONSULTAR_RESTAURANTES = 200,
	RTA_SELECCIONAR_RESTAURANTE = 201,
	RTA_OBTENER_RESTAURANTE = 202,
	RTA_CONSULTAR_PLATOS = 203,
	RTA_CREAR_PEDIDO = 204,
	RTA_GUARDAR_PEDIDO = 205,
	RTA_ANIADIR_PLATO = 206,
	RTA_GUARDAR_PLATO = 207,
	RTA_CONFIRMAR_PEDIDO = 208,
	RTA_PLATO_LISTO = 209,
	RTA_CONSULTAR_PEDIDO = 210,
	RTA_OBTENER_PEDIDO = 211,
	RTA_FINALIZAR_PEDIDO = 212,
	RTA_TERMINAR_PEDIDO = 213,
	RTA_OBTENER_RECETA = 214,
	RTA_OBTENER_PROCESO = 215,
	RTA_OBTENER_RECETA_2 = 216
} m_code;

/* Commons (todos los procesos van a tener estos tres) */

t_log *logger;
p_code process;
t_config *config;

typedef struct {
	int socketThread;
} pthread_data;

typedef struct {
    char *key;
    int valor;
} t_keys;

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
	{ "OBTENER_PROCESO", OBTENER_PROCESO },
	{ "ENVIAR_DATACLIENTE", ENVIAR_DATACLIENTE },
    { "RTA_OBTENER_RESTAURANTE", RTA_OBTENER_RESTAURANTE },
    { "RTA_CONSULTAR_RESTAURANTES", RTA_CONSULTAR_RESTAURANTES },
	{ "RTA_SELECCIONAR_RESTAURANTE", RTA_SELECCIONAR_RESTAURANTE },
	{ "RTA_CONSULTAR_PLATOS", RTA_CONSULTAR_PLATOS },
	{ "RTA_CREAR_PEDIDO", RTA_CREAR_PEDIDO },
	{ "RTA_GUARDAR_PEDIDO", RTA_GUARDAR_PEDIDO },
	{ "RTA_ANIADIR_PLATO", RTA_ANIADIR_PLATO },
	{ "RTA_GUARDAR_PLATO", RTA_GUARDAR_PLATO },
	{ "RTA_CONFIRMAR_PEDIDO", RTA_CONFIRMAR_PEDIDO },
	{ "RTA_PLATO_LISTO", RTA_PLATO_LISTO },
	{ "RTA_CONSULTAR_PEDIDO", RTA_CONSULTAR_PEDIDO },
	{ "RTA_OBTENER_PEDIDO", RTA_OBTENER_PEDIDO },
	{ "RTA_FINALIZAR_PEDIDO", RTA_FINALIZAR_PEDIDO },
	{ "RTA_TERMINAR_PEDIDO", RTA_TERMINAR_PEDIDO },
	{ "RTA_OBTENER_RECETA", RTA_OBTENER_RECETA },
	{ "RTA_OBTENER_RECETA_2", RTA_OBTENER_RECETA_2 },
	{ "RTA_OBTENER_PROCESO", RTA_OBTENER_PROCESO }
};

#define COMMANDNKEYS (sizeof(diccionarioComandos)/sizeof(t_keys))

// Devuelve el valor en string de un proceso/comando, el parámetro option corresponde al diccionario (PROCNKEYS o COMMANDNKEYS)
char *getStringKeyValue(int key, int option);
// Devuelve el número correspondiente a un comando para ser utilizado en un switch o comparación
int commandToString(char *key);
// Devuelve el p_code correspondiente a un int
p_code intToPCode(int key);

bool stringFound(char *expected, char *actual);

 /* Funciones de t_config */

int obtenerPuertoEscucha();
char *obtenerCliente();
char *obtenerLogFileName();
bool obtenerActiveConsole();
int obtenerLogLevel();

/* Structs */

typedef struct {
	char *idCliente;
	char *restauranteSeleccionado;
} t_selecc_rest;

typedef struct {
	bool hasError;
	char *msg;
} t_result;

typedef struct {
    int posX;
	int posY;
} t_posicion;

typedef struct {
    char *nombre;
    int idPedido;
} t_request;

typedef struct {
	char *idCliente; // Instancias de CLIENTE: CLIENTE_ID, Instancias de RESTAURANTE: NOMBRE_RESTAURANTE
	int socketCliente;
	bool esRestaurante;
	t_posicion *posCliente;
	// Propiedade que sólo van a tener valores si es una instancia de CLIENTE
	char *restSeleccionado;
	t_posicion *posRest;
} t_cliente;

// PLATOS

typedef struct {
	char *restaurante;
	int idPedido;
	char *plato;
	int cantidadPlato;
} t_req_plato;

// PEDIDOS

typedef enum {
	PENDIENTE = 1,
	CONFIRMADO = 2,
	FINALIZADO = 3,
	REST_INEXISTENTE = 4,
	PEDIDO_INEXISTENTE = 5,
	SIN_PLATOS = 6
} t_estado;

static t_keys diccionarioTEstados[] = {
    { "PENDIENTE", PENDIENTE },
	{ "CONFIRMADO", CONFIRMADO },
	{ "FINALIZADO", FINALIZADO },
	{ "REST_INEXISTENTE", REST_INEXISTENTE },
	{ "PEDIDO_INEXISTENTE", PEDIDO_INEXISTENTE },
	{ "SIN_PLATOS", SIN_PLATOS }
};

#define T_ESTADOSNKEYS (sizeof(diccionarioTEstados)/sizeof(t_keys))

// Devuelve el t_estado conrrespondiente a un string
t_estado string_to_t_estado(char *estado);

typedef struct {
	char *plato;
	int cantidadPedida;
	int cantidadLista;
	//int precio;
} t_plato;

typedef struct {
	char *restaurante;
	int idPedido;
	char *plato;
} t_plato_listo;

typedef struct {
	char *restaurante;
	t_estado estado;
	t_list *platos; // Es una lista de t_plato
	int precioTotal; // Quizás después corresponda un float o double
} t_pedido;

// METADATA RESTAURANTE

typedef struct {
	char *plato;
	int precio;
} t_md_receta;

/* Mapea todos los datos indicados a partir del archivo Info.AFIP del restaurante con el formato de la API
 	1. Lista de afinidades
	2. PosX
	3. PosY
	4. Lista de los platos con sus precios
	5. Cantidad de hornos
	6. Cantidad de pedidos
	7. Cantidad de cocineros
*/
typedef struct {
	int posX;
	int posY;
	int cantidadHornos;
	int cantidadPedidos;
	int cantidadCocineros;
	t_list *platos; // Es una lista de t_md_receta. Ej.: [{PapasAlHorno,55},{Lasagna,147}]
	t_list *afinidades; // Es una lista de strings. Ej.: [Milanesas]
} t_md;

/* Funciones */

typedef struct {
	char *paso;
	int qPaso;
} t_instrucciones_receta;

typedef struct {
	char *plato;
	t_list *instrucciones;
} t_receta;

void limpiarPantalla();

void mostrarListaStrings(t_list *listaStrings);
void mostrarListaPlatos(t_list *listaPlatos);

t_link_element* list_find_element(t_list *self, bool(*condition)(void*), int* index);
int calcularPrecioTotal(t_list *listaPlatos);
char *getStringEstadoPedido(t_estado estado);
double get_current_time();
#endif