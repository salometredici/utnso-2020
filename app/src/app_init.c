#include "../include/app_init.h"

/* Utils */

int getKeyAlgoritmo(char *key) {
    t_keys *diccionario = diccionarioAlgoritmos;
    for (int i = 0; i < ALGORITMONKEYS; i++) {
        t_keys sym = diccionario[i];
        if (strcmp(sym.key, key) == 0) {
            return sym.valor;
        }
    }
    return ERROR;
}

/* Getters */

double getAlpha() {
	return config_get_double_value(config, "ALPHA");
}

int getEstimacionInicial() {
	return config_get_double_value(config, "ESTIMACION_INICIAL");
}

char *getAlgoritmoPlanificacion() {
	return config_get_string_value(config, "ALGORITMO_DE_PLANIFICACION");
}

int getGradoMultiprocesamiento() {
	return config_get_int_value(config, "GRADO_DE_MULTIPROCESAMIENTO");
}

int getTiempoRetardoCpu() {
	return config_get_int_value(config, "RETARDO_CICLO_CPU");
}

char **getRepartidores() {
	return config_get_array_value(config, "REPARTIDORES");
}

char **getFrecuenciasDescanso() {
	return config_get_array_value(config, "FRECUENCIA_DE_DESCANSO");
}

char **getTiemposDescanso() {
	return config_get_array_value(config, "TIEMPO_DE_DESCANSO");
}

char **getPlatosDefault() {
	return config_get_array_value(config,"PLATOS_DEFAULT");
}

/* Inicialización */

void inicializarRepartidores() {
	int i = 0;
	repartidoresDisponibles = list_create();
	char **tiemposDescanso = getTiemposDescanso();
	char **repartidoresConfig = getRepartidores();
	char **frecuenciasDescanso = getFrecuenciasDescanso();
	log_info(logger, "Inicializando repartidores y queues...");

	do {
		t_repartidor *repartidorActual = malloc(sizeof(t_repartidor));
		t_posicion *posRActual = malloc(sizeof(t_posicion));

		char **valores = string_split(repartidoresConfig[i], "|");
		posRActual->posX = atoi(valores[0]);
		posRActual->posY = atoi(valores[1]);

		repartidorActual->idRepartidor = i;
		repartidorActual->posRepartidor = posRActual;
		repartidorActual->tiempoDescanso = atoi(tiemposDescanso[i]);
		repartidorActual->freqDescanso = atoi(frecuenciasDescanso[i]);

		log_info(logger, "[Repartidor #%d]: PosX: %d, PosY %d, FreqDescanso: %d, TiempoDescanso: %d",
				i,
				repartidorActual->posRepartidor->posX,
				repartidorActual->posRepartidor->posY,
				repartidorActual->freqDescanso,
				repartidorActual->tiempoDescanso);
		
		list_add(repartidoresDisponibles, repartidorActual);
		free(repartidorActual);
		free(posRActual);
		i++;
	} while(repartidoresConfig[i] != NULL);

	cantidadRepartidores = list_size(repartidoresDisponibles);
}

void inicializarPosResDefault() {
	posResDefault = malloc(sizeof(t_posicion));
	posResDefault->posX = config_get_int_value(config,"POSICION_REST_DEFAULT_X");
	posResDefault->posY = config_get_int_value(config,"POSICION_REST_DEFAULT_Y");
	log_info(logger, "[Posición Default]: PosX: %d, PosY: %d", posResDefault->posX, posResDefault->posY);
}

void inicializarPlatosDefault() {
	platosResDefault = list_create();
	char **platosDefault = getPlatosDefault();
	int i = 0;
	do {
		list_add(platosResDefault, platosDefault[i]);
		log_info(logger, "[Plato Default #%d]: %s", i, platosDefault[i]);
		i++;
	} while(platosDefault[i] != NULL);
}

void inicializarRestauranteDefault() {
	log_info(logger, "Inicializando Restaurante Default...");
	inicializarPosResDefault();
	inicializarPlatosDefault();
}

void inicializarVariablesGlobales() {
	alpha = getAlpha();
	tiempoRetardoCpu = getTiempoRetardoCpu();
	estimacionInicial = getEstimacionInicial();
	gradoMultiprocesamiento = getGradoMultiprocesamiento();
	algoritmo = getAlgoritmoPlanificacion();
	algoritmoSeleccionado = getKeyAlgoritmo(algoritmo);
	clientesConectados = list_create();
	restaurantesConectados = list_create();
	repartidoresDisponibles = list_create();
	repartidoresOcupados = list_create();
}

void inicializarQueues() {
	qN = queue_create();
	qR = queue_create();
	qE = queue_create();
	qB = queue_create();
	qF = queue_create();
}

void initApp() {
	conexionComanda = conectarseA(COMANDA);
	inicializarVariablesGlobales();
	inicializarRestauranteDefault();
	inicializarRepartidores();
	inicializarQueues();
}
