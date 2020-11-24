#ifndef SINDICATO_IMPL_H
#define SINDICATO_IMPL_H

#include <sys/mman.h>
#include <commons/bitarray.h>
#include "../../shared/include/shared_commons.h"
#include "../../shared/include/shared_core.h"
#include "../../shared/include/shared_serialization.h"
#include "../include/sindicato_init.h"
#include "tests.h"

#define RESTAURANTE 1
#define RECETA 2
#define PEDIDO 3

char *get_restaurant_path(char *restaurante);
char *get_content_from_AFIP_file(int option, char *object);
char *get_full_pedido_path(t_request *request);
char *get_info(int option, char *object);

bool existe_restaurante(char *restaurante);
bool existe_receta(char *receta);
bool existe_pedido(t_request *request);

void crear_restaurante(char **params);
void crear_receta(char **params);
void crear_pedido(t_request *request);

// crear archivo
// borrar archivo
// validar archivo
// obtener data
// guardar data
// borrar data
// actualizar data

#endif