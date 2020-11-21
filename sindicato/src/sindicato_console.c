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

void show_valid_commands() {
    printf("-------------------Comandos Válidos-------------------"BREAK);
	printf("Ejemplo: AIUDA"BREAK);
	printf("Ejemplo: CLEAR"BREAK);
	printf("Ejemplo: BAI"BREAK);
    printf("-----------------------Opciones-----------------------"BREAK);
	printf("Formato: "BOLDYELLOW"[MENSAJE]"RESET" [PARAMETROS] ~~旦_(･o･;)"BREAK);
	printf(YELLOW"CREAR_RESTAURANTE"RESET" [NOMBRE] [CANTIDAD_COCINEROS] [POSICION] [AFINIDAD_COCINEROS] [PLATOS] [PRECIO_PLATOS] [CANTIDAD_HORNOS]"BREAK);
	printf(YELLOW"CREAR_RECETA"RESET" [NOMBRE] [PASOS] [TIEMPO_PASOS]"BREAK);
    printf("------------------------------------------------------"BREAK);
}

void show_command_error_msg(char *msg) {
	printf("-----------------------[ERROR]------------------------"BREAK);
	printf(BOLDRED"Faltan uno o más parámetros."RESET BREAK);
	printf("El formato aceptado para el comando "YELLOW"[%s]"RESET" es el siguiente:"BREAK, msg);
	log_error(logger, "Faltan uno o más parámetros para el comando [%s]", msg);
}


int validate_console_command(char *msg, char **parameters) {
	int option = sindicato_option_to_key(msg);
	switch (option) {
		case OPT_CREAR_RESTAURANTE:
			if (!parameters[1] || !parameters[2] || !parameters[3] || !parameters[4] || !parameters[5] ||
			    !parameters[6] || !parameters[7] || !parameters[8]) {
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
			break;
		default:
			return option;
	}
	return option;
}

void show_invalid_command_msg() {
	printf("------------------------------------------------------"BREAK);
	printf("Comando no válido. Escriba 'AIUDA' para ver los formatos aceptados ◔_◔"BREAK);
	printf("------------------------------------------------------"BREAK);
}