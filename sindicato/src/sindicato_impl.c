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

bool existe_plato_en_pedido(char *plato, t_pedido *pedido) {
	bool es_plato_actual(void *actual) {
		t_plato *plato_pedido = actual;
		return string_equals_ignore_case(plato, plato_pedido->plato);
	};
	return list_any_satisfy(pedido->platos, &es_plato_actual);
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
	string_to_upper(rest);
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

char *get_AFIP_file_path(int option, char *object) {
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
	return AFIP_file_path;
}

// Para editar el archivo AFIP.file o crearlo si no existe
void check_AFIP_file(int option, char *fileContent, char *object) {
	char *AFIP_file_path = get_AFIP_file_path(option, object);
	FILE *fp = fopen(AFIP_file_path, "w+");
	if (fp != NULL) {
		fputs(fileContent, fp);
		fclose(fp);
	}
}

void save_empty_AFIP_file(char *path_pedido) {
	char *AFIP_file_content = new_AFIP_file_content(0,ERROR);
	check_AFIP_file(PEDIDO, AFIP_file_content, path_pedido);
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
			AFIP_file_path = object;
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

// Método para la primer asignación de bloques y el guardado del contenido en los archivos
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

void update_blocks_content() {

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
		
		if (total_size == 0 && fst_block_number == ERROR) {
			return BLOQUES_NO_ASIGNADOS;
		}

		char *content = get_full_blocks_content(total_size, fst_block_number);
		log_full_blocks_content(content);
		free(afip_content); free(snd_line);
		return content;
	} else {
		log_no_AFIP_content();
		exit(EXIT_FAILURE);
	}
}

/* Formateos */

/* Retorna el valor comprendido entre el char '=' y '\n' como int
	Ejemplo:
		line = "SIZE=158\n"
		index_equal = 4
		index_endl = 8
	Retorna el valor comprendido entre ambos -> 158 */
int get_int_file_value(char *line, int index_equal, int index_endl) {
	return atoi(string_substring(line, index_equal, index_endl));
}

char *get_string_file_value(char *line, int index_equal, int index_endl) {
	return string_substring(line, index_equal, index_endl);
}

char *get_plain_line_content(char *content, int line_number) {
	char **lines = string_split(content, "\n");
	char *line = lines[line_number];
	int equal_index = find_char_index(line, '=');
	free(lines);
	string_substring(line, equal_index, strlen(line)-1);//
}

char *get_full_line_content(char *content, int line_number) {
	char **lines = string_split(content, "\n");
	char *line = lines[line_number];
	free(lines);
	return line;	
}

t_list *get_list_from_string(char *info, int line_number, int start_index) {
	t_list *new_list = list_create();
	char **lines = string_split(info, "\n");
	char *line_to_use = lines[line_number];
	char *list = strrchr(line_to_use, '[');
	char *string_list = string_substring(list, 1, strlen(list)-2);
	int values_quantity = getValuesQuantity(string_list);
	char **values = string_split(string_list, ",");
	for (int i = 0; i < values_quantity; i ++) {
		list_add(new_list, values[i]);
	}
	free(lines); free(line_to_use); free(string_list); free(values);
	return new_list;
}

t_list *get_platos_restaurante(char *info_restaurante) {
	return get_list_from_string(info_restaurante, 3, 8);
}

// Retorna una lista de t_instruccion_receta
t_list *get_instrucciones_from_string(char *info) {
	t_list *inst_list = list_create();
	t_list *pasos = get_list_from_string(info, 0, 7);
	t_list *tiempos = get_list_from_string(info, 1, 14);
	for (int i = 0; i < list_size(pasos); i++) {
		char *current_step = list_get(pasos, i);
		char *current_time = list_get(tiempos, i);
		t_instrucciones_receta *current_inst = getTPaso(current_step, atoi(current_time));
		list_add(inst_list, current_inst);
	}
	free(pasos); free(tiempos);
	return inst_list;
}

// Retorna un t_receta
t_receta *get_receta_from_string(char *info, char *receta_a_buscar) {
	t_receta *receta = malloc(sizeof(t_receta));
	receta->plato = receta_a_buscar;
	receta->instrucciones = get_instrucciones_from_string(info);
	return receta;
}

// Retorna un t_posicion
t_posicion *get_posicion_from_string(char *info){
	char *list = string_substring(info, 1, strlen(info)-1);
	char **values = string_split(list, ",");
	t_posicion *posicion = malloc(sizeof(t_posicion));
	posicion->posX = atoi(values[0]);
	posicion->posY = atoi(values[1]);
	free(list); free(values);
	return posicion;
}

// Retorna una lista de t_md_receta a partir de dos listas de strings, una para los platos y otra para sus precios
t_list *get_platos_con_precios_from_rest(char *info_restaurante) {
	t_list *platos_con_precios = list_create();
	t_list *platos = get_list_from_string(info_restaurante, 3, 8);
	t_list *precios = get_list_from_string(info_restaurante, 4, 15);
	for (int i = 0; i < list_size(platos); i++) {
		t_md_receta *plato_actual = malloc(sizeof(t_md_receta));
		plato_actual->plato = list_get(platos, i);
		plato_actual->precio = atoi(list_get(precios, i));
		list_add(platos_con_precios, plato_actual);
	}
	free(platos); free(precios);
	return platos_con_precios;
}

int obtener_precio_plato(char *plato, char *info_restaurante) {
	t_list *menu_restaurante = get_platos_con_precios_from_rest(info_restaurante);
	bool es_plato_actual(void *actual) {
		t_md_receta *plato_menu_actual = actual;
		return string_equals_ignore_case(plato, plato_menu_actual->plato);
	};
	t_md_receta *plato_encontrado = list_find(menu_restaurante, &es_plato_actual);
	int precio_plato = plato_encontrado != NULL ? plato_encontrado->precio : ERROR;
	free(menu_restaurante); if (plato_encontrado != NULL) { free(plato_encontrado); }
	return precio_plato;
}

// Retorna una lista de t_plato a partir de tres listas de strings, una para los platos, otra para sus cant. pedidas y otra para sus cant. listas
t_list *get_t_plato_list_from_lists(t_list *platos, t_list *cant_pedidas, t_list *cant_listas) {
	t_list *t_plato_list = list_create();
	for (int i = 0; i < list_size(platos); i++) {
		t_plato *plato_actual = malloc(sizeof(t_plato));
		plato_actual->plato = list_get(platos, i);
		plato_actual->cantidadPedida = atoi(list_get(cant_pedidas, i));
		plato_actual->cantidadLista = atoi(list_get(cant_listas, i));
		list_add(t_plato_list, plato_actual);
	}
	//free(platos); free(cant_listas); free(cant_pedidas);
	return t_plato_list;	
}

int obtener_precio_pedido(t_list *platos_con_precios, t_list *platos_pedido) {
	int precio_total = 0;
	for (int i = 0; i < list_size(platos_pedido); i++) {

		t_plato *plato_actual = list_get(platos_pedido, i);

		bool es_plato_actual(void *actual) {
			t_md_receta *plato_menu_actual = actual;
			return string_equals_ignore_case(plato_actual->plato, plato_menu_actual->plato);
		};

		t_md_receta *plato_encontrado = list_find(platos_con_precios, &es_plato_actual);

		if (plato_encontrado) { precio_total += plato_encontrado->precio; }

		free(plato_actual); free(plato_encontrado);
	}
	return precio_total;
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

char *get_IniciarPedido_Data(t_req_plato *request, int precio) {
	char *fileContent = string_new();
	string_append_with_format(&fileContent, "ESTADO_PEDIDO=%s\n", getStringEstadoPedido(PENDIENTE));
	string_append_with_format(&fileContent, "LISTA_PLATOS=[%s]\n", request->plato);
	string_append_with_format(&fileContent, "CANTIDAD_PLATOS=[%d]\n", request->cantidadPlato);
	string_append_with_format(&fileContent, "CANTIDAD_LISTA=[%d]\n", 0);
	string_append_with_format(&fileContent, "PRECIO_TOTAL=%d\n", request->cantidadPlato * precio);
	log_CrearPedido_Data(request);
	return fileContent;
}

void create_pedido_dir(t_request *request) {
	char *new_pedido_path = get_full_pedido_path(request);
	createDirectory(new_pedido_path);
}

/* GUARDAR_PLATO */

char *get_GuardarPlato_Data(t_req_plato *request, int precio_plato, bool es_plato_nuevo) {
	t_request *req_pedido = getTRequest(request->idPedido, request->restaurante);
	char *file_content_pedido = get_info(PEDIDO, get_full_pedido_path(req_pedido));
	
	char *updated_file_content = string_new();

	// Obtenemos el contenido del archivo del pedido actualizado con el plato a incrementar o agregar
	char **lines = string_split(file_content_pedido, "\n");
	char *new_lista_platos_line = string_new(); char *new_cant_platos_line = string_new();
	char *new_cant_lista_platos_line = string_new(); char *new_precio_total_line = string_new();
	int precio_total_actual = atoi(get_plain_line_content(file_content_pedido, 4));

	if (es_plato_nuevo) {
		// Agregamos el plato nuevo al final de LISTA_PLATOS
		new_lista_platos_line = string_substring_until(lines[1], strlen(lines[1]) - 1);
		string_append_with_format(&new_lista_platos_line, ",%s]\n", request->plato);
		// Agregamos la cantidad pedida del plato nuevo al final de CANTIDAD_PLATOS
		new_cant_platos_line = string_substring_until(lines[2], strlen(lines[2]) - 1);
		string_append_with_format(&new_cant_platos_line, ",%d]\n", request->cantidadPlato);
		// Agregamos un 0 al final de CANTIDAD_LISTA
		new_cant_lista_platos_line = string_substring_until(lines[3], strlen(lines[3]) - 1);
		string_append_with_format(&new_cant_lista_platos_line, ",%d]\n", 0);
	} else {
		// LISTA_PLATOS queda igual
		string_append_with_format(&new_lista_platos_line, "%s%s", lines[1], "\n");
		// Incrementamos el valor de CANTIDAD_PLATOS para el correspondiente
		new_cant_platos_line = string_substring_until(lines[2], strlen(lines[2]) - 2);
		t_list *platos = get_list_from_string(file_content_pedido, 1, 14);
		t_list *cantidades = get_list_from_string(file_content_pedido, 2, 17);		
		int total_platos = list_size(platos);
		for (int i = 0; i < total_platos; i++) {
			char *plato_actual = list_get(platos, i);
			if (string_equals_ignore_case(plato_actual, request->plato)){ 
				string_append_with_format(&new_cant_platos_line, "%d%s", atoi(list_get(cantidades, i)) + request->cantidadPlato, i+1 == total_platos ? "]\n" : ",");
			} else {
				string_append_with_format(&new_cant_platos_line, "%d%s", atoi(list_get(cantidades, i)), i+1 == total_platos ? "]\n" : ",");
			}
		}
		// CANTIDAD_LISTA queda igual
		string_append_with_format(&new_cant_lista_platos_line, "%s%s", lines[3], "\n");
	}

	// La actualización del precio total es igual en ambos casos
	new_precio_total_line = string_substring_until(lines[4], 13);
	string_append_with_format(&new_precio_total_line, "%d\n", precio_total_actual + precio_plato * request->cantidadPlato);

	// Finalmente, agregamos todas las líneas actualizadas al char* del contenido actualizado
	string_append_with_format(&updated_file_content, "%s%s%s%s%s%s", lines[0], "\n", new_lista_platos_line, new_cant_platos_line, new_cant_lista_platos_line, new_precio_total_line);

	return updated_file_content;
}

/* Funcionalidades */

/* Primeras asignaciones/creaciones */

void check_and_save(int option, char *object, char *content, int reqBlocks) {
	if (enough_blocks_available(reqBlocks)) {
		save_in_blocks(option, object, content, reqBlocks);
	} else {
		free(object); free(content);
		log_full_FS(reqBlocks, get_available_blocks_number());
		exit(EXIT_FAILURE);
	}
}

void guardar_primer_plato(t_req_plato *request, int precio) {
	char *fst_fileContent = get_IniciarPedido_Data(request, precio);
	int reqBlocks = get_required_blocks_number(strlen(fst_fileContent));
	t_request *req_a_guardar = getTRequest(request->idPedido, request->restaurante);
	check_and_save(PEDIDO, get_full_pedido_path(req_a_guardar), fst_fileContent, reqBlocks);
	free(fst_fileContent); free(req_a_guardar);
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
}