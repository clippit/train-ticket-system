#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <limits.h>
#include <string.h>
#include "cclient_fifo.h"
#include "cclient.h"

static int  server_fd                      = -1;
static char client_pipe_name[PATH_MAX + 1] = {'\0'};
static int  client_fd                      = -1;
static int  client_write_fd                = -1;

void fc_init(pid_t client_pid) {
  #ifdef DEBUG
  printf("---%d: fc_init...\n", client_pid);
  #endif

  if ((server_fd = open(SERVER_PIPE, O_WRONLY)) == -1) 
    err(EXIT_FAILURE, "Is server running?");

  (void)sprintf(client_pipe_name, CLIENT_PIPE, client_pid);
  (void)unlink(client_pipe_name);
  if (mkfifo(client_pipe_name, 0777) == -1) 
      err(EXIT_FAILURE, "Unable to create client pipe %s\n", client_pipe_name);
}

void fc_request(request_t *request, response_t* response) {
  if (send_request(request) && read_response(response)) {

  }
    
}

int send_request(request_t *request) {
  int ret = 0;
  int write_bytes;
  size_t payload_size = sizeof(*request);
  if (server_fd == -1) {
    warnx("Server Pipe fault.");
    return ret;
  }
  write_bytes = write(server_fd, request, payload_size);
  if (write_bytes != payload_size) {
    warnx("Sent request to server fault.");
    return ret;
  }
  ret = 1;
  return ret;
}

int read_response(response_t* response) {

  strcpy(response->content, "Hello");
  return 1;
}

void fc_cleanup(void) {
  #ifdef DEBUG
  printf("%d: ---fc_cleanup...\n", getpid());
  #endif

  if (client_write_fd != -1) (void)close(client_write_fd);
  if (client_fd != -1) (void)close(client_fd);
  if (server_fd != -1) (void)close(server_fd);
  (void)unlink(client_pipe_name);
}
