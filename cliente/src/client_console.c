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
		printf("Ejemplo: AIUDA\n");
		printf("Ejemplo: CLEAR\n");
		printf("Ejemplo: BAI\n");
		printf("Para obtener los comandos válidos por módulo, ingresar: AIUDA [MODULO]\n");
	}

    printf("-------------------Comandos Válidos-------------------\n");

	if (modulo) {
		int proceso = clientOptionToKey(modulo);
		printf("Formato: [MODULO] [MENSAJE] [PARAMETROS] ~~旦_(･o･;)\n");
		switch (proceso) {
			case OPTION_APP:
				printf("APP CONSULTAR_RESTAURANTES\n");
				printf("APP SELECCIONAR_RESTAURANTE [NOMBRE_CLIENTE] [NOMBRE_RESTAURANTE]\n");
				printf("APP CONSULTAR_PLATOS [NOMBRE_RESTAURANTE]\n");
				printf("APP CREAR_PEDIDO\n");
				printf("APP ANIADIR_PLATO [NOMBRE_PLATO] [ID_PEDIDO]\n");
				printf("APP CONFIRMAR_PEDIDO [ID_PEDIDO]\n");
				printf("APP PLATO_LISTO [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_COMIDA]\n");
				printf("APP CONSULTAR_PEDIDO [ID_PEDIDO]\n");
				break;
			case OPTION_COMANDA:
				printf("COMANDA GUARDAR_PEDIDO [NOMBRE_RESTAURANTE] [ID_PEDIDO]\n");
				printf("COMANDA GUARDAR_PLATO [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_COMIDA] [CANTIDAD]\n");
				printf("COMANDA CONFIRMAR_PEDIDO [ID_PEDIDO] [NOMBRE_RESTAURANTE]\n");
				printf("COMANDA PLATO_LISTO [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_COMIDA]\n");
				printf("COMANDA OBTENER_PEDIDO [NOMBRE_RESTAURANTE] [ID_PEDIDO]\n");
				break;
			case OPTION_RESTAURANTE:
				printf("RESTAURANTE CONSULTAR_PLATOS [NOMBRE_RESTAURANTE]\n");
				printf("RESTAURANTE CREAR_PEDIDO\n");
				printf("RESTAURANTE ANIADIR_PLATO [NOMBRE_PLATO] [ID_PEDIDO]\n");
				printf("RESTAURANTE CONFIRMAR PEDIDO [ID_PEDIDO] \n");
				printf("RESTAURANTE CONSULTAR_PEDIDO [ID_PEDIDO]\n");
				break;
			case OPTION_SINDICATO:
				printf("SINDICATO OBTENER_RESTAURANTE [NOMBRE_RESTAURANTE]\n");
				printf("SINDICATO CONSULTAR_PLATOS [NOMBRE_RESTAURANTE]\n");
				printf("SINDICATO GUARDAR_PEDIDO [NOMBRE_RESTAURANTE] [ID_PEDIDO]\n");
				printf("SINDICATO GUARDAR_PLATO [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_COMIDA] [CANTIDAD]\n");
				printf("SINDICATO CONFIRMAR_PEDIDO [ID_PEDIDO] [NOMBRE_RESTAURANTE]\n");
				printf("SINDICATO PLATO_LISTO [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_COMIDA]\n");
				printf("SINDICATO OBTENER_PEDIDO [NOMBRE_RESTAURANTE] [ID_PEDIDO]\n");
				break;
			default:
				mensajesValidos();
				break;
		}
	} else {
		mensajesValidos();                           
	}
	
	printf("------------------------------------------------------\n");
}

void showCommandErrorMsg(int command) {
	printf("-----------------------[ERROR]------------------------\n");
	printf("\033[1;31mFaltan uno o más parámetros.\033[0m\n");
	printf("El formato aceptado para el comando \033[0;33m[%s]\033[0m es el siguiente:\n", getStringKeyValue(command, COMMANDNKEYS));
	log_error(logger, "Faltan uno o más parámetros para el comando [%s]", getStringKeyValue(command, COMMANDNKEYS));
}

