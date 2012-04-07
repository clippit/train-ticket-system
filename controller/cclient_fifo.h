#ifndef _CONTROLLER_CLIENT_FIFO_H
#define _CONTROLLER_CLIENT_FIFO_H

#include "cclient.h"
#include "fifo.h"

void fc_init(pid_t);
int  fc_request(request_t*, response_t*);
void fc_cleanup(void);

int send_request(request_t*);
int read_response(response_t*);

#endif