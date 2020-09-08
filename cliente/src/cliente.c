#include "../include/cliente.h"

int main(int argc, char* argv[])
{
	inicializarProceso(CLIENTE);
    //conexionApp = conectarseA(APP);
	//conexionComanda = conectarseA(COMANDA);
	conexionRestaurante = conectarseA(RESTAURANTE);
	//conexionSindicato = conectarseA(SINDICATO);

	// Inicio del hilo de la consola y su lectura
	pthread_create(&threadConsola, NULL, (void *)threadLecturaConsola, NULL);
    pthread_detach(threadConsola);

	leer_consola(logger);

	// enviar_mensaje(valor, conexion);

	// paquete(conexion);

	liberarConexion(conexionRestaurante);
    finalizarProceso();
    return EXIT_SUCCESS;
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