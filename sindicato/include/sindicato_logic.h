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

bool sabe_preparar_plato_restaurante(t_req_plato *request);
t_list *obtener_platos_restaurante(char *restaurante);
t_list *obtener_receta(char *receta_a_buscar);
t_md *obtener_restaurante(char *restaurante);
t_pedido *obtener_pedido(t_request *request);
t_result *check_and_add_plato(t_req_plato *request);

#endif