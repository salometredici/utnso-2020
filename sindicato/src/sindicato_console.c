#include "../include/sindicato_console.h"

int sindicato_option_to_key(char *key) {
    t_keys *diccionario = diccionarioConsola;
    for (int i = 0; i < CONSOLENKEYS; i++) {
        t_keys sym = diccionario[i];
        if (strcmp(sym.key, key) == 0) {
            return sym.valor;
        }
    }
    return ERROR;
}

// bool only_contains_numbers(char *input) {
// 	for (int i = 0; i < strlen(input); i++) {
// 		if (input[i] < '0' || input[i] > '9') { return false; }
// 	}
// 	return true;
// }

bool only_contains_chars(char *input) {
	for (int i = 0; i < strlen(input); i++) {
		if (input[i] == '0' || input[i] == '1' || input[i] == '2' || input[i] == '3' || input[i] == '4' ||
		    input[i] == '5' || input[i] == '6' || input[i] == '7' || input[i] == '8' || input[i] == '9') { return false; }
	}
	return true;
}

int getValuesQuantity(char *input) {
	int count = 0;
	bool anterior_fue_coma = false;
	for (int i = 0; i < strlen(input); i++) {
		if (anterior_fue_coma) {
			count++;
		}
		anterior_fue_coma = input[i] == ',' ? true : false;
	}
	count++;
	return count;
}

bool only_numbers_list(char *input) {
	int inicio = find_char_index(input, '[');
	int fin = find_char_index(input, ']');
	if (inicio == fin - 1) {
		return true;
	} else {
		char *inputs = string_substring(input, inicio, fin - 2);
		int values_quantity = getValuesQuantity(inputs);
		char **values = string_split(inputs, ",");
		for (int i = 0; i < values_quantity; i++) {
			char *current = values[i];
			if (!only_contains_numbers(current)) { return false;}
		}
	}
	return true;
}

bool only_strings_list(char *input) {
	int inicio = find_char_index(input, '[');
	int fin = find_char_index(input, ']');
	if (inicio == fin - 1) {
		return true;
	} else {
		char *inputs = string_substring(input, inicio, fin - 2);
		int values_quantity = getValuesQuantity(inputs);
		char **values = string_split(inputs, ",");
		for (int i = 0; i < values_quantity; i++) {
			char *current = values[i];
			if (!only_contains_chars(current)) { return false;}
		}
	}
	return true;
}

bool valid_position(char *pos) {
	int values_quantity = getValuesQuantity(pos);
	return values_quantity == 2;
}

bool list_sizes_match(char *list1, char *list2) {
	char *items_l1 = string_substring(list1, 1, strlen(list1) - 2);
	char *items_l2 = string_substring(list2, 1, strlen(list2) - 2);
	int items_quantity_l1 = getValuesQuantity(list1);
	int items_quantity_l2 = getValuesQuantity(list2);
	return items_quantity_l1 == items_quantity_l2;
}

/* Mensajes */

void show_valid_commands() {
    printf("-------------------Comandos V??lidos-------------------"BREAK);
	printf("Ejemplo: AIUDA"BREAK);
	printf("Ejemplo: CLEAR"BREAK);
    printf("-----------------------Opciones-----------------------"BREAK);
	printf("Formato: "BOLDYELLOW"[MENSAJE]"RESET" [PARAMETROS] ~~???_(???o???;)"BREAK);
	printf(YELLOW"CREAR_RESTAURANTE"RESET" [NOMBRE] [CANTI_COCINEROS] [POSICION] [AFINIDAD_COCINEROS] [PLATOS] [PRECIO_PLATOS] [CANT_HORNOS] [CANT_PEDIDOS]"BREAK);
	printf(YELLOW"CREAR_RECETA"RESET" [NOMBRE] [PASOS] [TIEMPO_PASOS]"BREAK);
    printf("------------------------------------------------------"BREAK);
}

void show_command_error_msg(char *msg) {
	printf("-----------------------[ERROR]------------------------"BREAK);
	printf(BOLDRED"Faltan uno o m??s par??metros."RESET BREAK);
	printf("El formato aceptado para el comando "YELLOW"[%s]"RESET" es el siguiente:"BREAK, msg);
	log_error(logger, "Faltan uno o m??s par??metros para el comando [%s]", msg);
}

void show_invalid_position_error_msg() {
	printf("-----------------------[ERROR]------------------------"BREAK);
	printf(BOLDRED"Formato incorrecto para la posici??n del restaurante"RESET BREAK);
	printf("El par??metro "YELLOW"[POSICI??N]"RESET" debe ser una lista de dos n??meros. Ejemplo: [3,4]"BREAK);
	log_error(logger, "El formato para la posici??n del restaurante era incorrecto");
}

