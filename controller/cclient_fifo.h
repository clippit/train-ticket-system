#ifndef _CONTROLLER_CLIENT_FIFO_H
#define _CONTROLLER_CLIENT_FIFO_H

#include "cclient.h"

int  fc_init(void);
int  fc_register(user*);
int  fc_login(user*);
int  fc_search(client_options*);
int  fc_order(client_options*);
int  fc_view(order*);
int  fc_refund(unsigned long int);
void fc_cleanup(void);

#endif