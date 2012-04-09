#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "cclient_socket.h"

extern char hostname[HOSTNAME_MAX_LENTH + 1];
int sockfd = -1;

void sc_init(pid_t client_pid) {
  #ifdef DEBUG
  printf("---%d: sc_init...\n", client_pid);
  #endif

  struct addrinfo hints, *servinfo, *p;
  int rv;

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
      warn("Cannot connect to socket");
      continue;
    }
    break;  // if we get here, we must have connected successfully
  }

  if (p == NULL)
    err(EXIT_FAILURE, "Failed to connect server");

  freeaddrinfo(servinfo);
}

int  sc_request(request_t* request, response_t* response) {
  return 0;
}

void sc_cleanup(void) {

}