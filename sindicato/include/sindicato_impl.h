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
bool existe_restaurante(char *restaurante);

void crear_restaurante(char **params);
void crear_receta(char **params);
void crear_pedido(char **params);

// crear archivo
// borrar archivo
// validar archivo
// obtener data
// guardar data
// borrar data
// actualizar data

#endif