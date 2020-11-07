#ifndef COMANDA_INIT_H
#define COMANDA_INIT_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <commons/bitarray.h>
#include "../../shared/include/shared_commons.h"
#include "../../shared/include/shared_core.h"
#include "../../shared/include/shared_serialization.h"
#include "../include/comanda_handler.h"
#include "tests.h"

#define PAGE_SIZE 32 // por enunciado 

void **MEMORIA;
int MEMORY_SIZE;
int SWAP_SIZE;
char *ALGORITMO_REEMPLAZO;
int frames;

void init_sructures();
void print_structure();

#endif