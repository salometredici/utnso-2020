#ifndef APP_MAIN_IMPL_H
#define APP_MAIN_IMPL_H

#include "../../shared/include/shared_commons.h"
#include "../../shared/include/shared_core.h"
#include "../../shared/include/shared_objects.h"
#include "../../shared/include/shared_logging.h"
#include "../../shared/include/shared_serialization.h"
#include "../include/app_init.h"
#include "tests.h"

void bind_client_to_rest(t_cliente *cliente, char *restSelecc);
bool es_valido_restaurante(char *rest);
int generar_id(int min, int max);
void actualizar_clientes_conectados(t_cliente *cliente);
t_cliente *get_rest_conectado(char *rest_buscado);
bool esta_conectado_rest(char *rest_buscado);

#endif