#include "../include/sindicato_init.h"

char *getPuntoMontaje() {
	return config_get_string_value(config, "PUNTO_MONTAJE");
}

int getPuertoEscucha() {
	return config_get_int_value(config, "PUERTO_ESCUCHA");
}

char *get_full_bitmap_path() {
	char *bitmapPath = string_new();
	string_append_with_format(&bitmapPath, "%s%s", puntoMontaje, BITMAP_PATH);
	return bitmapPath;
}

bool existe_bitmap_previo() {
	char *path = get_full_bitmap_path();
	bool existe_bitmap = fdExists(path);
	free(path); // Para bajar el leak
	return existe_bitmap;
}

int get_available_blocks_number() {
	int cont = 0;
	pthread_mutex_lock(&mutexBitmap);
	for (int i = 0; i < blocksQuantity; i++) {
		if (bitarray_test_bit(bitarray, i) == 0) { cont++; }
	}
	pthread_mutex_unlock(&mutexBitmap);
	return cont;
}

/* Inicializaciones */

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

// Setea blockSize, blocksQuantity y maxContentSize
void setMetadata(t_config *metadata) {
	blockSize = config_get_int_value(metadata, "BLOCK_SIZE");
	blocksQuantity = config_get_int_value(metadata, "BLOCKS");
	maxContentSize = blockSize - sizeof(uint32_t);
}

// Crea un directorio a partir del dirInicial
void initFromBaseDir(char *dir) {
	char *newDir = string_new();
	string_append_with_format(&newDir, "%s%s", dirInicial, dir);
	createDirectory(newDir);
	free(newDir); // Para bajar el leak
}

// Seteamos las variables globales
void init_variables() {
	puertoEscucha = getPuertoEscucha();
	puntoMontaje = getPuntoMontaje();
	createDirectory(puntoMontaje);
	setBasePath();
	setBaseDirs();
}

// Creamos los directorios principales
void init_directories() {
	initFromBaseDir(BLOCKS_PATH);
	initFromBaseDir(FILES_PATH);
	initFromBaseDir(RECETAS_PATH);
	initFromBaseDir(RESTAURANTES_PATH);
}

// Obtiene los valores de Metadata.AFIP
void init_metadata() {
	char *metadataPath = string_new();
	string_append_with_format(&metadataPath, "%s%s", puntoMontaje, METADATA_PATH);
	t_config *metadata = config_create(metadataPath);
	setMetadata(metadata);
	free(metadataPath); config_destroy(metadata);
}

// Crea la cantidad BLOCKS de bloques #.AFIP
void init_blocks() {
	if (!existe_bitmap_previo()) {
		for (int i = 0; i < blocksQuantity; i++) {
			char *fullPath = string_new();
			string_append_with_format(&fullPath, "%s/%d.AFIP", blocksPath, i+1);
			FILE *fp = fopen(fullPath, "w");
			fclose(fp);
		}
	}
}

/* Bitmap */

// Retorna el bitarray inicial con todos los bits en cero
void set_initial_bitarray(char *bitmap, int size) {
	bitarray = bitarray_create_with_mode((char*)bitmap, size, LSB_FIRST);
	for (int i = 0; i < bitarray_get_max_bit(bitarray); i++) {
		bitarray_clean_bit(bitarray, i);
	}
	log_bitarray_info(bitarray, blocksQuantity, get_available_blocks_number());
}

void set_existent_bitarray(char *bitmap, int size) {
	bitarray = bitarray_create_with_mode((char*)bitmap, size, LSB_FIRST);
	log_bitarray_info(bitarray, blocksQuantity, get_available_blocks_number());
}

char *getBitmap(int size, int fd) {
	char *bitmap = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (bitmap == MAP_FAILED) {
		log_bitmap_error();
		exit(EXIT_FAILURE);
	} else {
		return bitmap;
	}
}

int getBitmapFile(char *path) {
	int fd = open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if (fd == ERROR) {
		log_bitmap_file_error();
		exit(EXIT_FAILURE);
	} else {
		return fd;
	}
}

void init_bitmap() {
	int bitmap_size = (blocksQuantity / 8) + 1; // Sólo va a tener de tamaño la cantidad de bloques en bits + 1
	char *bitmap_path = get_full_bitmap_path();
	if (!fdExists(bitmap_path)) {
		log_bitmap_init();
		// Crear archivo
		int bitmap_file = getBitmapFile(bitmap_path);
		ftruncate(bitmap_file, bitmap_size);
		// Crear bitmap
		bitmap = getBitmap(bitmap_size, bitmap_file);
		// Crear bitarray
		set_initial_bitarray(bitmap, bitmap_size);
		// Actualizamos el archivo
		msync(bitmap, bitmap_file, MS_SYNC);
		close(bitmap_file);
		log_bitmap_success();
	} else {
		log_bitmap_reload();
		int bitmap_file = getBitmapFile(bitmap_path);
		ftruncate(bitmap_file, bitmap_size);
		bitmap = getBitmap(bitmap_size, bitmap_file);
		set_existent_bitarray(bitmap, bitmap_size);
		close(bitmap_file);
		log_bitmap_reload_success();
	}
	free(bitmap_path); // Para bajar el leak
}

/* Init */

void init() {
	init_variables();
	init_directories();
	init_metadata();
	init_blocks();
	init_bitmap();
}
