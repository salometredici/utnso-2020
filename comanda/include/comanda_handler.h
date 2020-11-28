#ifndef MEMORIA_HANDLER_H
#define MEMORIA_HANDLER_H

#include "../include/memoria_core.h"

t_result* _guardar_pedido(t_request *request);
t_result* _guardar_plato(t_req_plato *request);
t_pedido* _obtener_pedido(t_request *request);
t_result* _confirmar_pedido(t_request *request);
t_result* _plato_listo(t_plato_listo* request);
t_result* _plato_listo(t_plato_listo* request);
#endif