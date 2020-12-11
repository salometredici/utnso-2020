#include "../include/restaurante_init.h"

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

void logInitQueuesRestaurante(t_list *queuesCocineros) {
	printf("------------------------------------------------------\n");
	printf("[Queues de cocineros creadas]\n");
	log_info(logger, "[Queues de cocineros creadas]");
	for (int i = 0; i < list_size(queuesCocineros); i++) {
		t_queue_obj *cpuActual = list_get(queuesCocineros, i);
		printf("\t[CPU #%d]: Afinidad: %s, Instancias: %d\n", i, cpuActual->afinidad, cpuActual->instanciasTotales);
		log_info(logger, "\t[CPU #%d]: Afinidad: %s, Instancias: %d", i, cpuActual->afinidad, cpuActual->instanciasTotales);
	}
	printf("------------------------------------------------------\n");
}

void logInitQueueIORestaurante() {
	printf("[Queue I/O creada] - Instancias: %d\n", instanciasTotalesIO);
	log_info(logger, "[Queue I/O creada] - Instancias: %d", instanciasTotalesIO);
	printf("------------------------------------------------------\n");
}

char *getNombreRestaurante() {
	return config_get_string_value(config, "NOMBRE_RESTAURANTE");
}

char *getAlgoritmoPlanificacion() {
	return config_get_string_value(config, "ALGORITMO_PLANIFICACION");
}

char *getQuantum() {
	return config_get_int_value(config, "QUANTUM");
}

int getTiempoRetardoCpu() {
	return config_get_int_value(config, "RETARDO_CICLO_CPU");
}

void obtenerMetadata() {
	conexionSindicato = conectarseA(SINDICATO);
	enviarPaquete(conexionSindicato, RESTAURANTE, OBTENER_RESTAURANTE, nombreRestaurante);
	t_header *header = recibirHeaderPaquete(conexionSindicato);
	t_md *md = recibirPayloadPaquete(header, conexionSindicato);
	liberarConexion(conexionSindicato);
	inicializarVariablesMd(md);
	logMetadata(md); // Probar con log_rta_ObtenerRestaurante
	free(header);
	free(md);
}

int crearConexionOpcional() {
	char *ip = config_get_string_value(config, "IP_APP");
	int puerto = config_get_int_value(config, "PUERTO_APP");
	int socketCliente;
	struct sockaddr_in dir;

	socketCliente = crearSocket(puerto);

	dir.sin_family = AF_INET;
	dir.sin_port = htons(puerto);
	dir.sin_addr.s_addr = inet_addr(ip);
	memset(&(dir.sin_zero), '\0', 8);
	
	if (connect(socketCliente, (struct sockaddr*)&dir, sizeof(struct sockaddr)) == ERROR) {
		log_connection_failure(ip, puerto);
		close(socketCliente);
		socketCliente = ERROR;
	}
	else {
		log_connection_success(APP, puerto);
	}

	return socketCliente;
}

void conectarseConApp(){
	int conexionApp = crearConexionOpcional();
	if (conexionApp != ERROR) {
		t_cliente *dataCliente = malloc(sizeof(t_cliente));
		dataCliente->esRestaurante = true;
		dataCliente->restSelecc = nombreRestaurante;
		dataCliente->posRest = malloc(sizeof(t_posicion));
		dataCliente->posRest->posX = posicion->posX;
		dataCliente->posRest->posY = posicion->posY;
		dataCliente->idCliente = nombreRestaurante;
		dataCliente->posCliente = malloc(sizeof(t_posicion));
		dataCliente->posCliente->posX = ERROR;
		dataCliente->posCliente->posY = ERROR;
		dataCliente->ip_cliente = config_get_string_value(config, "IP_RESTAURANTE");
		dataCliente->puerto_cliente = config_get_int_value(config, "PUERTO_ESCUCHA");
		dataCliente->socket_notifs = ERROR;
		dataCliente->socketCliente = ERROR;
		log_init_data_cliente(dataCliente);
		enviarPaquete(conexionApp, RESTAURANTE, ENVIAR_DATACLIENTE, dataCliente);
		free(dataCliente);
		liberarConexion(conexionApp);
	}
}

