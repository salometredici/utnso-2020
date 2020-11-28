#ifndef SHARED_LOGGING_H
#define SHARED_LOGGING_H

#include <stdio.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include "../include/shared_commons.h"
#include "../include/shared_core.h"

#define TAB "\t"
#define BREAK "\n"

#define ESCAPE "\033"
#define BOLD "\033[1m"
#define RED "\033[0;31m"
#define BOLDRED "\033[1;31m"
#define GREEN "\033[0;32m"
#define BOLDGREEN "\033[1;32m"
#define YELLOW "\033[0;33m"
#define BOLDYELLOW "\033[1;33m"
#define BLUE "\033[0;34m"
#define BOLDBLUE "\033[1;34m"
#define MAGENTA "\033[0;35m"
#define BOLDMAGENTA "\033[1;35m"
#define CYAN "\033[0;36m" 
#define BOLDCYAN "\033[1;36m"
#define RESET "\033[0m"

void logRtaConsultarPlatos(t_list *platosEnviados);

void logConsoleInput(char *comandoLeido);
void logClientDisconnection(int socketCliente);
void logMetadataRestaurante(t_posicion posicion); // Después va a ser un t_md, es un response
void logHeader(m_code codigoOperacion, p_code procesoOrigen, int socket);
void logMessageSent(m_code codigoOperacion);
void logNewClientConnection(int socket);
void logConnectionCliente(p_code proceso);
void logRequest(t_request *request, m_code codigoOperacion);
void logTResult(t_result *result);
void logConsultarPedido(t_pedido *pedidoCons, int idPedido);
void logInitDataCliente(t_cliente *cliente);

/* Mensajes */

void log_ConfirmarPedido(t_request *request, m_code codigo_operacion);
void log_rta_ConfirmarPedido(t_result *result);
void log_ObtenerPedido(t_request *request, m_code codigo_operacion);
void log_rta_ObtenerPedido(t_pedido *pedido_obtenido, t_request *request);
void log_rta_GuardarPedido(t_result *result);
void log_ConsultarPlatos(char *restaurante);
void log_rta_ConsultarPlatos(t_list *platos);
void log_ObtenerReceta(char *receta_solicitada);
void log_rta_ObtenerReceta(t_receta *receta);
void log_ObtenerRestaurante(char *restaurante);
void log_rta_ObtenerRestaurante(t_md *md);
void log_GuardarPlato(t_req_plato *request_plato);
void log_rta_GuardarPlato(t_result *result);

// APP
void logClientInfo(t_cliente *cliente);

/* SINDICATO */

// Conexiones
void log_metadata_request(char *nombreRestaurante);
void log_AFIP_file_line(ssize_t line_size, size_t line_buf_size, char *current_line);

// Consola
void log_full_FS(int cantReq, int cantDisp);
void log_no_AFIP_content();
void log_Info_AFIP(char *rest);
void log_Pedido_AFIP(int nroPedido);
void log_Receta_AFIP(char *receta);

// Bloques
void log_assigned_blocks(int *assigned_blocks, int cant_bloques);
// Bitmap
void log_bitmap_file_error();
void log_bitmap_error();
void log_bitarray_info(t_bitarray *bitarray, int available_blocks);
void log_bitmap_init();
void log_bitmap_success();
void log_bitmap_reload();
void log_bitmap_reload_success();
// Bitmap updates
void log_bit_state(int pos, int bit);
void log_bit_update(int pos, t_bitarray *bitarray);
void log_unavailable_bit(int pos);
void log_bit_already_free(int bit_pos, int estado);

// CREAR_RESTAURANTE

void log_CrearRestaurante_Data(char **params);

// CREAR_RECETA

void log_CrearReceta_Data(char **params);

// CREAR_PEDIDO

void log_CrearPedido_Data(t_request *request);

// INICIAR PEDIDO

void log_IniciarPedido_Data(t_req_plato *request);

#endif