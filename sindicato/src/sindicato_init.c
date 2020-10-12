#include "../include/sindicato_init.h"

char *obtenerPuntoMontaje() {
	return config_get_string_value(config, "PUNTO_MONTAJE");
}

void init() {
	puntoMontaje = obtenerPuntoMontaje();
	createDirectory(puntoMontaje);

}

// void we() {

// 	int blocksnumber = config_get_int_value(metadata, "BLOCKS");
// 	int blocksize = config_get_int_value(metadata, "BLOCK_SIZE");

// 	for (int i = 0; i<blocksnumber; i++) {
// 		int baseLength  = string_length(AFIP_PATH);
// 		char *fileName;
// 		int e = asprintf(&fileName, "%d.AFIP", i+1);
// 		printf("%s", fileName);
// 		int secLength = string_length(fileName);
// 		char we[baseLength + secLength];
// 		strcpy(we, AFIP_PATH);
// 		printf("base path: %s", we);
// 		strcat(we, fileName);
// 		printf("full path: %s", we);
// 		free(fileName);
// 		FILE *fp = fopen(we, "w");
// 		fclose(fp);
// 	}
// }
