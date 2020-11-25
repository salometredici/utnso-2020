#ifndef CLIENT_CONSOLE_H_
#define CLIENT_CONSOLE_H_

#include <commons/string.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <commons/collections/list.h>
#include "../../shared/include/shared_commons.h"

void showInvalidMsg();
void mostrarComandosValidos();
int clientOptionToKey(char *key);
void showInvalidCommandMsg(int option);
int validateCommand(int option, int command, char **parameters);

// Diccionario de opciones/módulos para la consola de Cliente

#define OPTION_APP 1
#define OPTION_COMANDA 2
#define OPTION_RESTAURANTE 3
#define OPTION_SINDICATO 4
#define OPTION_AIUDA 5
#define OPTION_BAI 6
#define OPTION_CLEAR 7

static t_keys diccionarioOpciones[] = {
    { "APP", OPTION_APP },
    { "COMANDA", OPTION_COMANDA },
    { "RESTAURANTE", OPTION_RESTAURANTE },
    { "SINDICATO", OPTION_SINDICATO },
    { "AIUDA", OPTION_AIUDA },
    { "BAI", OPTION_BAI },
    { "CLEAR", OPTION_CLEAR }
};

#define CONSOLENKEYS (sizeof(diccionarioOpciones)/sizeof(t_keys))

#endif