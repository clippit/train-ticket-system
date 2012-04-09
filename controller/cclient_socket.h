#ifndef _CONTROLLER_CLIENT_SOCKET_H
#define _CONTROLLER_CLIENT_SOCKET_H

#include "cclient.h"
#include "socket.h"

void sc_init(pid_t);
int  sc_request(request_t*, response_t*);
void sc_cleanup(void);

int _socket_send_request(request_t*);
int _socket_read_response(response_t*);

#endif