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