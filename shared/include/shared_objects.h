#ifndef SHARED_OBJECTS_H
#define SHARED_OBJECTS_H

#include "../include/shared_commons.h"
#include "../include/shared_core.h"
#include "../include/shared_logging.h"

t_request *getTRequest(int idPedido, char *nombre);
t_result *getTResult(char *msg, bool hasError);

#endif