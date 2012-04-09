#ifndef _CONTROLLER_CLIENT_SOCKET_H
#define _CONTROLLER_CLIENT_SOCKET_H

#include "cclient.h"
#include "socket.h"

void sc_init(pid_t);
int  sc_request(request_t*, response_t*);
void sc_cleanup(void);

#endif