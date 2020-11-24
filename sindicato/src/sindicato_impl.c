#include "../include/sindicato_impl.h"

/* Utils */

int get_required_blocks_number(int contentSize) {
	return (contentSize/maxContentSize) + (contentSize % maxContentSize != 0);
}

bool enough_blocks_available(int bloquesReq) {
	return bloquesReq <= get_available_blocks_number();
}

int find_char_index(char *string, char caracter) {
	char *found_char = strchr(string, caracter);
	if (found_char != NULL) {
		return found_char - string + 1;
	} else {
		return ERROR;
	}
}

/* Utils Restaurantes */

char *get_restaurant_path(char *restaurante) { // Retorna, por ejemplo: /Restaurantes/BK
	char *restPath = string_new();
	string_append_with_format(&restPath, "%s%s", RESTAURANTES_PATH, restaurante);
	return restPath;
}

char *get_full_rest_path(char *restaurante) { // Retorna la ruta absoluta, por ejemplo: /home/utnso/afip/Files/Restaurantes/BK
	char *full_path = string_new();
	string_append_with_format(&full_path, "%s%s", dirInicial, get_restaurant_path(restaurante));
	return full_path;
}

char *get_InfoAFIP_path(char *restaurante) {
	char *InfoAFIP_path = string_new();
	string_append_with_format(&InfoAFIP_path, "%s%s", get_full_rest_path(restaurante), "/Info.AFIP");
	return InfoAFIP_path;
}

/* Utils Pedidos */

char *get_filename_pedido(int idPedido) {
	char *filename = string_new();
	string_append_with_format(&filename, "Pedido%d.AFIP", idPedido);
	return filename;
}

char *get_full_pedido_path(t_request *request) { // Retorna la ruta absoluta, por ejemplo: /home/utnso/afip/Files/Restaurantes/BK/Pedido3.AFIP
	char *full_path = string_new();
	string_append_with_format(&full_path, "%s/Pedido%d.AFIP", get_full_rest_path(request->nombre), request->idPedido);
	return full_path;
}

/* Utils Recetas */

char *get_full_recipe_path(char *receta) { // Retorna la ruta absoluta, por ejemplo: /home/utnso/afip/Files/Recetas/PapasAlHorno.AFIP
	char *full_path = string_new();
	string_append_with_format(&full_path, "%s%s/%s.AFIP", dirInicial, RECETAS_PATH, receta);
	return full_path;
}

/* Existencia */

bool existe_restaurante(char *rest) {
	char *path = get_full_rest_path(rest);
	return fdExists(path);
}

bool existe_receta(char *receta) {
	char *path = get_full_recipe_path(receta);
	return fdExists(path);
}

