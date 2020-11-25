#ifndef SINDICATO_H
#define SINDICATO_H

#include <stdio.h>
#include <commons/string.h>
#include <readline/readline.h>
#include "../../shared/include/shared_commons.h"
#include "../../shared/include/shared_core.h"
#include "../../shared/include/shared_logging.h"
#include "../../shared/include/shared_serialization.h"
#include "../include/sindicato_console.h"
#include "../include/sindicato_init.h"
#include "../include/sindicato_impl.h"
#include "../include/sindicato_logic.h"
#include "tests.h"

int socketServidor;
pthread_t threadConsola;
pthread_t threadConexiones;

#endif