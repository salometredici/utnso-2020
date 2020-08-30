#include "../include/cliente.h"

int main(void)
{
	int conexion;
	char* ip;
	char* puerto;

	t_log* logger = iniciar_logger();
    log_info(logger, "a ver");

	t_config* config = leer_config();
    char* valor = config_get_string_value(config, "CLAVE");
    log_info(logger, valor);

	leer_consola(logger);

    ip = config_get_string_value(config, "IP");
    puerto = config_get_string_value(config, "PUERTO");
    conexion = crear_conexion(ip, puerto);

	enviar_mensaje(valor, conexion);

	paquete(conexion);

	terminar_programa(conexion, logger, config);
}

t_log* iniciar_logger()
{
    return log_create("cliente.log", "cliente", 1, LOG_LEVEL_INFO);
}

t_config* leer_config()
{
    return config_create("cliente.config");
}

void leer_consola(t_log* logger)
{
	void loggear(char* leido) {
		log_info(logger, leido);
	}
	leer_consola_y((void*) loggear);
}

void leer_consola_y(void(*accion)(char*)) {
	char* leido;
	leido = readline(">");
    while(strncmp(leido, "", 1) != 0){
        accion(leido);
        free(leido);
        leido = readline(">");
    }
	free(leido);
}

void paquete(int conexion)
{
	t_paquete* paquete;
	paquete = crear_paquete();
	void agregar(char* leido) {
		agregar_a_paquete(paquete, leido, strlen(leido) + 1);
	}
	leer_consola_y((void*) agregar);
	enviar_paquete(paquete, conexion);
	eliminar_paquete(paquete);
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
    log_destroy(logger);
	config_destroy(config);
	liberar_conexion(conexion);
}