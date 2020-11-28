#include "../include/client_console.h"

int clientOptionToKey(char *key) {
    t_keys *diccionario = diccionarioOpciones;
    for (int i = 0; i < CONSOLENKEYS; i++) {
        t_keys sym = diccionario[i];
        if (strcmp(sym.key, key) == 0) {
            return sym.valor;
        }
    }
    return ERROR;
}

void mostrarComandosValidos(char *modulo) {
	void mensajesValidos() {
		printf("Ejemplo: AIUDA"BREAK);
		printf("Ejemplo: CLEAR"BREAK);
		printf("Ejemplo: BAI"BREAK);
		printf("Para obtener los comandos válidos por módulo, ingresar: AIUDA [MODULO]"BREAK);
	}

    printf("-------------------Comandos Válidos-------------------"BREAK);

	if (modulo) {
		int proceso = clientOptionToKey(modulo);
		printf("Formato: "BOLDYELLOW"[MENSAJE]"RESET" [PARAMETROS] ~~旦_(･o･;)"BREAK);
		switch (proceso) {
			case OPTION_APP:
				printf(YELLOW"CONSULTAR_RESTAURANTES"RESET BREAK);
				printf(YELLOW"SELECCIONAR_RESTAURANTE"RESET" [NOMBRE_RESTAURANTE]"BREAK);
				printf(YELLOW"CONSULTAR_PLATOS"RESET" [NOMBRE_RESTAURANTE]"BREAK);
				printf(YELLOW"CREAR_PEDIDO"RESET BREAK);
				printf(YELLOW"ANIADIR_PLATO"RESET" [NOMBRE_PLATO] [ID_PEDIDO]"BREAK);
				printf(YELLOW"CONFIRMAR_PEDIDO"RESET" [ID_PEDIDO]"BREAK);
				printf(YELLOW"PLATO_LISTO"RESET" [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_COMIDA]"BREAK);
				printf(YELLOW"CONSULTAR_PEDIDO"RESET" [ID_PEDIDO]"BREAK);
				break;
			case OPTION_COMANDA:
				printf(YELLOW"GUARDAR_PEDIDO"RESET" [NOMBRE_RESTAURANTE] [ID_PEDIDO]"BREAK);
				printf(YELLOW"GUARDAR_PLATO"RESET" [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_COMIDA] [CANTIDAD]"BREAK);
				printf(YELLOW"CONFIRMAR_PEDIDO"RESET" [ID_PEDIDO] [NOMBRE_RESTAURANTE]"BREAK);
				printf(YELLOW"PLATO_LISTO"RESET" [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_COMIDA]"BREAK);
				printf(YELLOW"OBTENER_PEDIDO"RESET" [NOMBRE_RESTAURANTE] [ID_PEDIDO]"BREAK);
				break;
			case OPTION_RESTAURANTE:
				printf(YELLOW"CONSULTAR_PLATOS"RESET" [NOMBRE_RESTAURANTE]"BREAK);
				printf(YELLOW"CREAR_PEDIDO"RESET BREAK);
				printf(YELLOW"ANIADIR_PLATO"RESET" [NOMBRE_PLATO] [ID_PEDIDO]"BREAK);
				printf(YELLOW"CONFIRMAR PEDIDO"RESET" [ID_PEDIDO]"BREAK);
				printf(YELLOW"CONSULTAR_PEDIDO"RESET" [ID_PEDIDO]"BREAK);
				break;
			case OPTION_SINDICATO:
				printf(YELLOW"OBTENER_RESTAURANTE"RESET" [NOMBRE_RESTAURANTE]"BREAK);
				printf(YELLOW"CONSULTAR_PLATOS"RESET" [NOMBRE_RESTAURANTE]"BREAK);
				printf(YELLOW"GUARDAR_PEDIDO"RESET" [NOMBRE_RESTAURANTE] [ID_PEDIDO]"BREAK);
				printf(YELLOW"GUARDAR_PLATO"RESET" [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_COMIDA] [CANTIDAD]"BREAK);
				printf(YELLOW"CONFIRMAR_PEDIDO"RESET" [ID_PEDIDO] [NOMBRE_RESTAURANTE]"BREAK);
				printf(YELLOW"PLATO_LISTO"RESET" [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_COMIDA]"BREAK);
				printf(YELLOW"OBTENER_PEDIDO"RESET" [NOMBRE_RESTAURANTE] [ID_PEDIDO]"BREAK);
				printf(YELLOW"TERMINAR_PEDIDO"RESET" [ID_PEDIDO] [NOMBRE_RESTAURANTE]"BREAK);
				break;
			default:
				mensajesValidos();
				break;
		}
	} else {
		mensajesValidos();                           
	}
	
	printf("------------------------------------------------------"BREAK);
}

