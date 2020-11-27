#include "../include/shared_objects.h"

t_request *getTRequest(int idPedido, char *nombre) {
    t_request *request = malloc(sizeof(t_request));
    request->idPedido = idPedido;
    request-> nombre = nombre;
    return request;
}

t_result *getTResult(char *msg, bool hasError) {
    t_result *result = malloc(sizeof(t_result));
    result->msg = msg;
    result->hasError = hasError;
    return result;
}

t_receta *getEmptyRecipe() {
    t_receta *receta = malloc(sizeof(t_receta));
    receta->plato = RECETA_NO_EXISTE;
    receta->instrucciones = list_create();
    return receta;
}

t_receta *getTReceta() { // Revisar
    t_receta *receta = malloc(sizeof(t_receta));
    receta->plato = "";
    receta->instrucciones = list_create();
    t_instrucciones_receta *paso1 = malloc(sizeof(t_instrucciones_receta));
    paso1->paso ="";
    paso1->qPaso = 0;
    return receta;
}

t_instrucciones_receta *getTPaso(char *paso, int qPaso) {
    t_instrucciones_receta *inst = malloc(sizeof(t_instrucciones_receta));
    inst->paso = paso;
    inst->qPaso = qPaso;
    return inst;
}

t_md *getEmptyMd() {
    t_md *md = malloc(sizeof(t_md));
    md->posX = ERROR;
    md->posY = ERROR;
    md->cantidadHornos = ERROR;
    md->cantidadPedidos = ERROR;
    md->cantidadCocineros = ERROR;
    md->platos = list_create();
    md->afinidades = list_create();
    return md;
}

t_pedido *getEmptyPedido() {
    t_pedido *pedido = malloc(sizeof(t_pedido));
    pedido->estado = PENDIENTE;
    pedido->precioTotal = ERROR;
    pedido->restaurante = string_new();
    pedido->platos = list_create();
    return pedido;
}

t_pedido *getEmptyPedido_with_error(t_estado error_code) {
    t_pedido *pedido = malloc(sizeof(t_pedido));
    pedido->restaurante = string_new();
    pedido->estado = error_code == SIN_PLATOS ? PENDIENTE : error_code;
    pedido->platos = list_create();
    pedido->precioTotal = ERROR;
    return pedido;
}