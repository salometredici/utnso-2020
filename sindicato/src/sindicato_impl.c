#include "../include/sindicato_impl.h"

int get_required_blocks_number(int contentSize) {
	return (contentSize/maxContentSize) + (contentSize % maxContentSize != 0);
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

bool enough_blocks_available(int bloquesReq) {
	return bloquesReq <= get_available_blocks_number();
}

// Actualizar bitmap.bin

int buscar_bloque_libre_y_ocupar() {
    for (int i = 0; i < blocksQuantity; i++) {
		int currentBit = bitarray_test_bit(bitarray, i); log_bit_state(i, currentBit);
        if (!currentBit) { // Si no está ocupado, lo reservamos
            bitarray_set_bit(bitarray, i);
			log_bit_update(i, bitarray);
            return i;
        } else {
			log_unavailable_bit(i);
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

char *get_AFIP_file_content(int fullSize, int initialBlock) {
	char *fileContent = string_new();
	string_append_with_format(&fileContent, "SIZE=%d\n", fullSize);
	string_append_with_format(&fileContent, "INITIAL_BLOCK=%d\n", initialBlock);
	return fileContent;
}

void check_AFIP_file(int option, char *fileContent, char *object) {
	char *AFIP_file_path = string_new();
	switch (option) {
		case RESTAURANTE:
			string_append_with_format(&AFIP_file_path, "%s%s%s%s", dirInicial, RESTAURANTES_PATH, object, "/Info.AFIP");
			break;
		case RECETA:
			string_append_with_format(&AFIP_file_path, "%s%s%s%s", dirInicial, RECETAS_PATH, object, ".AFIP");
			break;
		case PEDIDO:
			break;
	}
	if (!fdExists(AFIP_file_path)) { // Hace falta revisar esto?
		FILE *fp = fopen(AFIP_file_path, "w+");
		if (fp != NULL) {
			fputs(fileContent, fp);
			fclose(fp);
		}
	}
}

void save_content(int reqBlocks, char *fileContent, uint32_t *bloquesAsignados) {
	int start = 0; //next_block_number;
	char *aux = string_new();
	int content_size = strlen(fileContent);
	int next_block_content_size = content_size < maxContentSize ? content_size : maxContentSize; 

	for (int i = 0; i < reqBlocks; i++) {
		// Obtenemos el tamaño del string a guardar y el substring
		aux = string_substring(fileContent, start, next_block_content_size);
		start += next_block_content_size;
		// Actualizamos el tamaño del bloque siguiente
		int remaining_content = content_size - next_block_content_size;
		next_block_content_size = remaining_content < maxContentSize ? remaining_content : maxContentSize;
		// Creamos la ruta del bloque a llenar
		char *block_to_assign_path = string_new();
		string_append_with_format(&block_to_assign_path, "%s%d.AFIP", blocksPath, bloquesAsignados[i]);
		// Guardamos el contenido en el archivo del bloque correspondiente
		FILE *fp = fopen(block_to_assign_path, "w+");
		if (fp != NULL) {
			fprintf(fp, "%s%d", aux, i+1 >= reqBlocks ? ERROR : bloquesAsignados[i+1]);
			fclose(fp);
		}
	}	
}

void save_in_blocks(int option, char *object, char *fileContent, int bloquesReq) {
	int contentSize = strlen(fileContent);
	// Creamos el array para guardar los números de bloques asignados
	uint32_t bloquesAsignados[bloquesReq];
	asignar_bloques(bloquesAsignados, bloquesReq);
	// Obtenemos el contenido que vamos a grabar en:
	// Restaurante: Info.AFIP
	// Receta: nombreReceta.AFIP
	// Pedido: Pedido#.AFIP
	char *AFIP_file_content = get_AFIP_file_content(contentSize, bloquesAsignados[0]);
	check_AFIP_file(option, AFIP_file_content, object);
	save_content(bloquesReq, fileContent, bloquesAsignados);
}

/* CREAR_RESTAURANTE */

char *get_CrearRestaurante_Data(char **params) {
	char *fileContent = string_new();
	string_append_with_format(&fileContent, "CANTIDAD_COCINEROS=%s\n", params[2]);
	string_append_with_format(&fileContent, "POSICION=%s\n", params[3]);
	string_append_with_format(&fileContent, "AFINIDAD_COCINEROS=%s\n", params[4]);
	string_append_with_format(&fileContent, "PLATOS=%s\n", params[5]);
	string_append_with_format(&fileContent, "PRECIO_PLATOS=%s\n", params[6]);
	string_append_with_format(&fileContent, "CANTIDAD_HORNOS=%s\n", params[7]);
	string_append_with_format(&fileContent, "CANTIDAD_PEDIDOS=%s\n", params[8]);
	log_CrearRestaurante_Data(params);
	return fileContent;
}

void create_rest_dir(char *restaurante) {
	char *newRestPath = string_new();
	string_append_with_format(&newRestPath, "%s%s", RESTAURANTES_PATH, restaurante);
	initFromBaseDir(newRestPath);
}

/* CREAR_RECETA */

char *get_CrearReceta_Data(char **params) {
	char *fileContent = string_new();
	string_append_with_format(&fileContent, "PASOS=%s\n", params[2]);
	string_append_with_format(&fileContent, "TIEMPO_PASOS=%s\n", params[3]);
	log_CrearReceta_Data(params);
	return fileContent;
}

/* Funcionalidades */

void check_and_save(int option, char *object, char *content, int reqBlocks) {
	if (enough_blocks_available(reqBlocks)) {
		save_in_blocks(option, object, content, reqBlocks);
	} else {
		log_full_FS(reqBlocks, get_available_blocks_number());
		exit(EXIT_FAILURE);
	}
}

void crear_restaurante(char **params) {
	char *rest = params[1]; create_rest_dir(rest);
	char *fileContent = get_CrearRestaurante_Data(params);
	int reqBlocks = get_required_blocks_number(strlen(fileContent));
	check_and_save(RESTAURANTE, rest, fileContent, reqBlocks);
}

void crear_receta(char **params) {
	char *plato = params[1];
	char *fileContent = get_CrearReceta_Data(params);
	int reqBlocks = get_required_blocks_number(strlen(fileContent));
	check_and_save(RECETA, plato, fileContent, reqBlocks);
}

void crear_pedido(char **params) {
	char *plato = params[1];
	char *fileContent = get_CrearReceta_Data(params);
	int reqBlocks = get_required_blocks_number(strlen(fileContent));
	check_and_save(PEDIDO, plato, fileContent, reqBlocks);
}