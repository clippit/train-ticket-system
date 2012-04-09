#include <arpa/inet.h>
#include "socket.h"

inline void *_get_in_addr(struct sockaddr *sa) {
  /* get socket address */
  if (sa->sa_family == AF_INET)
    return &(((struct sockaddr_in*)sa)->sin_addr);
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
