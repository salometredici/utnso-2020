#ifndef CLIENTE_H_
#define CLIENTE_H_

#include <stdlib.h>
#include <commons/string.h>
#include <readline/readline.h>
#include "shared_utils.h"
#include "tests.h"

t_log* iniciar_logger(void);
t_config* leer_config(void);
void leer_consola(t_log* logger);
t_paquete* armar_paquete();

void _leer_consola_haciendo(void(*accion)(char*));

#endif