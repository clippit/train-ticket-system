#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <err.h>
#include <limits.h>
#include "cserver_fifo.h"
#include "cserver.h"

static int server_fd       = -1;
static int server_fd_write = -1;

void fs_init(void) {
  #ifdef DEBUG
  printf("%d: ---fs_init...\n", getpid());
  #endif

  puts("Offline Mode, using FIFOs(named pipes)");
  unlink(SERVER_PIPE);
  if (mkfifo(SERVER_PIPE, 0777) == -1)
    err(EXIT_FAILURE, "Server startup error, no FIFO created\n");
  /* open server pipe as read & write to prevent blocking and EOF, but we only read it actually*/
  if ((server_fd = open(SERVER_PIPE, O_RDONLY | O_NONBLOCK)) == -1)
    err(EXIT_FAILURE, "Server startup error, no FIFO opened\n");
  if ((server_fd_write = open(SERVER_PIPE, O_WRONLY)) == -1)
    err(EXIT_FAILURE, "server startup error, no FIFO for writing opened\n");
  _clr_fl(server_fd, O_NONBLOCK);
}

void fs_listener(void) {
  while(1) {
    #ifdef DEBUG
    syslog(LOG_INFO, "server listener running.....");
    #endif

    request_t request = {0, -1, "\0", "\0", "\0", "\0", "\0", 0, 0};
    if (!_fifo_read_request(&request)) {
      syslog(LOG_ERR, "An error occurred when reading request.");
      continue;
    }
    #ifdef DEBUG
    syslog(LOG_INFO, "got a request!.....");
    #endif

    pid_t fork_pid;
    if ((fork_pid = fork()) < 0) {
      syslog(LOG_ERR, "Fork failed.");
      exit(EXIT_FAILURE);
    } else if (fork_pid == 0) {  // child
      response_t response = {0, "\0"};
      action_dispatch(&request, &response);  // entrance of action handling
      char client_pipe_name[PATH_MAX + 1] = {'\0'};
      (void)sprintf(client_pipe_name, CLIENT_PIPE, request.client_pid);
      if (!_fifo_send_response(client_pipe_name, &response)) {
        syslog(LOG_ERR, "An error occured when sending response.");
        _exit(EXIT_FAILURE);
      }
      _exit(EXIT_SUCCESS);
    } else {  // parent
      continue;
    }
  }
}

int _fifo_read_request(request_t* request) {
  int ret = 0;
  int read_bytes;
  int request_size = sizeof(*request);
  if (server_fd != -1) {
    read_bytes = read(server_fd, request, request_size);
    if (read_bytes != request_size) {
      syslog(LOG_WARNING, "Read request fault.");
      return ret;
    }
  } else {
    syslog(LOG_WARNING, "Server pipe not existed.");
    return ret;
  }
  return ret = 1;
}

int _fifo_send_response(const char* client_pipe_name, const response_t *response) {
  int ret = 0;
  int client_fd = -1;
  int write_bytes;
  int response_size = sizeof(*response);
  if ((client_fd = open(client_pipe_name, O_WRONLY)) == -1) {
    syslog(LOG_WARNING, "Cannot open client pipe");
    return ret;
  }

  write_bytes = write(client_fd, response, response_size);
  if (write_bytes != response_size) {
    syslog(LOG_WARNING, "Write client pipe fault.");
    return ret;
  }

  (void)close(client_fd);
  return ret = 1;
}

void fs_cleanup(void) {
  #ifdef DEBUG
  syslog(LOG_INFO, "%d: fs_cleanup", getpid());
  #endif

  (void)close(server_fd);
  (void)unlink(SERVER_PIPE);
}

void fs_sigterm(int sig) {
  #ifdef DEBUG
  syslog(LOG_INFO, "receive signal %d", sig);
  #endif
  exit(EXIT_SUCCESS);
}

void _clr_fl(int fd, int flags) {
  int val;

  if ((val = fcntl(fd, F_GETFL, 0)) == -1) {
      err(EXIT_FAILURE, "fcntl() error");
  }
  val &= ~flags; /* turn flags off */

  if (fcntl(fd, F_SETFL, val) == -1) {
      err(EXIT_FAILURE, "fcntl() error");
  }
  return;
}
