#ifndef SHARED_LOGGING_H
#define SHARED_LOGGING_H

#include <stdio.h>
#include <commons/bitarray.h>
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
void logHeader(m_code codigoOperacion, p_code procesoOrigen);
void logMessageSent(m_code codigoOperacion);
void logNewClientConnection(int socket);
void logConnectionCliente(p_code proceso);
void logRequest(t_request *request, m_code codigoOperacion);
void logTResult(t_result *result);
void logConsultarPedido(t_pedido *pedidoCons, int idPedido);
void logInitDataCliente(t_cliente *cliente);

// APP
void logClientInfo(t_cliente *cliente);

/* SINDICATO */

// Conexiones
void log_metadata_request(char *nombreRestaurante);
void log_ConsultarPlatos(char *restaurante);
void log_AFIP_file_line(ssize_t line_size, size_t line_buf_size, char *current_line);

// Consola
void log_full_FS(int cantReq, int cantDisp);
void log_no_AFIP_content();
void log_Info_AFIP(char *rest);
void log_Pedido_AFIP(int nroPedido);
void log_Receta_AFIP(char *receta);

// Bitmap
void logBitmapFileError();
void logBitmapError();
void logInitialBitarrayInfo(t_bitarray *bitarray);
void logBitmapInit();
void logBitmapSuccess();
// Bitmap updates
void log_bit_state(int pos, int bit);
void log_bit_update(int pos, t_bitarray *bitarray);
void log_unavailable_bit(int pos);

// CREAR_RESTAURANTE

void log_CrearRestaurante_Data(char **params);

// CREAR_RECETA

void log_CrearReceta_Data(char **params);

#endif