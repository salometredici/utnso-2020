#ifndef SINDICATO_INIT_H
#define SINDICATO_INIT_H

#include <sys/mman.h>
#include <commons/bitarray.h>
#include "../../shared/include/shared_commons.h"
#include "../../shared/include/shared_core.h"
#include "../../shared/include/shared_serialization.h"
#include "tests.h"

#define FILES_PATH "/Files/"
#define BLOCKS_PATH "/Blocks/"
#define BITMAP_PATH "/Metadata/Bitmap.bin"
#define METADATA_PATH "/Metadata/Metadata.AFIP"
#define RECETAS_PATH "/Files/Recetas/"
#define RESTAURANTES_PATH "/Files/Restaurantes/"

int puertoEscucha;

char *puntoMontaje;
char *dirInicial;
char *blocksPath;
char *filesPath;

int blockSize;
int blocksQuantity;

t_bitarray *bitarray;

pthread_mutex_t mx1;
pthread_mutex_t mx2;

void init();

// crear archivo
// borrar archivo
// validar archivo
// obtener data
// guardar data
// borrar data
// actualizar data

#endif