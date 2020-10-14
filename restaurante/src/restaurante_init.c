#include "../include/restaurante_init.h"

char *getNombreRestaurante() {
	return config_get_string_value(config, "NOMBRE_RESTAURANTE");
}

// El modelo de la metadata está sujeto a cambios
void obtenerMetadata() {
	enviarPaquete(conexionSindicato, RESTAURANTE, OBTENER_RESTAURANTE, nombreRestaurante);
	t_header *header = recibirHeaderPaquete(conexionSindicato);
	md_restaurante *md = recibirPayloadPaquete(header, conexionSindicato);
	logMetadata(md);
	// TODO: Inicializar cantidad de cocineros y sus afinidades, posición del restaurante, recetas disponibles con sus precios y cantidad de hornos
	// TODO: Cantidad de pedidos que ya disponga para no pisar los ID con los nuevos que se generen
	free(header);
	free(md);
}

void inicializarQueues() {

}

void initRestaurante() {
	conexionSindicato = conectarseA(SINDICATO);
	nombreRestaurante = getNombreRestaurante();
	obtenerMetadata();
	inicializarQueues();
}