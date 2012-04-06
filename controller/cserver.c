#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include <syslog.h>
#include <signal.h>
#include "cserver.h"
#include "cserver_fifo.h"

void register_running_mode(int mode) {
  if (mode == MODE_ONLINE) {
    // TODO Not implemented yet
  } else if (mode == MODE_OFFLINE) {
    c.init         = fs_init;
    c.listener     = fs_listener;
    c.exit_hook    = fs_cleanup;
    c.sigterm_hook = fs_sigterm;
  } else {
    syslog(LOG_ERR, "Internal Unknown Error.");
  }
  signal(SIGTERM, c.sigterm_hook);
  signal(SIGINT,  c.sigterm_hook);
  signal(SIGHUP,  c.sigterm_hook);
  atexit(c.exit_hook);
}

void server_run() {
  c.init();
  
  /* Running as a daemon, close stdin stdout stderr but do not change working directory */
  puts( "Running as daemon..." );
  if ( daemon(1, 0) != 0 )
    err(EXIT_FAILURE, "Cannot run as a daemon.");

  c.listener();
}