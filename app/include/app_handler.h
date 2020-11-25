#ifndef APP_HANDLER_H
#define APP_HANDLER_H

#include "../include/app_init.h"

t_list* _consultar_restaurantes();
t_result* _seleccionar_restaurante(t_cliente *cliente, t_selecc_rest *tupla_cliente_rest);

#endif