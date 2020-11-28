#ifndef SHARED_OBJECTS_H
#define SHARED_OBJECTS_H

#include "../include/shared_commons.h"
#include "../include/shared_core.h"
#include "../include/shared_logging.h"

t_request *getTRequest(int idPedido, char *nombre);
t_result *getTResult(char *msg, bool hasError);
t_receta *getEmptyRecipe();
t_instrucciones_receta *getTPaso(char *paso, int qPaso);
t_md *getEmptyMd();
t_pedido *getEmptyPedido();
t_pedido *getEmptyPedido_with_error(t_estado error_code);
t_req_plato *getTReqPlato(char *rest, int idPedido, char *plato, int cantidad);

#endif