void showCommandErrorMsg(int command) {
	printf("-----------------------[ERROR]------------------------"BREAK);
	printf(BOLDRED"Faltan uno o más parámetros."RESET BREAK);
	printf("El formato aceptado para el comando "YELLOW"[%s]"RESET" es el siguiente:"BREAK, getStringKeyValue(command, COMMANDNKEYS));
	log_error(logger, "Faltan uno o más parámetros para el comando [%s]", getStringKeyValue(command, COMMANDNKEYS));
}

void showInvalidMsg() {
	printf("------------------------------------------------------"BREAK);
	printf("Comando no válido. Escriba 'AIUDA' para ver el formato aceptado つ´Д`)つ"BREAK);
	printf("------------------------------------------------------"BREAK);
}

void showInvalidCommandMsg(int option) {
	printf("------------------------------------------------------"BREAK);
	printf("El mensaje ingresado no es válido para el módulo %s (・ε・`*)...!"BREAK, getStringKeyValue(option, PROCNKEYS));
	printf("------------------------------------------------------"BREAK);
}

int validate_obtener_pedido(char** parameters){
	if (!parameters[1] || !parameters[2]) {
		showCommandErrorMsg(OBTENER_PEDIDO);
		printf(BOLDYELLOW"OBTENER_PEDIDO [NOMBRE_RESTAURANTE] [ID_PEDIDO]"RESET BREAK);
		printf(TAB"[!] "BOLD"[NOMBRE_RESTAURANTE]: char*"RESET BREAK);
		printf(TAB"[!] "BOLD"[ID_PEDIDO]: int"RESET BREAK);
		return ERROR;
	}

	if (!only_contains_numbers(parameters[2])) {
		show_NaN_error_msg();
		return ERROR;
	}
	return 0;
}

int validate_plato_listo(char** parameters){
	if (!parameters[1] || !parameters[2] || !parameters[3]) {
		showCommandErrorMsg(PLATO_LISTO);
		printf(BOLDYELLOW"PLATO_LISTO [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_PLATO]"RESET BREAK);
		printf(TAB"[!] "BOLD"[NOMBRE_RESTAURANTE]: char*"RESET BREAK);
		printf(TAB"[!] "BOLD"[ID_PEDIDO]: int"RESET BREAK);
		printf(TAB"[!] "BOLD"[NOMBRE_PLATO]: char*"RESET BREAK);
		return ERROR;
	}

	if (!only_contains_numbers(parameters[2])) {
		show_NaN_error_msg();
		return ERROR;
	}
	return 0;	
}

int validate_confirmar_pedido(char** parameters){
	if (!parameters[1] || !parameters[2]) {
		showCommandErrorMsg(CONFIRMAR_PEDIDO);
		printf(BOLDYELLOW"CONFIRMAR_PEDIDO [NOMBRE_RESTAURANTE] [ID_PEDIDO]"RESET BREAK);
		printf(TAB"[!] "BOLD"[ID_PEDIDO]: int"RESET BREAK);
		printf(TAB"[!] "BOLD"[NOMBRE_RESTAURANTE]: char*"RESET BREAK);
		return ERROR;
	}

	if (!only_contains_numbers(parameters[1])) {
		show_NaN_error_msg();
		return ERROR;
	}
	return 0;
}

