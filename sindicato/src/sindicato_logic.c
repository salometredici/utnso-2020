#include "../include/sindicato_logic.h"

t_list *obtener_platos_restaurante(char *restaurante) {
	t_list *platos_restaurante = list_create();

	// Obtener contenido de info.afip
	char *file_content = get_content_from_AFIP_file(RESTAURANTE, restaurante);

	if (file_content != NULL) {
		int fst_equal_i = find_char_index(file_content, '='); int fst_endl_i = find_char_index(file_content, '\n');
		char *snd_line = string_substring(file_content, fst_endl_i, strlen(file_content)-1);
		int snd_equal_i = find_char_index(snd_line, '=');

		int totalSize = atoi(string_substring(file_content, fst_equal_i, fst_endl_i - 1));
		int fst_block_number = atoi(string_substring(file_content, snd_equal_i, strlen(file_content)-2)); 

	}

	return platos_restaurante;
}


// crear_restaurante bk 1 [1,2] [hamburguesas] [hamburguesas,papas] [135,78] 1 0