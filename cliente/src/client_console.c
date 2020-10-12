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
		printf("Formato: \033[1;33m[MENSAJE]\033[0m [PARAMETROS] ~~旦_(･o･;)\n");
		switch (proceso) {
			case OPTION_APP:
				printf("\033[0;33mCONSULTAR_RESTAURANTES\033[0m\n");
				printf("\033[0;33mSELECCIONAR_RESTAURANTE\033[0m [NOMBRE_CLIENTE] [NOMBRE_RESTAURANTE]\n");
				printf("\033[0;33mCONSULTAR_PLATOS\033[0m [NOMBRE_RESTAURANTE]\n");
				printf("\033[0;33mCREAR_PEDIDO\033[0m\n");
				printf("\033[0;33mANIADIR_PLATO\033[0m [NOMBRE_PLATO] [ID_PEDIDO]\n");
				printf("\033[0;33mCONFIRMAR_PEDIDO\033[0m [ID_PEDIDO]\n");
				printf("\033[0;33mPLATO_LISTO\033[0m [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_COMIDA]\n");
				printf("\033[0;33mCONSULTAR_PEDIDO\033[0m [ID_PEDIDO]\n");
				break;
			case OPTION_COMANDA:
				printf("\033[0;33mGUARDAR_PEDIDO\033[0m [NOMBRE_RESTAURANTE] [ID_PEDIDO]\n");
				printf("\033[0;33mGUARDAR_PLATO\033[0m [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_COMIDA] [CANTIDAD]\n");
				printf("\033[0;33mCONFIRMAR_PEDIDO\033[0m [ID_PEDIDO] [NOMBRE_RESTAURANTE]\n");
				printf("\033[0;33mPLATO_LISTO\033[0m [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_COMIDA]\n");
				printf("\033[0;33mOBTENER_PEDIDO\033[0m [NOMBRE_RESTAURANTE] [ID_PEDIDO]\n");
				break;
			case OPTION_RESTAURANTE:
				printf("\033[0;33mCONSULTAR_PLATOS\033[0m [NOMBRE_RESTAURANTE]\n");
				printf("\033[0;33mCREAR_PEDIDO\033[0m\n");
				printf("\033[0;33mANIADIR_PLATO\033[0m [NOMBRE_PLATO] [ID_PEDIDO]\n");
				printf("\033[0;33mCONFIRMAR PEDIDO\033[0m [ID_PEDIDO] \n");
				printf("\033[0;33mCONSULTAR_PEDIDO\033[0m [ID_PEDIDO]\n");
				break;
			case OPTION_SINDICATO:
				printf("\033[0;33mOBTENER_RESTAURANTE\033[0m [NOMBRE_RESTAURANTE]\n");
				printf("\033[0;33mCONSULTAR_PLATOS\033[0m [NOMBRE_RESTAURANTE]\n");
				printf("\033[0;33mGUARDAR_PEDIDO\033[0m [NOMBRE_RESTAURANTE] [ID_PEDIDO]\n");
				printf("\033[0;33mGUARDAR_PLATO\033[0m [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_COMIDA] [CANTIDAD]\n");
				printf("\033[0;33mCONFIRMAR_PEDIDO\033[0m [ID_PEDIDO] [NOMBRE_RESTAURANTE]\n");
				printf("\033[0;33mPLATO_LISTO\033[0m [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_COMIDA]\n");
				printf("\033[0;33mOBTENER_PEDIDO\033[0m [NOMBRE_RESTAURANTE] [ID_PEDIDO]\n");
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
	printf("------------------------------------------------------\n");
	printf("Comando no válido. Escriba 'AIUDA' para ver el formato aceptado つ´Д`)つ\n");
	printf("------------------------------------------------------\n");
}

void showInvalidCommandMsg(int option) {
	printf("------------------------------------------------------\n");
	printf("El mensaje ingresado no es válido para el módulo %s (・ε・`*)...!\n", getStringKeyValue(option, PROCNKEYS));
	printf("------------------------------------------------------\n");
}

