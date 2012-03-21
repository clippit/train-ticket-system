#ifndef _CONTROLLER_CLIENT_H
#define _CONTROLLER_CLIENT_H

#include "common.h"

typedef struct client_options_t {
    char name [TRAIN_NUMBER_MAX_LENGTH + 1];
    char from[TRAIN_STATION_MAX_LENGTH + 1];
    char to  [TRAIN_STATION_MAX_LENGTH + 1];
    unsigned long int order_id;
    unsigned short int amount;
} client_options;

void client_run(user* current_user, client_options* options);

#endif
