#include "../include/sindicato_init.h"

char *obtenerPuntoMontaje() {
	return config_get_string_value(config, "PUNTO_MONTAJE");
}

void initBaseDirs() {
	// Obtener la ruta del punto de montaje sin el dir /afip
	char *ultimoDir = strrchr(puntoMontaje, '/');
	int basePathLength = strlen(puntoMontaje) - strlen(ultimoDir);
	char *basePath = malloc(basePathLength + 1); strncpy(basePath, puntoMontaje, basePathLength);
	// Crear dir Blocks
	char *dirBlocks = malloc(basePathLength + strlen(BLOCKS_PATH) + 1);
	strcpy(dirBlocks, basePath);
	strcat(dirBlocks, BLOCKS_PATH);
	createDirectory(dirBlocks);
	// Crear dir Files
	char *dirFiles = malloc(basePathLength + strlen(FILES_PATH) + 1);
	strcpy(dirFiles, basePath);
	strcat(dirFiles, FILES_PATH);
	createDirectory(dirFiles);
}

void initBitMap() {
	char *bitMapPath = malloc(strlen(puntoMontaje) + strlen(BITMAP_PATH) + 1);
	strcpy(bitMapPath, puntoMontaje); strcat(bitMapPath, BITMAP_PATH);
	//createDirectory(bitMapPath);
}

void init() {
	puntoMontaje = obtenerPuntoMontaje();
	createDirectory(puntoMontaje);
	initBitMap();
	initBaseDirs();
	createDirectory(RECETAS_PATH);
	createDirectory(RESTAURANTES_PATH);
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
