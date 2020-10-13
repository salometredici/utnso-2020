#include "../include/sindicato_init.h"

char *getPuntoMontaje() {
	return config_get_string_value(config, "PUNTO_MONTAJE");
}

int getPuertoEscucha() {
	return config_get_int_value(config, "PUERTO_ESCUCHA");
}

void setBasePath() {
	// Obtener la ruta del punto de montaje sin el dir /afip
	char *ultimoDir = strrchr(puntoMontaje, '/');
	int basePathLength = strlen(puntoMontaje) - strlen(ultimoDir);
	dirInicial = malloc(basePathLength + 1);
	strncpy(dirInicial, puntoMontaje, basePathLength);
}

// Setea blocksPath y filesPath
void setBaseDirs() {
	blocksPath = string_new();
	filesPath = string_new();
	string_append_with_format(&blocksPath, "%s%s", dirInicial, BLOCKS_PATH);
	string_append_with_format(&filesPath, "%s%s", dirInicial, FILES_PATH);
}

// Setea blockSize y blocksQuantity
void setMetadata(t_config *metadata) {
	blockSize = config_get_int_value(metadata, "BLOCK_SIZE");
	blocksQuantity = config_get_int_value(metadata, "BLOCKS");
}

// Crea un directorio a partir del dirInicial
void initFromBaseDir(char *dir) {
	char *newDir = string_new();
	string_append_with_format(&newDir, "%s%s", dirInicial, dir);
	createDirectory(newDir);
}

// Seteamos las variables globales
void initVariables() {
	puertoEscucha = getPuertoEscucha();
	puntoMontaje = getPuntoMontaje();
	createDirectory(puntoMontaje);
	setBasePath();
	setBaseDirs();
}

// Creamos los directorios principales
void initDirectories() {
	initFromBaseDir(BLOCKS_PATH);
	initFromBaseDir(FILES_PATH);
	initFromBaseDir(RECETAS_PATH);
	initFromBaseDir(RESTAURANTES_PATH);
}

// Obtiene los valores de Metadata.AFIP
void initMetadata() {
	char *metadataPath = string_new();
	string_append_with_format(&metadataPath, "%s%s", puntoMontaje, METADATA_PATH);
	t_config *metadata = config_create(metadataPath);
	setMetadata(metadata);
	config_destroy(metadata);
}

// Crea la cantidad BLOCKS de bloques #.AFIP
void initBlocks() {
	for (int i = 0; i < blocksQuantity; i++) {
		char *fullPath = string_new();
		string_append_with_format(&fullPath, "%s/%d.AFIP", blocksPath, i+1);
		FILE *fp = fopen(fullPath, "w");
		fclose(fp);
	}
}

void initBitMap() {
	int bitmapSize = blocksQuantity * blockSize; // Tamaño del bitmap en bytes
	char *bitmapPath = string_new();
	string_append_with_format(&bitmapPath, "%s%s", puntoMontaje, BITMAP_PATH);

	if (!fdExists(bitmapPath)) {
		FILE *fp = fopen(bitmapPath, "wb+"); // Y si no existe logueamos algo? 
		ftruncate(fp, bitmapSize);
		char *bitmap = mmap(NULL, bitmapSize, PROT_READ | PROT_WRITE, MAP_SHARED, fp, 0);
		bitarray = bitarray_create_with_mode(bitmap, blocksQuantity/8, LSB_FIRST); // Falta revisar que sea siempre múltiplo de 8
		fclose(fp);
	}

}

void init() {
	initVariables();
	initDirectories();
	initMetadata();
	initBlocks();
	initBitMap();
}
