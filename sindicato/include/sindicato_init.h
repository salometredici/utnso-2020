#ifndef SINDICATO_INIT_H
#define SINDICATO_INIT_H

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
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
int maxContentSize;

char *bitmap;
t_bitarray *bitarray;
pthread_mutex_t mutexBitmap;

void init();
int get_available_blocks_number();

#endif