int validateCommand(int option, int command, char **parameters) {
	switch (option) {
		case OPTION_APP:
			switch (command) {
				case SELECCIONAR_RESTAURANTE:
					if (!parameters[1] || !parameters[2]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mSELECCIONAR_RESTAURANTE [CLIENTE] [NOMBRE_RESTAURANTE]\033[0m\n");
						printf("\t[!] \033[1m[CLIENTE]: int\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_RESTAURANTE]: char*\033[0m\n");
						return ERROR;
					}
					break;
				case ANIADIR_PLATO:
					if (!parameters[1] || !parameters[2]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mANIADIR_PLATO [NOMBRE_PLATO] [ID_PEDIDO]\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_PLATO]: char*\033[0m\n");
						printf("\t[!] \033[1m[ID_PEDIDO]: int\033[0m\n");
						return ERROR;
					}
					break;
				case CONFIRMAR_PEDIDO:
					if (!parameters[1]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mCONFIRMAR_PEDIDO [ID_PEDIDO]\033[0m\n");
						printf("\t[!] \033[1m[ID_PEDIDO]: int\033[0m\n");
						return ERROR;
					}
					break;
				case PLATO_LISTO:
					if (!parameters[1] || !parameters[2] || !parameters[3]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mPLATO_LISTO [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_PLATO]\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_RESTAURANTE]: char*\033[0m\n");
						printf("\t[!] \033[1m[ID_PEDIDO]: int\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_PLATO]: char*\033[0m\n");
						return ERROR;
					}
					break;
				case CONSULTAR_PEDIDO:
					if (!parameters[1]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mCONSULTAR_PEDIDO [ID_PEDIDO]\033[0m\n");
						printf("\t[!] \033[1m[ID_PEDIDO]: int\033[0m\n");
						return ERROR;
					}
					break;
			}
			break;
		case OPTION_COMANDA:
			switch (command) {
				case GUARDAR_PEDIDO:
					if (!parameters[1] || !parameters[2]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mGUARDAR_PEDIDO [NOMBRE_RESTAURANTE] [ID_PEDIDO]\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_RESTAURANTE]: char*\033[0m\n");
						printf("\t[!] \033[1m[ID_PEDIDO]: int\033[0m\n");
						return ERROR;	
					}
					break;
				case GUARDAR_PLATO:
					if (!parameters[1] || !parameters[2] || !parameters[3] || !parameters[4]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mGUARDAR_PLATO [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_PLATO] [CANTIDAD]\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_RESTAURANTE]: char*\033[0m\n");
						printf("\t[!] \033[1m[ID_PEDIDO]: int\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_PLATO]: char*\033[0m\n");
						printf("\t[!] \033[1m[CANTIDAD]: int\033[0m\n");
						return ERROR;	
					}
					break;
				case CONFIRMAR_PEDIDO:
					if (!parameters[1] || !parameters[2]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mCONFIRMAR_PEDIDO [ID_PEDIDO] [NOMBRE_RESTAURANTE]\033[0m\n");
						printf("\t[!] \033[1m[ID_PEDIDO]: int\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_RESTAURANTE]: char*\033[0m\n");
						return ERROR;
					}
					break;
				case PLATO_LISTO:
					if (!parameters[1] || !parameters[2] || !parameters[3]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mPLATO_LISTO [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_PLATO]\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_RESTAURANTE]: char*\033[0m\n");
						printf("\t[!] \033[1m[ID_PEDIDO]: int\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_PLATO]: char*\033[0m\n");
						return ERROR;
					}
					break;
				case OBTENER_PEDIDO:
					if (!parameters[1] || !parameters[2]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mOBTENER_PEDIDO [NOMBRE_RESTAURANTE] [ID_PEDIDO]\033[0m\n");
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
					if (!parameters[1] || !parameters[2]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mANIADIR_PLATO [NOMBRE_PLATO] [ID_PEDIDO]\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_PLATO]: char*\033[0m\n");
						printf("\t[!] \033[1m[ID_PEDIDO]: int\033[0m\n");
						return ERROR;	
					}
					break;
				case CONFIRMAR_PEDIDO:
					if (!parameters[1]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mCONFIRMAR_PEDIDO [ID_PEDIDO]\033[0m\n");
						printf("\t[!] \033[1m[ID_PEDIDO]: int\033[0m\n");
						return ERROR;
					}
					break;
				case CONSULTAR_PEDIDO:
					if (!parameters[1]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mCONSULTAR_PEDIDO [ID_PEDIDO]\033[0m\n");
						printf("\t[!] \033[1m[ID_PEDIDO]: int\033[0m\n");
						return ERROR;
					}
					break;
			}
			break;
		case OPTION_SINDICATO:
			switch (command) {
				case OBTENER_RESTAURANTE:
					if (!parameters[1]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mOBTENER_RESTAURANTE [NOMBRE_RESTAURANTE]\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_RESTAURANTE]: char*\033[0m\n");
						return ERROR;	
					}
					break;
				case GUARDAR_PEDIDO:
					if (!parameters[1] || !parameters[2]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mGUARDAR_PEDIDO [NOMBRE_RESTAURANTE] [ID_PEDIDO]\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_RESTAURANTE]: char*\033[0m\n");
						printf("\t[!] \033[1m[ID_PEDIDO]: int\033[0m\n");
						return ERROR;	
					}
					break;
				case GUARDAR_PLATO:
					if (!parameters[1] || !parameters[2] || !parameters[3] || !parameters[4]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mGUARDAR_PLATO [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_PLATO] [CANTIDAD]\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_RESTAURANTE]: char*\033[0m\n");
						printf("\t[!] \033[1m[ID_PEDIDO]: int\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_PLATO]: char*\033[0m\n");
						printf("\t[!] \033[1m[CANTIDAD]: int\033[0m\n");
						return ERROR;	
					}
					break;
				case CONFIRMAR_PEDIDO:
					if (!parameters[1] || !parameters[2]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mCONFIRMAR_PEDIDO [ID_PEDIDO] [NOMBRE_RESTAURANTE]\033[0m\n");
						printf("\t[!] \033[1m[ID_PEDIDO]: int\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_RESTAURANTE]: char*\033[0m\n");
						return ERROR;
					}
					break;
				case PLATO_LISTO:
					if (!parameters[1] || !parameters[2] || !parameters[3]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mPLATO_LISTO [NOMBRE_RESTAURANTE] [ID_PEDIDO] [NOMBRE_PLATO]\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_RESTAURANTE]: char*\033[0m\n");
						printf("\t[!] \033[1m[ID_PEDIDO]: int\033[0m\n");
						printf("\t[!] \033[1m[NOMBRE_PLATO]: char*\033[0m\n");
						return ERROR;
					}
					break;
				case OBTENER_PEDIDO:
					if (!parameters[1] || !parameters[2]) {
						showCommandErrorMsg(command);
						printf("\033[1;33mOBTENER_PEDIDO [NOMBRE_RESTAURANTE] [ID_PEDIDO]\033[0m\n");
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