int validate_terminar_pedido(char** parameters){
	if (!parameters[1] || !parameters[2]) {
		showCommandErrorMsg(TERMINAR_PEDIDO);
		printf(BOLDYELLOW"TERMINAR_PEDIDO [ID_PEDIDO] [NOMBRE_RESTAURANTE]"RESET BREAK);
		printf(TAB"[!] "BOLD"[ID_PEDIDO]: int"RESET BREAK);
		printf(TAB"[!] "BOLD"[NOMBRE_RESTAURANTE]: char*"RESET BREAK);
		return ERROR;
	}

	if (!only_contains_numbers(parameters[1])) {
		show_NaN_error_msg();
		return ERROR;
	}
	return 0;
}

int validate_confirmar_pedido_v2(char** parameters){
	if (!parameters[1]) {
		showCommandErrorMsg(CONFIRMAR_PEDIDO);
		printf(BOLDYELLOW"CONFIRMAR_PEDIDO [ID_PEDIDO]"RESET BREAK);
		printf(TAB"[!] "BOLD"[ID_PEDIDO]: int"RESET BREAK);
		return ERROR;
	}
	
	if (!only_contains_numbers(parameters[1])) {
		show_NaN_error_msg();
		return ERROR;
	}
	return 0;
}

int validate_guardar_plato(char** parameters){
	if (!parameters[1] || !parameters[2] || !parameters[3] || !parameters[4]) {
		showCommandErrorMsg(GUARDAR_PLATO);
		printf(BOLDYELLOW"GUARDAR_PLATO [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_PLATO] [CANTIDAD]"RESET BREAK);
		printf(TAB"[!] "BOLD"[NOMBRE_RESTAURANTE]: char*"RESET BREAK);
		printf(TAB"[!] "BOLD"[ID_PEDIDO]: int"RESET BREAK);
		printf(TAB"[!] "BOLD"[NOMBRE_PLATO]: char*"RESET BREAK);
		printf(TAB"[!] "BOLD"[CANTIDAD]: int"RESET BREAK);
		return ERROR;	
	}

	if (!only_contains_numbers(parameters[2]) || !only_contains_numbers(parameters[4])) {
		show_NaN_error_msg();
		return ERROR;
	}
	return 0;
}

int validate_guardar_pedido(char** parameters){
	if (!parameters[1] || !parameters[2]) {
		showCommandErrorMsg(GUARDAR_PEDIDO);
		printf(BOLDYELLOW"GUARDAR_PEDIDO [NOMBRE_RESTAURANTE] [ID_PEDIDO]"RESET BREAK);
		printf(TAB"[!] "BOLD"[NOMBRE_RESTAURANTE]: char*"RESET BREAK);
		printf(TAB"[!] "BOLD"[ID_PEDIDO]: int"RESET BREAK);
		return ERROR;	
	}

	if (!only_contains_numbers(parameters[2])) {
		show_NaN_error_msg();
		return ERROR;
	}
	return 0;
}

int validate_consultar_pedido(char** parameters){
	if (!parameters[1]) {
		showCommandErrorMsg(CONSULTAR_PEDIDO);
		printf(BOLDYELLOW"CONSULTAR_PEDIDO [ID_PEDIDO]"RESET BREAK);
		printf(TAB"[!] "BOLD"[ID_PEDIDO]: int"RESET BREAK);
		return ERROR;
	}

	if (!only_contains_numbers(parameters[1])) {
		show_NaN_error_msg();
		return ERROR;
	}
	return 0;
}

int validate_aniadir_plato(char** parameters){
	if (!parameters[1] || !parameters[2]) {
		showCommandErrorMsg(ANIADIR_PLATO);
		printf(BOLDYELLOW"ANIADIR_PLATO [NOMBRE_PLATO] [ID_PEDIDO]"RESET BREAK);
		printf(TAB"[!] "BOLD"[NOMBRE_PLATO]: char*"RESET BREAK);
		printf(TAB"[!] "BOLD"[ID_PEDIDO]: int"RESET BREAK);
		return ERROR;
	}

	if (!only_contains_numbers(parameters[2])) {
		show_NaN_error_msg();
		return ERROR;
	}
	return 0;
}

