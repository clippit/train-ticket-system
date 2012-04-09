#ifndef _CONTROLLER_SERVER_SERVER_H
#define _CONTROLLER_SERVER_SERVER_H

#include "cserver.h"
#include "socket.h"

void ss_init(void);
void ss_listener(void);
void ss_cleanup(void);
void ss_sigterm(int);

int _socket_read_request(const int, request_t*);
int _socket_send_response(const int, const response_t*);

#endif