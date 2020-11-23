#include "../include/sindicato_logic.h"

/* Retorna el valor comprendido entre el char '=' y '\n' como int
	Ejemplo:
		line = "SIZE=158\n"
		index_equal = 4
		index_endl = 8
	Retorna el valor comprendido entre ambos -> 158 */
int get_int_file_value(char *line, int index_equal, int index_endl) {
	return atoi(string_substring(line, index_equal, index_endl));
}

t_list *obtener_platos_restaurante(char *restaurante) {
	t_list *platos_restaurante = list_create();
	char *content = get_info(RESTAURANTE, restaurante);
	printf("%s",content);
	return platos_restaurante;
}

// crear_restaurante bk 1 [1,2] [hamburguesas] [hamburguesas,papas] [135,78] 1 0