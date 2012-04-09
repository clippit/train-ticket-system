#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "cclient_socket.h"

extern char hostname[HOSTNAME_MAX_LENTH + 1];
int sockfd = -1;

void sc_init(pid_t client_pid) {
  #ifdef DEBUG
  printf("---%d: sc_init...\n", client_pid);
  #endif

  struct addrinfo hints, *servinfo, *p;
  int rv;
  char addr[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof(hints));
  hints.ai_flags    = AI_CANONNAME;
  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if((rv = getaddrinfo(hostname, SERVICE_PORT, &hints, &servinfo)) != 0)
    errx(EXIT_FAILURE, "getaddrinfo error: %s", gai_strerror(rv));

  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
      warn("Cannot open socket");
      continue;
    }
    if (connect(sockfd, p->ai_addr, p->ai_addrlen) < 0) {
      close(sockfd);
      inet_ntop(p->ai_family, _get_in_addr((struct sockaddr *)p->ai_addr), addr, sizeof(addr));
      warn("Cannot connect to socket. Address: %s.", addr);
      continue;
    }
    break;  // if we get here, we must have connected successfully
  }

  if (p == NULL)
    err(EXIT_FAILURE, "Failed to connect server");

  inet_ntop(p->ai_family, _get_in_addr((struct sockaddr *)p->ai_addr), addr, sizeof(addr));
  printf("Connecting to %s...\n", addr);
  freeaddrinfo(servinfo);
}

int  sc_request(request_t* request, response_t* response) {
  return 0;
}

void sc_cleanup(void) {

}

inline void *_get_in_addr(struct sockaddr *sa) {
  /* get socket address */
  if (sa->sa_family == AF_INET)
    return &(((struct sockaddr_in*)sa)->sin_addr);
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}