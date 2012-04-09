#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include <string.h>
#include <syslog.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "cserver_socket.h"

int sockfd = -1;

void ss_init(void) {
  #ifdef DEBUG
  printf("%d: ---ss_init...\n", getpid());
  #endif

  struct addrinfo hints, *servinfo, *p;
  int rv;

  memset(&hints, 0, sizeof(hints));
  hints.ai_flags    = AI_PASSIVE;  // use my IP address
  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(NULL, SERVICE_PORT, &hints, &servinfo)) != 0)
    errx(EXIT_SUCCESS, "getaddrinfo error: %s", gai_strerror(rv));

  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
      warn("Cannot open socket");
      continue;
    }
    if (bind(sockfd, p->ai_addr, p->ai_addrlen) < 0) {
      close(sockfd);
      warn("Cannot bind socket");
      continue;
    }
    break;  // if we get here, we must have connected successfully
  }

  if (p == NULL)
    err(EXIT_FAILURE, "Failed to set up server");

  freeaddrinfo(servinfo);

  printf("Online Mode, using Sockets. Server at 0.0.0.0:%s\n", SERVICE_PORT);

  if (listen(sockfd, SERVER_BACKLOG) < 0)
    err(EXIT_FAILURE, "Cannot listen.");
}

void ss_listener(void) {
  while(1) {
    #ifdef DEBUG
    syslog(LOG_INFO, "server: waiting for connections...");
    #endif

    struct sockaddr_storage client_addr;
    socklen_t client_addr_size = sizeof(client_addr); 
    int client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_size);
    if (client_sockfd < 0) {
      syslog(LOG_ERR, "An error occurred when accepting a connection.");
      continue;
    }

    pid_t fork_pid;
    if ((fork_pid = fork()) < 0) {
      syslog(LOG_ERR, "Fork failed.");
      exit(EXIT_FAILURE);
    } else if (fork_pid == 0) {  // child
      char addr[INET6_ADDRSTRLEN];
      inet_ntop(client_addr.ss_family, _get_in_addr((struct sockaddr *)&client_addr), addr, sizeof(addr));
      syslog(LOG_INFO, "Connection from %s", addr);
      close(sockfd);

      request_t request   = {0, -1, "\0", "\0", "\0", "\0", "\0", 0, 0};
      response_t response = {0, "\0"};

      if (!_socket_read_request(client_sockfd, &request)) {
        syslog(LOG_ERR, "An error occurred when reading request.");
        _exit(EXIT_FAILURE);
      }
      action_dispatch(&request, &response);  // entrance of action handling
      if (!_socket_send_response(client_sockfd, &response)) {
        syslog(LOG_ERR, "An error occured when sending response.");
        _exit(EXIT_FAILURE);
      }
      close(client_sockfd);
      _exit(EXIT_SUCCESS);
    } else {  // parent
      close(client_sockfd);
      continue;
    }
  }
}

void ss_cleanup(void) {
  #ifdef DEBUG
  syslog(LOG_INFO, "%d: ss_cleanup", getpid());
  #endif

  shutdown(sockfd, SHUT_RDWR);
  close(sockfd);
}

void ss_sigterm(int sig) {
  #ifdef DEBUG
  syslog(LOG_INFO, "receive signal %d", sig);
  #endif
  exit(EXIT_SUCCESS);
}

int _socket_read_request(const int client_sockfd, request_t *request) {
  int ret = 0;
  int receive_bytes;
  int request_size = sizeof(*request);
  if (client_sockfd != -1) {
    receive_bytes = recv(client_sockfd, request, request_size, MSG_WAITALL);
    if (receive_bytes != request_size) {
      syslog(LOG_WARNING, "Read request fault.");
      return ret;
    }
  } else {
    syslog(LOG_WARNING, "Client Socket not existed.");
    return ret;
  }
  return ret = 1;
}

int _socket_send_response(const int client_sockfd, const response_t* response) {
  int ret = 0;
  int send_bytes;
  int response_size = sizeof(*response);
  if (client_sockfd != -1) {
    send_bytes = send(client_sockfd, response, response_size, 0);
    if (send_bytes != response_size) {
      syslog(LOG_WARNING, "Send response fault.");
      return ret;
    }
  } else {
    syslog(LOG_WARNING, "Client Socket not existed.");
    return ret;
  }
  return ret = 1;
}