// void show_NaN_error_msg() {
// 	printf("-----------------------[ERROR]------------------------"BREAK);
// 	printf(BOLDRED"Se ingres?? un valor no num??rico en alg??n/algunos par??metro/s"RESET BREAK);
// 	printf("Los par??metros "YELLOW"[CANT_COCINEROS]"RESET", "YELLOW"[CANT_HORNOS]"RESET", y "YELLOW"[CANT_PEDIDOS]"RESET" deben ser n??meros"BREAK);
// 	log_error(logger, "Se ingres?? un valor no num??rico en alg??n/algunos par??metro/s");
// }

void show_not_a_list_error_msg(int option) {
	printf("-----------------------[ERROR]------------------------"BREAK);
	printf(BOLDRED"Se ingres?? un valor que "RESET BOLD"NO"RESET BOLDRED" era una lista en alg??n/algunos par??metro/s"RESET BREAK);
	switch (option) {
		case OPT_CREAR_RESTAURANTE:
			printf("Los siguientes par??metros "BOLD"deben"RESET" ser listas de elementos, por ejemplo: [1,2] o [PapasAlHorno,Milanesas]"BREAK);
			printf(YELLOW"[POSICION]"RESET" - Debe ser un par de n??meros. Ejemplo: [1,2]"BREAK);
			printf(YELLOW"[AFINIDADES]"RESET" - Debe ser una vac??a o una lista de strings. Ejemplo: [] o [PapasAlHorno]"BREAK);
			printf(YELLOW"[PLATOS]"RESET" - Debe ser una lista de strings. Ejemplo: [Lasagna,PolloConPapas]"BREAK);
			printf(YELLOW"[PRECIOS]"RESET" - Debe ser una lista de n??meros, del mismo tama??o que [PLATOS]. Ejemplo: Para [Lasagna,Papas,Milanesa] corresponder??a [110,55,70]"BREAK);
			break;
		case OPT_CREAR_RECETA:
			printf("Los siguientes par??metros "BOLD"deben"RESET" ser listas de elementos, por ejemplo: [1,2] o [PapasAlHorno,Milanesas]"BREAK);
			printf(YELLOW"[PASOS]"RESET" - Debe ser una lista de strings. Ejemplo: [Trocear,Reposar,Hornear]"BREAK);
			printf(YELLOW"[TIEMPO_PASOS]"RESET" - Debe ser una lista de n??meros, de la misma longitud que la lista de pasos. Ejemplo: [3,4,5]"BREAK);
			break;
	}
	log_error(logger, "Se ingres?? un valor que NO era una lista en alg??n/algunos par??metro/s");
}

void show_not_a_numbers_list_error_msg(int option) {
	printf("-----------------------[ERROR]------------------------"BREAK);
	printf(BOLDRED"Se ingres?? un valor que "RESET BOLD"NO"RESET BOLDRED" era una lista de "RESET BOLD"N??MEROS"RESET BOLDRED" en alg??n/algunos par??metro/s"RESET BREAK);
	switch (option) {
		case OPT_CREAR_RESTAURANTE:
			printf("Los par??metros "YELLOW"[POSICION]"RESET" y "YELLOW"[PRECIOS]"RESET" deben ser listas de n??meros"BREAK);
			break;
		case OPT_CREAR_RECETA:
			printf("El par??metro "YELLOW"[TIEMPO_PASOS]"RESET" debe ser una lista de n??meros"BREAK);
			break;
	}
	log_error(logger, "Se ingres?? un valor que NO era una lista de N??MEROS en alg??n/algunos par??metro/s");
}

void show_not_a_strings_list_error_msg(int option) {
	printf("-----------------------[ERROR]------------------------"BREAK);
	printf(BOLDRED"Se ingres?? un valor que "RESET BOLD"NO"RESET BOLDRED" era una lista de "RESET BOLD"STRINGS"RESET BOLDRED" alg??n/algunos par??metro/s"RESET BREAK);
	switch (option) {
		case OPT_CREAR_RESTAURANTE:
			printf("Los par??metros "YELLOW"[AFINIDADES]"RESET" y "YELLOW"[PLATOS]"RESET" deben ser listas de strings"BREAK);
			break;
		case OPT_CREAR_RECETA:
			printf("El par??metro "YELLOW"[PASOS]"RESET" debe ser una lista de strings"BREAK);
			break;
	}
	log_error(logger, "Se ingres?? un valor que NO era una lista de STRINGS en alg??n/algunos par??metro/s");
}

void show_incorrect_list_sizes_error_msg(int option) {
	printf("-----------------------[ERROR]------------------------"BREAK);
	printf(BOLDRED"La cantidad de par??metros en las listas no coincide"RESET BREAK);
	switch (option) {
		case OPT_CREAR_RESTAURANTE:
			printf("Los par??metros "YELLOW"[PLATOS]"RESET" y "YELLOW"[PRECIOS]"RESET" deben ser listas con la misma cantidad de valores"BREAK);
			break;
		case OPT_CREAR_RECETA:
			printf("Los par??metros "YELLOW"[PASOS]"RESET" y "YELLOW"[TIEMPO_PASOS]"RESET" deben ser listas con la misma cantidad de valores"BREAK);
			break;
	}
	log_error(logger, "Las listas %s no ten??an la misma cantidad de valores", option == OPT_CREAR_RECETA ? "[PASOS] y [TIEMPO_PASOS]" : "[PLATOS] y [PRECIOS]");
}

