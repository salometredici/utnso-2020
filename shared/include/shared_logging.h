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

/* Commons */

void logConsoleInput(char *comandoLeido);
void logMessageSent(m_code codigoOperacion);
void logConnectionCliente(p_code proceso);
void logNewClientConnection(int socket);
void log_TCliente_disconnection(t_cliente *cliente);
void logClientDisconnection(int socketCliente);

/* General */

void logHeader(m_code codigoOperacion, p_code procesoOrigen, int socket);
void logRequest(t_request *request, m_code codigoOperacion);
void logTResult(t_result *result);
void logConsultarPedido(t_pedido *pedidoCons, int idPedido);
void logInitDataCliente(t_cliente *cliente);

/* Planificación APP */

void log_app_added_to_new(char *algoritmo, int pid);
void log_app_removed_from_new(char *algoritmo, int pid);
void log_app_asignando_repartidores(char *algoritmo, int repartidores_disp, int exec_disp);
void log_app_repartidor_asignado(char *algoritmo, int idRepartidor, int pid);

void log_app_added_to_ready(char *algoritmo, int pid);
void log_app_removed_from_ready(char *algoritmo, int pid);
void log_app_ready_to_exec(char *algoritmo, int grado_multiprocesamiento, int size_qE);

void log_app_running_exec_cycle(char *algoritmo);
void log_app_added_to_exec(char *algoritmo, int pid);

void log_app_updating_QB_times(char *algoritmo);
void log_app_QB_times_increased(char *algoritmo);
void log_app_pasar_a_QB(char *algoritmo, int pid, bool llego_al_rest);
void log_app_unblocking_pcb(char *algoritmo, int idPedido);
void log_app_blocked_to_ready(char *algoritmo, int pid);
void log_app_pcb_rest_end(char *algoritmo, int pid, int qDescansado);
void log_app_repartidor_cansado(char *algoritmo, int pid);
void log_app_platos_pendientes(char *algoritmo, int pid);

void log_app_pcb_llego_al_cliente(char *algoritmo, int pid, char *idCliente);
void log_app_pcb_entregado_al_cliente(char *algoritmo, int pid, char *idCliente, int idRepartidor);
void log_app_continua_hacia_cliente(char *algoritmo, int pid);
void log_app_pcb_llego_al_rest(char *algoritmo, int pid);
void log_app_repartidor_en_camino(char *algoritmo, int pid, tour_code code);
void log_app_traslado_repartidor(char *algoritmo, int pid, int old_posX, int old_posY, int new_posX, int new_posY);
void log_app_repartidor_libre(char *algoritmo, int idRepartidor, int cant_disp);
void log_checking_all_platos_listos(char *algoritmo, int pid);
void log_app_FinalizarPedido(char *algoritmo, int pid);
void log_app_entrega_a_cliente(char *algoritmo, int pid, char *cliente);

/* HRRN */

void log_app_increasing_tiempos_espera();
void log_app_tiempos_espera_increased();
void log_app_next_pcb_HRRN();

/* SJF */

void log_app_next_pcb_SJF();

/* Mensajes */

void log_CrearPedido_app(char *cliente, char *rest);
void log_rta_CrearPedido(int new_id_pedido);
void log_rta_EnviarDataCliente(t_cliente *cliente);
void log_TerminarPedido(t_request *request, m_code codigo_operacion);
void log_rta_TerminarPedido(t_result *result);
void log_ConfirmarPedido(t_request *request, m_code codigo_operacion);
void log_rta_ConfirmarPedido(t_result *result);
void log_ObtenerPedido(t_request *request, m_code codigo_operacion);
void log_rta_ObtenerPedido(t_pedido *pedido_obtenido, t_request *request);
void log_rta_GuardarPedido(t_result *result);
void log_ConsultarPlatos_a_rest(char *rest);
void log_ConsultarPlatos_default(t_list *platos_rest_default);
void log_ConsultarPlatos(char *restaurante);
void log_rta_ConsultarPlatos(t_list *platos);
void log_ObtenerReceta(char *receta_solicitada);
void log_rta_ObtenerReceta(t_receta *receta);
void log_ObtenerRestaurante(char *restaurante);
void log_rta_ObtenerRestaurante(t_md *md);
void log_GuardarPlato(t_req_plato *request_plato);
void log_rta_GuardarPlato(t_result *result);
void log_PlatoListo(t_plato_listo *plato_listo);
void log_rta_PlatoListo(t_result *result);

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
void log_planif_step(char *step, char *desc);
void log_ConsultarPedido(int idPedido);
void log_rta_Consultar_Pedido(t_pedido *pedido, int idPedido);
#endif