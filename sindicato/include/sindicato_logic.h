#ifndef SINDICATO_LOGIC_H
#define SINDICATO_LOGIC_H

#include <sys/mman.h>
#include <commons/bitarray.h>
#include "../../shared/include/shared_commons.h"
#include "../../shared/include/shared_core.h"
#include "../../shared/include/shared_serialization.h"
#include "../include/sindicato_impl.h"
#include "../include/sindicato_init.h"
#include "tests.h"

t_list *get_t_plato_list_from_lists(t_list *platos, t_list *cant_pedidas, t_list *cant_listas);
t_list *get_platos_con_precios_from_rest(char *info_restaurante);

t_md *get_md_from_string(char *afip_content);
t_pedido *get_pedido_from_string(char *info, t_request *request, char *info_restaurante);

int obtener_precio_pedido(t_list *platos_con_precios, t_list *platos_pedido);
t_list *obtener_platos_restaurante(char *restaurante);
t_list *obtener_receta(char *receta_a_buscar);
t_md *obtener_restaurante(char *restaurante);

bool sabe_preparar_plato_restaurante(t_req_plato *request);
t_result *check_and_add_plato(t_req_plato *request);

#endif