int validateCommand(int option, int command, char **parameters) {
	int res;
	switch (option) {
		case OPTION_APP:
			switch (command) {
				case SELECCIONAR_RESTAURANTE:
					if (!parameters[1]) {
						showCommandErrorMsg(command);
						printf(BOLDYELLOW"SELECCIONAR_RESTAURANTE [NOMBRE_RESTAURANTE]"RESET BREAK);
						printf(TAB"[!] "BOLD"[NOMBRE_RESTAURANTE]: char*"RESET BREAK);
						return ERROR;
					}
					break;
				case ANIADIR_PLATO:;
					res = validate_aniadir_plato(parameters);

					if(res == ERROR)
						return ERROR;
					break;
				case CONFIRMAR_PEDIDO:;
					res = validate_confirmar_pedido_v2(parameters);

					if(res == ERROR)
						return ERROR;
					break;
				case PLATO_LISTO:;
					res = validate_plato_listo(parameters);

					if(res == ERROR)
						return ERROR;
					break;
				case CONSULTAR_PEDIDO:;
					res = validate_consultar_pedido(parameters);
					
					if(res == ERROR)
						return ERROR;
					break;
			}
			break;
		case OPTION_COMANDA:
			switch (command) {
				case GUARDAR_PEDIDO:;
					res = validate_guardar_pedido(parameters);
					
					if(res == ERROR)
						return ERROR;
					break;
				case GUARDAR_PLATO:;
					res = validate_guardar_plato(parameters);
					
					if(res == ERROR)
						return ERROR;
					break;
				case CONFIRMAR_PEDIDO:;
					res = validate_confirmar_pedido(parameters);					
					
					if(res == ERROR)
						return ERROR;
					break;
				case PLATO_LISTO:;
					res = validate_plato_listo(parameters);
					
					if(res == ERROR)
						return ERROR;
					break;
				case OBTENER_PEDIDO:;
					res = validate_obtener_pedido(parameters);
					
					if(res == ERROR)
						return ERROR;
					break;
			}
			break;
		case OPTION_RESTAURANTE:
			switch (command) {
				case ANIADIR_PLATO:;
					res = validate_aniadir_plato(parameters);
					if(res == ERROR)
						return ERROR;
					break;
				case CONFIRMAR_PEDIDO:;
					res = validate_confirmar_pedido_v2(parameters);
					
					if(res == ERROR)
						return ERROR;
					break;
				case CONSULTAR_PEDIDO:;
					res = validate_consultar_pedido(parameters);
					
					if(res == ERROR)
						return ERROR;
					break;
			}
			break;
		case OPTION_SINDICATO:
			switch (command) {
				case OBTENER_RESTAURANTE:
					if (!parameters[1]) {
						showCommandErrorMsg(command);
						printf(BOLDYELLOW"OBTENER_RESTAURANTE [NOMBRE_RESTAURANTE]"RESET BREAK);
						printf(TAB"[!] "BOLD"[NOMBRE_RESTAURANTE]: char*"RESET BREAK);
						return ERROR;	
					}
					break;
				case GUARDAR_PEDIDO:;
					res = validate_guardar_pedido(parameters);
					
					if(res == ERROR)
						return ERROR;
					break;
				case GUARDAR_PLATO:;
					res = validate_guardar_plato(parameters);

					if(res == ERROR)
						return ERROR;
					break;
				case CONFIRMAR_PEDIDO:;
					res = validate_confirmar_pedido(parameters);

					if(res == ERROR)
						return ERROR;					
					break;
				case PLATO_LISTO:;					
					res = validate_plato_listo(parameters);
					
					if(res == ERROR)
						return ERROR;
					break;
				case OBTENER_PEDIDO:
					validate_obtener_pedido(parameters);
					break;
				case TERMINAR_PEDIDO:
					res = validate_terminar_pedido(parameters);

					if(res == ERROR)
						return ERROR;
					break;

			}
			break;
		default:
			return command;
	}
	return command;
}

