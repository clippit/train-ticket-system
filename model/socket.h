#ifndef _SOCKET_TICKET_H
#define _SOCKET_TICKET_H

#include <netinet/in.h>
#include <netdb.h>

#define HOSTNAME_MAX_LENTH 255
#define SERVICE_PORT "12306"
#define SERVER_BACKLOG 10

inline void *_get_in_addr(struct sockaddr*);

#endif