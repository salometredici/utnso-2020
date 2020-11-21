#include "../include/sindicato_impl.h"

int getBloquesNecesarios(int contentSize) {
	return (contentSize/maxContentSize) + (contentSize % maxContentSize != 0);
}

int getCantBloquesDisp() {
	int cont = 0;
	pthread_mutex_lock(&mutexBitmap);
	for (int i = 0; i < blocksQuantity; i++) {
		if (bitarray_test_bit(bitarray, i) == 0) { cont++; }
	}
	pthread_mutex_unlock(&mutexBitmap);
	return cont;
}

bool hayBloquesSuficientes(int bloquesReq) {
	return bloquesReq <= getCantBloquesDisp();
}

// Actualizar bitmap.bin

int buscar_bloque_libre_y_ocupar() {
    for (int i = 0; i < blocksQuantity; i++) {
		int currentBit = bitarray_test_bit(bitarray, i); logBitState(i, currentBit);
        if (!currentBit) {
            bitarray_set_bit(bitarray, i); // Si no está ocupado, lo reservamos
			logBitUpdate(i, bitarray);
            return i;
        } else {
			logUnavailableBit(i);
		}
    }
}

void asignar_bloques(uint32_t *bloquesAsignados, int bloquesReq) {
	for (int i = 0; i < bloquesReq; i++) {
		pthread_mutex_lock(&mutexBitmap);
		bloquesAsignados[i] = buscar_bloque_libre_y_ocupar(); // Reservamos los bloques que vamos a ocupar
		pthread_mutex_unlock(&mutexBitmap);
	}
}

// Generalizar para recetas y pedidos
char *getAFIPFileContent(char *rest, int fullSize, int initialBlock) {
	char *fileContent = string_new();
	string_append_with_format(&fileContent, "SIZE=%d\n", fullSize);
	string_append_with_format(&fileContent, "INITIAL_BLOCK=%d\n", initialBlock);
	logInfoAFIP(rest, initialBlock);
	return fileContent;
}

void guardarEnBloques(char *rest, char *fileContent, int bloquesReq) {
	char *aux = string_new();
	int start = 0;
	int nextBlockNumber = 0;
	int contentSize = strlen(fileContent);
	int nextBlockContentSize = contentSize < maxContentSize ? contentSize : maxContentSize;

	// Creamos el array para guardar los números de bloques asignados
	uint32_t bloquesAsignados[bloquesReq];
	asignar_bloques(bloquesAsignados, bloquesReq);

	// Obtenemos el contenido que vamos a grabar en Info.AFIP/#.AFIP/unaReceta.AFIP
	char *AFIPFileContent = getAFIPFileContent(rest, contentSize, bloquesAsignados[0]);
	
	// Si no existe el archivo, lo creamos
	char *infoAFIPPath = string_new();
	string_append_with_format(&infoAFIPPath, "%s%s%s%s", dirInicial, RESTAURANTES_PATH, rest, "/Info.AFIP");

	/// Guardamos el contenido en el archivo
	if (!fdExists(infoAFIPPath)) {
		FILE *fp = fopen(infoAFIPPath, "w+");
		if (fp != NULL) {
			fputs(AFIPFileContent, fp);
			fclose(fp);
		}
	}

	for (int i = 0; i < bloquesReq; i++) {

		// Obtenemos el tamaño del string a guardar y el substring
		aux = string_substring(fileContent, start, nextBlockContentSize);
		start += nextBlockContentSize;
		// Actualizamos el tamaño del bloque siguiente
		int contFaltante = contentSize - nextBlockContentSize;
		nextBlockContentSize = contFaltante < maxContentSize ? contFaltante : maxContentSize;

		char *blockToAssignPath = string_new();
		string_append_with_format(&blockToAssignPath, "%s%d.AFIP", blocksPath, bloquesAsignados[i]);

		FILE *fp = fopen(blockToAssignPath, "w+");
		if (fp != NULL) {
			fprintf(fp, "%s%d", aux, i+1 > bloquesReq ? ERROR : bloquesAsignados[i+1]);
			fclose(fp);
		}

	}

}

/* CREAR_RESTAURANTE */

char *getCrearRestauranteData(char **params) {
	char *fileContent = string_new();
	string_append_with_format(&fileContent, "CANTIDAD_COCINEROS=%s\n", params[2]);
	string_append_with_format(&fileContent, "POSICION=%s\n", params[3]);
	string_append_with_format(&fileContent, "AFINIDAD_COCINEROS=%s\n", params[4]);
	string_append_with_format(&fileContent, "PLATOS=%s\n", params[5]);
	string_append_with_format(&fileContent, "PRECIO_PLATOS=%s\n", params[6]);
	string_append_with_format(&fileContent, "CANTIDAD_HORNOS=%s\n", params[7]);
	string_append_with_format(&fileContent, "CANTIDAD_PEDIDOS=%s\n", params[8]);
	logCrearRestauranteData(params);
	return fileContent;
}

void crearDirRestaurante(char *restaurante) {
	char *newRestPath = string_new();
	string_append_with_format(&newRestPath, "%s%s", RESTAURANTES_PATH, restaurante);
	initFromBaseDir(newRestPath);
}

void crearRestaurante(char **params) {
	char *rest = params[1]; crearDirRestaurante(rest);
	char *fileContent = getCrearRestauranteData(params);
	int bloquesReq = getBloquesNecesarios(strlen(fileContent));
	if (hayBloquesSuficientes(bloquesReq)) {
		// 1. Ocupar los bloques en el bitmap
		// 2. Tener los números para ir grabándolos en info.bin y cada bloque

		guardarEnBloques(rest, fileContent, bloquesReq);
	} else {
		logFSFull(bloquesReq, getCantBloquesDisp());
		exit(EXIT_FAILURE);
	}	
}