void inicializarVariablesMd(t_md *md) {
	recetasDisponibles = list_create();
	afinidadesMd = list_create();
	posicion = malloc(sizeof(t_posicion));
	recetasDisponibles = md->platos;
	posicion->posX = md->posX;
	posicion->posY = md->posY;
	cantidadHornos = md->cantidadHornos;
	cantidadPedidos = md->cantidadPedidos;
	cantidadCocineros = md->cantidadCocineros;
	afinidadesMd = md->afinidades;
}

void inicializarAfinidadesUnicas()
{
	// Primero agregamos las afinidades sin repetidos de las obtenidas de Sindicato
	for (int i = 0; i < QAfinidadesMd; i++) {
		char *afinidadActual = list_get(afinidadesMd, i);

		bool stringFoundv2(void *actual) {
			char *stringActual = actual;
			return string_equals_ignore_case(afinidadActual, stringActual);
		};

		// Buscamos si esa afinidad ya fue agregada a la lista de afinidades únicas
		t_list *filtradas = list_filter(afinidadesUnicas, &stringFoundv2); 
		
		if (list_is_empty(filtradas)) {
			list_add(afinidadesUnicas, afinidadActual);
		}
	}
	
	// Si la cantidad de afinidades obtenidas de Sindicato era menor a la cantidad de cocineros, debemos agregar la afinidad General
	if (QAfinidadesMd < cantidadCocineros) {
		list_add(afinidadesUnicas, "General");
	}

	QAfinidadesUnicas = list_size(afinidadesUnicas);
}

void inicializarQueuesGlobales(){
    qF = queue_create();
	pthread_mutex_init(&mutexQF, NULL);
}

void inicializarListaCocineros()
{
	queuesCocineros = list_create();
	afinidadesUnicas = list_create();
	QAfinidadesMd = list_size(afinidadesMd);
	inicializarAfinidadesUnicas();

	// Una vez que tenemos la lista de afinidades únicas creamos los t_queue_obj correspondientes a cada una

	for (int i = 0; i < QAfinidadesUnicas; i++) {
		t_queue_obj *cpuActual = malloc(sizeof(t_queue_obj));
		char *afinidadActual = list_get(afinidadesUnicas, i);
		cpuActual->afinidad = afinidadActual;

		bool stringFoundv2(void *actual) {
			char *stringActual = actual;
			return string_equals_ignore_case(afinidadActual, stringActual);
		};

		cpuActual->instanciasTotales = !string_equals_ignore_case(cpuActual->afinidad, "General") ?
											list_count_satisfying(afinidadesMd, &stringFoundv2) :
											cantidadCocineros - list_size(afinidadesMd);
		
		cpuActual->qR = queue_create();
		cpuActual->qE = queue_create();
		cpuActual->qB = queue_create();

		pthread_mutex_init(&cpuActual->mutexQR, NULL);
		pthread_mutex_init(&cpuActual->mutexQE, NULL);
		pthread_mutex_init(&cpuActual->mutexQB, NULL);

		list_add(queuesCocineros, cpuActual);
	}

	logInitQueuesRestaurante(queuesCocineros);
}

void inicializarQueuesIO() {
	esperandoIO = queue_create();
	ejecutandoIO = queue_create();
	instanciasTotalesIO = cantidadHornos;
	logInitQueueIORestaurante();
}

void inicializarVariablesGlobales() {
	quantum = getQuantum();
	nombreRestaurante = getNombreRestaurante();
	algoritmo = getAlgoritmoPlanificacion();
	algoritmoSeleccionado = getKeyAlgoritmo(algoritmo);
	tiempoRetardoCpu = getTiempoRetardoCpu();
	clientesConectados = list_create();
}

void initRestaurante() {
	inicializarVariablesGlobales();
	obtenerMetadata();
	conectarseConApp();
	inicializarQueuesGlobales();
	inicializarListaCocineros();
	inicializarQueuesIO();
}