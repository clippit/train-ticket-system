#ifndef _CONTROLLER_SERVER_FIFO_H
#define _CONTROLLER_SERVER_FIFO_H

#include "cserver.h"
#include "fifo.h"

void fs_init(void);
void fs_listener(void);
void fs_cleanup(void);
void fs_sigterm(int);

int read_request(request_t*);
int send_response(const char*, const response_t*);

void clr_fl(int fd, int flags);

#endif