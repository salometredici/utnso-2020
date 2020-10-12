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
	blocksPath = malloc(strlen(dirInicial) + strlen(BLOCKS_PATH) + 1);
	filesPath = malloc(strlen(dirInicial) + strlen(FILES_PATH) + 1);
	strcpy(blocksPath, dirInicial);	strcpy(filesPath, dirInicial);
	strcat(blocksPath, BLOCKS_PATH); strcat(filesPath, FILES_PATH);
}

void initFromBaseDir(char *dir) {
	char *newDir = malloc(strlen(dirInicial) + strlen(dir) + 1);
	strcpy(newDir, dirInicial);
	strcat(newDir, dir);
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
	char *metadataPath = malloc(strlen(puntoMontaje) + strlen(METADATA_PATH) + 1);
	strcpy(metadataPath, puntoMontaje);
	strcat(metadataPath, METADATA_PATH);
	metadata = config_create(metadataPath);
}

void initBlocks() {

	int blocksNumber = getBlocksNumber();
	int blocksSize = getBlocksSize();
	int blocksPathLength = strlen(blocksPath);

	for (int i = 0; i< blocksNumber; i++) {
		// Obtenemos el nombre del archivo #i
		char *fileName;
		int res = asprintf(&fileName, "/%d.AFIP", i+1);

		char *fullPath = malloc(blocksPathLength + strlen(fileName) + 1);
		strcpy(fullPath, blocksPath);
		strcat(fullPath, fileName);
		free(fileName);

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