int validate_console_command(char *msg, char **parameters) {
	int option = sindicato_option_to_key(msg);
	switch (option) {
		case OPT_CREAR_RESTAURANTE:
			if (!parameters[1] || !parameters[2] || !parameters[3] || !parameters[4] ||
				!parameters[5] || !parameters[6] || !parameters[7] || !parameters[8]) {
				show_command_error_msg(msg);
				printf(BOLDYELLOW"CREAR_RESTAURANTE [NOMBRE] [CANT_COCINEROS] [POSICION] [AFINIDADES] [PLATOS] [PRECIOS] [CANT_HORNOS] [CANT_PEDIDOS]"RESET BREAK);
				printf("[!] Ejemplo: "BOLDCYAN"CREAR_RESTAURANTE BurguerKing 3 [1,4] [PapasAlHorno] [PapasAlHorno,Milanesas] [32,50] 2 1"RESET BREAK);
				printf(TAB "[!] "BOLD"[NOMBRE]: char*"RESET BREAK);
				printf(TAB "[!] "BOLD"[CANT_COCINEROS]: int"RESET BREAK);
				printf(TAB "[!] "BOLD"[POSICION]: [int,int]"RESET BREAK);
				printf(TAB "[!] "BOLD"[AFINIDADES]: char*"RESET BREAK);
				printf(TAB "[!] "BOLD"[PLATOS]: char*"RESET BREAK);
				printf(TAB "[!] "BOLD"[PRECIOS]: int*"RESET BREAK);
				printf(TAB "[!] "BOLD"[CANT_HORNOS]: int"RESET BREAK);
				printf(TAB "[!] "BOLD"[CANT_PEDIDOS]: int"RESET BREAK);
				return ERROR;
			}
			if (!only_contains_numbers(parameters[2]) || !only_contains_numbers(parameters[7]) || !only_contains_numbers(parameters[8])) {
				show_NaN_error_msg();
				return ERROR;
			}
			if (!string_starts_with(parameters[3], "[") || !string_starts_with(parameters[4], "[") ||
			    !string_starts_with(parameters[5], "[") || !string_starts_with(parameters[6], "[")) {
				show_not_a_list_error_msg(OPT_CREAR_RESTAURANTE);
				return ERROR;
			}
			if (!only_numbers_list(parameters[3]) || !only_numbers_list(parameters[6])) {
				show_not_a_numbers_list_error_msg(OPT_CREAR_RESTAURANTE);
				return ERROR;
			}
			if (!valid_position(parameters[3])) {
				show_invalid_position_error_msg();
				return ERROR;
			}
			if (!only_strings_list(parameters[4]) || !only_strings_list(parameters[5])) {
				show_not_a_strings_list_error_msg(OPT_CREAR_RESTAURANTE);
				return ERROR;
			}
			if (!list_sizes_match(parameters[5], parameters[6])) {
				show_incorrect_list_sizes_error_msg(OPT_CREAR_RESTAURANTE);
				return ERROR;
			}
			break;
		case OPT_CREAR_RECETA:
			if (!parameters[1] || !parameters[2] || !parameters[3]) {
				show_command_error_msg(msg);
				printf(BOLDYELLOW "CREAR_RECETA [NOMBRE_RECETA] [PASOS] [TIEMPO_PASOS]"RESET BREAK);
				printf("[!] Ejemplo: "BOLDCYAN"CREAR_RECETA PapasAlHorno [Trocear,Hornear] [2,3]"RESET BREAK);
				printf(TAB "[!] " BOLD "[NOMBRE_RECETA]: char*"RESET BREAK);
				printf(TAB "[!] " BOLD "[PASOS]: char*"RESET BREAK);
				printf(TAB "[!] " BOLD "[TIEMPO_PASOS]: int*"RESET BREAK);
				return ERROR;
			}
			if (!string_starts_with(parameters[2], "[") || !string_starts_with(parameters[3], "[")) {
				show_not_a_list_error_msg(OPT_CREAR_RECETA);
				return ERROR;
			}
			if (!only_numbers_list(parameters[3])) {
				show_not_a_numbers_list_error_msg(OPT_CREAR_RECETA);
				return ERROR;
			}
			if (!only_strings_list(parameters[2])) {
				show_not_a_strings_list_error_msg(OPT_CREAR_RECETA);
				return ERROR;
			}
			if (!list_sizes_match(parameters[2], parameters[3])) {
				show_incorrect_list_sizes_error_msg(OPT_CREAR_RECETA);
				return ERROR;
			}
			break;
		default:
			return option;
	}
	return option;
}

void show_invalid_command_msg() {
	printf("------------------------------------------------------"BREAK);
	printf("Comando no v??lido. Escriba 'AIUDA' para ver los formatos aceptados ???_???"BREAK);
	printf("------------------------------------------------------"BREAK);
}