bool existe_pedido(t_request *request) {
	char *path_pedido = get_full_pedido_path(request);
	return fdExists(path_pedido);	
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

// .AFIP files

char *new_AFIP_file_content(int fullSize, int initialBlock) {
	char *fileContent = string_new();
	string_append_with_format(&fileContent, "SIZE=%d\n", fullSize);
	string_append_with_format(&fileContent, "INITIAL_BLOCK=%d\n", initialBlock);
	return fileContent;
}

void save_empty_AFIP_file(char *path_pedido) {
	char *AFIP_file_content = new_AFIP_file_content(0,ERROR);
	check_AFIP_file(PEDIDO, AFIP_file_content, path_pedido);
}	

void check_AFIP_file(int option, char *fileContent, char *object) {
	char *AFIP_file_path = string_new();
	switch (option) {
		case RESTAURANTE:
			string_append_with_format(&AFIP_file_path, "%s%s%s%s", dirInicial, RESTAURANTES_PATH, object, "/Info.AFIP");
			log_Info_AFIP(object);
			break;
		case RECETA:
			string_append_with_format(&AFIP_file_path, "%s%s%s%s", dirInicial, RECETAS_PATH, object, ".AFIP");
			log_Receta_AFIP(object);
			break;
		case PEDIDO:
			string_append_with_format(&AFIP_file_path, "%s", object);
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

char *get_content_from_AFIP_file(int option, char *object) {	
	char *AFIP_file_path = string_new();
	switch (option) {
		case RESTAURANTE:
			AFIP_file_path = get_InfoAFIP_path(object);
			break;
		case RECETA:
			AFIP_file_path = get_full_recipe_path(object);
			break;
		case PEDIDO:
			break;
	}
	char *file_content = string_new();
	FILE *fp = fopen(AFIP_file_path, "r");
	if (fp != NULL) {
		ssize_t line_size;
		size_t line_buf_size = 0;
		char *current_line = NULL;
		for (int i = 0; i < 2; i++) {
			line_size = getline(&current_line, &line_buf_size, fp); 
			string_append_with_format(&file_content, "%s", current_line);
			log_AFIP_file_line(line_size, line_buf_size, current_line);
		}
		close(fp);
	} else {
		return NULL;
	}
	return file_content;
}

// Métodos y funciones para bloques

void save_content(int reqBlocks, char *fileContent, uint32_t *bloquesAsignados) {
	int start = 0;
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
	uint32_t bloquesAsignados[bloquesReq]; log_blocks_assignment();
	asignar_bloques(bloquesAsignados, bloquesReq);
	/* Obtenemos el contenido que vamos a grabar en:
		Restaurante: Info.AFIP
		Receta: nombreReceta.AFIP
		Pedido: Pedido#.AFIP */
	char *AFIP_file_content = new_AFIP_file_content(contentSize, bloquesAsignados[0]);
	check_AFIP_file(option, AFIP_file_content, object);
	save_content(bloquesReq, fileContent, bloquesAsignados);
}

// Retorna un string con el contenido total de los bloques según lo informado en el archivo .AFIP
char *get_full_blocks_content(int total_size, int fst_block_number) {
	int bytes_checked = 0;
	int total_blocks_number = get_required_blocks_number(total_size); 
	int bloques_a_leer[total_blocks_number]; bloques_a_leer[0] = fst_block_number;
	int curr_block_size = total_size < maxContentSize ? total_size : maxContentSize;

	char *full_content = string_new();

	for (int i = 0; i < total_blocks_number; i++) {
		
		char *curr_block_path = string_new();
		string_append_with_format(&curr_block_path, "%s%d%s", blocksPath, bloques_a_leer[i], ".AFIP");
		
		char *aux = malloc(curr_block_size + 1);

		FILE *fp = fopen(curr_block_path, "r");
		if (fp != NULL) {
			fread(aux, sizeof(char), curr_block_size, fp);
			string_append(&full_content, string_substring_until(aux, curr_block_size));
			bytes_checked += curr_block_size;

			char *next_block = malloc(sizeof(uint32_t) + 1);
			fseek (fp, curr_block_size, SEEK_SET);
			fread(next_block, sizeof(uint32_t), 1, fp);
			bloques_a_leer[i+1] = atoi(string_substring_until(next_block, sizeof(uint32_t)-1));
			
			int remaining_bytes = total_size - bytes_checked;
			curr_block_size = remaining_bytes < maxContentSize ? remaining_bytes : maxContentSize;

			free(curr_block_path);
			free(aux);
			free(next_block);
			fclose(fp);
		}
	}
	return full_content;
}

// Busca la información del archivo .AFIP correspondiente, y retorna el conjunto de información de los bloques en base a ello
char *get_info(int option, char *object) {
	char *afip_content = get_content_from_AFIP_file(option, object);
	if (afip_content != NULL) {
		/* Obtenemos los índices de los caracteres '=' y '\n' de cada línea */
			// Primer línea - Ej.: "SIZE=45\n"
			int fst_equal_i = find_char_index(afip_content, '='); int fst_endl_i = find_char_index(afip_content, '\n');
			// Segunda línea - Ej.: "INITIAL_BLOCK=2\n"
			char *snd_line = string_substring(afip_content, fst_endl_i, strlen(afip_content)-1);
			int snd_equal_i = find_char_index(snd_line, '=');
		/* Parseamos cada string a int */
		int total_size = get_int_file_value(afip_content, fst_equal_i, fst_endl_i);
		int fst_block_number = get_int_file_value(snd_line, snd_equal_i, strlen(snd_line)-2);
		
		char *content = get_full_blocks_content(total_size, fst_block_number);
		log_full_blocks_content(content);
		free(afip_content); free(snd_line);
		return content;
	} else {
		log_no_AFIP_content();
		exit(EXIT_FAILURE);
	}
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
	char *newRestPath = get_restaurant_path(restaurante);
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

/* CREAR_PEDIDO */

char *get_CrearPedido_Data(t_request *request) {
	char *fileContent = string_new();
	// string_append_with_format(&fileContent, "ESTADO_PEDIDO=%s\n", getStringEstadoPedido(PENDIENTE));
	// string_append_with_format(&fileContent, "LISTA_PLATOS=%s\n", "[]");
	// string_append_with_format(&fileContent, "CANTIDAD_PLATOS=%s\n", "[]");
	// string_append_with_format(&fileContent, "CANTIDAD_LISTA=%s\n", "[]");
	// string_append_with_format(&fileContent, "PRECIO_TOTAL=%s\n", 0);
	log_CrearPedido_Data(request);
	return fileContent;
}

void create_pedido_dir(t_request *request) {
	char *new_pedido_path = get_full_pedido_path(request);
	createDirectory(new_pedido_path);
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

void crear_pedido(t_request *request) {
	create_pedido_dir(request);
	save_empty_AFIP_file(get_full_pedido_path(request));
	log_CrearPedido_Data(request);
	//-------------Comentado porque no defini si asignarle bloques al principio o no, por ahora solo creo su archivo afip
	
	//  char *fileContent = get_CrearPedido_Data(request);
	// int reqBlocks = get_required_blocks_number(strlen(fileContent));
	// check_and_save(PEDIDO, pedido, fileContent, reqBlocks);
}