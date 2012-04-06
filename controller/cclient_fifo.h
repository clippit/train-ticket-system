#ifndef _CONTROLLER_CLIENT_FIFO_H
#define _CONTROLLER_CLIENT_FIFO_H

#include "cclient.h"
#include "fifo.h"

void fc_init(pid_t);
void fc_request(payload_t*);
void fc_cleanup(void);

#endif