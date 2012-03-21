#ifndef _CONTROLLER_CLIENT_MESSAGE_QUEUE_H
#define _CONTROLLER_CLIENT_MESSAGE_QUEUE_H

#include "cclient.h"

int  mqc_init(void);
int  mqc_register(user*);
int  mqc_login(user*);
int  mqc_search(client_options*);
int  mqc_order(client_options*);
int  mqc_view(order*);
int  mqc_refund(unsigned long int);
void mqc_cleanup(void);

#endif