#ifndef _CONTROLLER_SERVER_FIFO_H
#define _CONTROLLER_SERVER_FIFO_H

#include "cserver.h"
#include "fifo.h"

void fs_init(void);
void fs_listener(void);
// int  fc_register(user*);
// int  fc_login(user*);
// int  fc_search(client_options*);
// int  fc_order(client_options*);
// int  fc_view(order*);
// int  fc_refund(unsigned long int);
void fs_cleanup(void);
void fs_sigterm(int);

#endif