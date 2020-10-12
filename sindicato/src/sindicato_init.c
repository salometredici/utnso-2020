#include "../include/sindicato_init.h"

char *getPuntoMontaje() {
	return config_get_string_value(config, "PUNTO_MONTAJE");
}

int getBlocksNumber() {
	return config_get_int_value(metadata, "BLOCKS");
}

int getBlocksSize() {
	return config_get_int_value(metadata, "BLOCK_SIZE");
}

void obtenerBasePath() {
	// Obtener la ruta del punto de montaje sin el dir /afip
	char *ultimoDir = strrchr(puntoMontaje, '/');
	int basePathLength = strlen(puntoMontaje) - strlen(ultimoDir);
	dirInicial = malloc(basePathLength + 1);
	strncpy(dirInicial, puntoMontaje, basePathLength);
}

void setBaseDirs() {
	blocksPath = string_new();
	filesPath = string_new();
	string_append_with_format(&blocksPath, "%s%s", dirInicial, BLOCKS_PATH);
	string_append_with_format(&filesPath, "%s%s", dirInicial, FILES_PATH);
}

void initFromBaseDir(char *dir) {
	char *newDir = string_new();
	string_append_with_format(&newDir, "%s%s", dirInicial, dir);
	createDirectory(newDir);
}

void initDirectories() {
	// Seteamos las variables globales
	puntoMontaje = getPuntoMontaje();
	createDirectory(puntoMontaje);
	obtenerBasePath();
	setBaseDirs();
	// Creamos las carpetas
	initFromBaseDir(BLOCKS_PATH);
	initFromBaseDir(FILES_PATH);
	initFromBaseDir(RECETAS_PATH);
	initFromBaseDir(RESTAURANTES_PATH);
}

void initMetadata() {
	char *metadataPath = string_new();
	string_append_with_format(&metadataPath, "%s%s", puntoMontaje, METADATA_PATH);
	metadata = config_create(metadataPath);
}

void initBlocks() {
	int blocksNumber = getBlocksNumber();
	for (int i = 0; i< blocksNumber; i++) {
		char *fullPath = string_new();
		string_append_with_format(&fullPath, "%s/%d.AFIP", blocksPath, i+1);
		FILE *fp = fopen(fullPath, "w");
		fclose(fp);
	}
}

void initBitMap() {
	char *bitMapPath = malloc(strlen(puntoMontaje) + strlen(BITMAP_PATH) + 1);
	strcpy(bitMapPath, puntoMontaje); strcat(bitMapPath, BITMAP_PATH);
	FILE *fp = fopen(bitMapPath, "wb+");
	fclose(fp);
}

void init() {
	initDirectories();
	initMetadata();
	initBlocks();
	initBitMap();
}
