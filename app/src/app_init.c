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

int get_grado_multiprocesamiento() {
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
	repartidores_disp = list_create();
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
		
		list_add(repartidores_disp, repartidorActual);
		i++;
	} while(repartidoresConfig[i] != NULL);

	cantidadRepartidores = list_size(repartidores_disp);
}

void init_pos_rest_default() {
	posResDefault = malloc(sizeof(t_posicion));
	posResDefault->posX = config_get_int_value(config,"POSICION_REST_DEFAULT_X");
	posResDefault->posY = config_get_int_value(config,"POSICION_REST_DEFAULT_Y");
	log_info(logger, "[Posición Default]: PosX: %d, PosY: %d", posResDefault->posX, posResDefault->posY);
}

void init_platos_rest_default() {
	platos_rest_default = list_create();
	char **platosDefault = getPlatosDefault();
	int i = 0;
	do {
		list_add(platos_rest_default, platosDefault[i]);
		log_info(logger, "[Plato Default #%d]: %s", i, platosDefault[i]);
		i++;
	} while(platosDefault[i] != NULL);
}

void inicializarRestauranteDefault() {
	log_info(logger, "Inicializando Restaurante Default...");
	init_pos_rest_default();
	init_platos_rest_default();
}

void inicializarVariablesGlobales() {
	alpha = getAlpha();
	tiempoRetardoCpu = getTiempoRetardoCpu();
	estimacionInicial = getEstimacionInicial();
	grado_multiprocesamiento = get_grado_multiprocesamiento();
	algoritmo = getAlgoritmoPlanificacion();
	algoritmoSeleccionado = getKeyAlgoritmo(algoritmo);
	clientes_conectados = list_create();
	rests_conectados = list_create();
	repartidores_disp = list_create();
	repartidoresOcupados = list_create();
	idsGenerados = list_create();
	pthread_mutex_init(&mutex_rests, NULL);
	pthread_mutex_init(&mutex_clientes, NULL);
	pthread_mutex_init(&mutexListaDisponibles, NULL);
	pthread_mutex_init(&mutexListaOcupados, NULL);
	pthread_mutex_init(&mutexQN, NULL);
	pthread_mutex_init(&mutexQR, NULL);
	pthread_mutex_init(&mutexQE, NULL);
	pthread_mutex_init(&mutexQB, NULL);
	pthread_mutex_init(&mutexQF, NULL);
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
