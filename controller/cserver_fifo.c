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

static int  server_fd                      = -1;
static char client_pipe_name[PATH_MAX + 1] = {'\0'};
static int  client_fd                      = -1;

void fs_init(void) {
  #ifdef DEBUG
  printf("%d: ---fs_init...\n", getpid());
  #endif

  puts("Offline Mode, using FIFOs(named pipes)");
  unlink(SERVER_PIPE);
  if (mkfifo(SERVER_PIPE, 0777) == -1)
    err(EXIT_FAILURE, "Server startup error, no FIFO created\n");
}

void fs_listener(void) {
  if ((server_fd = open(SERVER_PIPE, O_RDONLY)) == -1)
    err(EXIT_FAILURE, "Server startup error, no FIFO opened\n");

  while(1) {
    syslog(LOG_INFO, "server running.....");
    sleep(10);
  }
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
