#ifndef SINDICATO_INIT_H
#define SINDICATO_INIT_H

#include "../../shared/include/shared_commons.h"
#include "../../shared/include/shared_core.h"
#include "../../shared/include/shared_serialization.h"
#include "tests.h"

#define METADATA_PATH "/Metadata/Metadata.AFIP"
#define BITMAP_PATH "/Metadata/Bitmap.bin"
#define BLOCKS_PATH "/Blocks/"
#define FILES_PATH "/Files/"
#define RESTAURANTES_PATH "/Files/Restaurantes/"
#define RECETAS_PATH "/Files/Recetas/"

char *puntoMontaje;
char *dirInicial;
t_config *metadata;

#endif