#include "../include/app_init.h"

void cargarConfiguracionApp() {
	appConfig= malloc(sizeof(appConfig_t));

	appConfig->platosDefault =config_get_array_value(config,"PLATOS_DEFAULT");
	appConfig->posicionRestDefaultX = config_get_int_value(config,"POSICION_REST_DEFAULT_X");
	appConfig->posicionRestDefaultY = config_get_int_value(config,"POSICION_REST_DEFAULT_Y");	
	/*
	printf("\n\n grado de multiprogramacion: %d",appConfig->gradoMultiprocesamiento);
	printf("\n algoritmo de planificacion: %s",appConfig->algoritmoPlanificacion);
	printf("\n alpha: %.2f", appConfig->alpha);
	printf("\n estimacion inicial: %d", appConfig->estimacionInicial);
	printf("\n platos default: %s", appConfig->platosDefault[0]);
	printf("\n posicion restaurante default X: %d", appConfig->posicionRestDefaultX);
	printf("\n posicion restaurante default Y: %d\n", appConfig->posicionRestDefaultY);

	int i;
	for (i = 0; appConfig->posicionRepartidores[i] != NULL; i++)
	{
		printf("\n posicion del repartidor %d: %s",i , appConfig->posicionRepartidores[i]);
    }
	printf("\n cantidad de posiciones de repartidores es: %d\n", i);
	
	for (i = 0; appConfig->frecuenciaDescando[i] != NULL; i++)
	{
		printf("\n frecuencia de descanso %d: %s",i, appConfig->frecuenciaDescando[i]);
    }

	for (i = 0; appConfig->tiempoDescanso[i] != NULL; i++)
	{
     	printf("\n tiempo de descanso %d: %s",i, appConfig->tiempoDescanso[i]);
    }
	printf("\n\n");
	*/
	free(appConfig);
}

void inicializarEstructurasYListas() {
	
	listaPedidos = list_create(); // aca se van a ir colocando los PCB
	listaRepartidores = list_create();
	NUEVOS = list_create();
    LISTOS = list_create();
	EJECUTANDO = list_create();
    BLOQUEADOS = list_create();
    FINALIZADOS = list_create();

	cargarPCByListas();

}
void cargarPCByListas() {
// Aca se carga los repartidores a la lista de repartidores
// y los PCB que referencian a cada pedido
}

//-----

double getAlpha() {
	return config_get_double_value(config, "ALPHA");
}

int getEstimacionInicial() {
	return config_get_int_value(config, "ESTIMACION_INICIAL");
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

void inicializarRepartidores() {
	int i = 0;
	repartidores = list_create();
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

		repartidorActual->posRepartidor = posRActual;
		repartidorActual->tiempoDescanso = atoi(tiemposDescanso[i]);
		repartidorActual->freqDescanso = atoi(frecuenciasDescanso[i]);

		log_info(logger, "[Repartidor #%d]: PosX: %d, PosY %d, FreqDescanso: %d, TiempoDescanso: %d",
				i,
				repartidorActual->posRepartidor->posX,
				repartidorActual->posRepartidor->posY,
				repartidorActual->freqDescanso,
				repartidorActual->tiempoDescanso);
		
		list_add(repartidores, repartidorActual);
		free(repartidorActual);
		free(posRActual);
		i++;
	} while(repartidoresConfig[i] != NULL);

	cantidadRepartidores = list_size(repartidores);
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
	algoritmoPlanificacion = getAlgoritmoPlanificacion();
	gradoMultiprocesamiento = getGradoMultiprocesamiento();
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


	// 	cargarConfiguracionApp();
	
	// inicializarEstructurasYListas();

	// planificar(); // seguramente habra que tener un hilo para esto. Despues ver donde va esta funcion
}
