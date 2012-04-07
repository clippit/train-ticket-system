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

int fc_request(request_t *request, response_t* response) {
  int ret = 0;
  if (send_request(request) && read_response(response)) {
    ret = 1;
  } else {
    warnx("An error occurred during communication.");
  }
  return ret;
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
  return ret = 1;
}

int open_response() {
  if (client_pipe_name[0] == '\0')
    return 0;
  if (client_fd != -1)
    return 1;

  client_fd = open(client_pipe_name, O_RDONLY);
  if (client_fd != -1) {
      client_write_fd = open(client_pipe_name, O_WRONLY);
      if (client_write_fd != -1)
        return 1;
      (void)close(client_fd);
      client_fd = -1;
  }
  return 0;
}

int read_response(response_t* response) {
  int ret = 0;
  if(!open_response()) {
    warnx("Open response pipe failed.");
    return ret;
  }

  #ifdef DEBUG
  printf("---%d: start read_response...\n", getpid());
  #endif

  int read_bytes;
  size_t response_size = sizeof(*response);
  if (!response) {
    warnx("Response parameter invalid.");
    return ret;
  }
  if (client_fd == -1)
    return ret;

  read_bytes = read(client_fd, response, response_size);
  if (read_bytes != response_size) {
    warnx("Read response fault.");
    return ret;
  }
  return ret = 1;
}

void fc_cleanup(void) {
  #ifdef DEBUG
  printf("---%d: fc_cleanup...\n", getpid());
  #endif

  if (client_write_fd != -1) (void)close(client_write_fd);
  if (client_fd != -1) (void)close(client_fd);
  if (server_fd != -1) (void)close(server_fd);
  (void)unlink(client_pipe_name);
}
