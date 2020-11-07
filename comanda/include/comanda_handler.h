#ifndef MEMORIA_HANDLER_H
#define MEMORIA_HANDLER_H

#include "../include/memoria_core.h"

t_result* _guardar_pedido(char *nombre_rest, int id_pedido);
t_result* _guardar_plato(char *nombre_rest, int id_pedido, char *plato, int cantidad);
void _obtener_pedido(char *nombre_rest, int id_pedido);
void _confirmar_pedido(char *nombre_rest, int id_pedido);
void _plato_listo(char *nombre_rest, int id_pedido, char *plato_listo);
void _finalizar_pedido(char *nombre_rest, int id_pedido);

#endif