void showInvalidMsg() {
	printf("Comando no válido. Escriba 'AIUDA' para ver el formato aceptado つ´Д`)つ\n");
	printf("------------------------------------------------------\n");
}

void showInvalidCommandMsg(int option) {
	printf("El mensaje ingresado no es válido para el módulo %s (・ε・`*)...!\n", getStringKeyValue(option, PROCNKEYS));
	printf("------------------------------------------------------\n");
}

int validateCommand(int option, int command, char **parameters) {
	switch (option) {
		case OPTION_APP:
			switch (command) {
				case SELECCIONAR_RESTAURANTE:
					if (!parameters[2] || !parameters[3]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mAPP SELECCIONAR_RESTAURANTE [CLIENTE] [NOMBRE_RESTAURANTE]\033[0m\n");
						printf("\t[!] \033[1m[CLIENTE]: int\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_RESTAURANTE]: char*\033[0m\n");
						return ERROR;
					}
					break;
				case ANIADIR_PLATO:
					if (!parameters[2] || !parameters[3]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mAPP ANIADIR_PLATO [NOMBRE_PLATO] [ID_PEDIDO]\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_PLATO]: char*\033[0m\n");
						printf("\t[!] \033[1m[ID_PEDIDO]: int\033[0m\n");
						return ERROR;
					}
					break;
				case CONFIRMAR_PEDIDO:
					if (!parameters[2]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mAPP CONFIRMAR_PEDIDO [ID_PEDIDO]\033[0m\n");
						printf("\t[!] \033[1m[ID_PEDIDO]: int\033[0m\n");
						return ERROR;
					}
					break;
				case PLATO_LISTO:
					if (!parameters[2] || !parameters[3] || !parameters[4]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mAPP PLATO_LISTO [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_PLATO]\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_RESTAURANTE]: char*\033[0m\n");
						printf("\t[!] \033[1m[ID_PEDIDO]: int\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_PLATO]: char*\033[0m\n");
						return ERROR;
					}
					break;
				case CONSULTAR_PEDIDO:
					if (!parameters[2]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mAPP CONSULTAR_PEDIDO [ID_PEDIDO]\033[0m\n");
						printf("\t[!] \033[1m[ID_PEDIDO]: int\033[0m\n");
						return ERROR;
					}
					break;
			}
			break;
		case OPTION_COMANDA:
			switch (command) {
				case GUARDAR_PEDIDO:
					if (!parameters[2] || !parameters[3]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mCOMANDA GUARDAR_PEDIDO [NOMBRE_RESTAURANTE] [ID_PEDIDO]\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_RESTAURANTE]: char*\033[0m\n");
						printf("\t[!] \033[1m[ID_PEDIDO]: int\033[0m\n");
						return ERROR;	
					}
					break;
				case GUARDAR_PLATO:
					if (!parameters[2] || !parameters[3] || !parameters[4] || !parameters[5]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mCOMANDA GUARDAR_PLATO [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_PLATO] [CANTIDAD]\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_RESTAURANTE]: char*\033[0m\n");
						printf("\t[!] \033[1m[ID_PEDIDO]: int\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_PLATO]: char*\033[0m\n");
						printf("\t[!] \033[1m[CANTIDAD]: int\033[0m\n");
						return ERROR;	
					}
					break;
				case CONFIRMAR_PEDIDO:
					if (!parameters[2] || !parameters[3]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mSINDICATO CONFIRMAR_PEDIDO [ID_PEDIDO] [NOMBRE_RESTAURANTE]\033[0m\n");
						printf("\t[!] \033[1m[ID_PEDIDO]: int\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_RESTAURANTE]: char*\033[0m\n");
						return ERROR;
					}
					break;
				case PLATO_LISTO:
					if (!parameters[2] || !parameters[3] || !parameters[4]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mSINDICATO PLATO_LISTO [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_PLATO]\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_RESTAURANTE]: char*\033[0m\n");
						printf("\t[!] \033[1m[ID_PEDIDO]: int\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_PLATO]: char*\033[0m\n");
						return ERROR;
					}
					break;
				case OBTENER_PEDIDO:
					if (!parameters[2] || !parameters[3]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mSINDICATO OBTENER_PEDIDO [NOMBRE_RESTAURANTE] [ID_PEDIDO]\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_RESTAURANTE]: char*\033[0m\n");
						printf("\t[!] \033[1m[ID_PEDIDO]: int\033[0m\n");
						return ERROR;
					}
					break;
			}
			break;
		case OPTION_RESTAURANTE:
			switch (command) {
				case ANIADIR_PLATO:
					if (!parameters[2] || !parameters[3]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mRESTAURANTE ANIADIR_PLATO [NOMBRE_PLATO] [ID_PEDIDO]\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_PLATO]: char*\033[0m\n");
						printf("\t[!] \033[1m[ID_PEDIDO]: int\033[0m\n");
						return ERROR;	
					}
					break;
				case CONFIRMAR_PEDIDO:
					if (!parameters[2]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mRESTAURANTE CONFIRMAR_PEDIDO [ID_PEDIDO]\033[0m\n");
						printf("\t[!] \033[1m[ID_PEDIDO]: int\033[0m\n");
						return ERROR;
					}
					break;
				case CONSULTAR_PEDIDO:
					if (!parameters[2]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mAPP CONSULTAR_PEDIDO [ID_PEDIDO]\033[0m\n");
						printf("\t[!] \033[1m[ID_PEDIDO]: int\033[0m\n");
						return ERROR;
					}
					break;
			}
			break;
		case OPTION_SINDICATO:
			switch (command) {
				case OBTENER_RESTAURANTE:
					if (!parameters[2]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mSINDICATO OBTENER_RESTAURANTE [NOMBRE_RESTAURANTE]\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_RESTAURANTE]: char*\033[0m\n");
						return ERROR;	
					}
					break;
				case GUARDAR_PEDIDO:
					if (!parameters[2] || !parameters[3]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mSINDICATO GUARDAR_PEDIDO [NOMBRE_RESTAURANTE] [ID_PEDIDO]\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_RESTAURANTE]: char*\033[0m\n");
						printf("\t[!] \033[1m[ID_PEDIDO]: int\033[0m\n");
						return ERROR;	
					}
					break;
				case GUARDAR_PLATO:
					if (!parameters[2] || !parameters[3] || !parameters[4] || !parameters[5]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mSINDICATO GUARDAR_PLATO [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_PLATO] [CANTIDAD]\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_RESTAURANTE]: char*\033[0m\n");
						printf("\t[!] \033[1m[ID_PEDIDO]: int\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_PLATO]: char*\033[0m\n");
						printf("\t[!] \033[1m[CANTIDAD]: int\033[0m\n");
						return ERROR;	
					}
					break;
				case CONFIRMAR_PEDIDO:
					if (!parameters[2] || !parameters[3]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mSINDICATO CONFIRMAR_PEDIDO [ID_PEDIDO] [NOMBRE_RESTAURANTE]\033[0m\n");
						printf("\t[!] \033[1m[ID_PEDIDO]: int\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_RESTAURANTE]: char*\033[0m\n");
						return ERROR;
					}
					break;
				case PLATO_LISTO:
					if (!parameters[2] || !parameters[3] || !parameters[4]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mSINDICATO PLATO_LISTO [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_PLATO]\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_RESTAURANTE]: char*\033[0m\n");
						printf("\t[!] \033[1m[ID_PEDIDO]: int\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_PLATO]: char*\033[0m\n");
						return ERROR;
					}
					break;
				case OBTENER_PEDIDO:
					if (!parameters[2] || !parameters[3]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mSINDICATO OBTENER_PEDIDO [NOMBRE_RESTAURANTE] [ID_PEDIDO]\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_RESTAURANTE]: char*\033[0m\n");
						printf("\t[!] \033[1m[ID_PEDIDO]: int\033[0m\n");
						return ERROR;
					}
					break;
			}
			break;
		default:
			return command;
	}
	return command;
}

