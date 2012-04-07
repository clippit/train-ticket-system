#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include <syslog.h>
#include <signal.h>
#include <string.h>
#include <time.h>
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
    errx(EXIT_FAILURE, "Internal Unknown Error.");
  }
  signal(SIGTERM, c.sigterm_hook);
  signal(SIGINT,  c.sigterm_hook);
  signal(SIGHUP,  c.sigterm_hook);
  signal(SIGCHLD, SIG_IGN);
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

inline const char* log_time() {
  time_t t = time(NULL);
  struct tm *local = localtime(&t);
  static char str[26];
  strftime(str, sizeof(str), "%c", local);
  return str;
}

void action_dispatch(const request_t* request, response_t* response) {
  int action = request->action;
  int user_id;

  if (action == ACTION_NOACTION) {
    noaction(response);
    return;
  }

  if (action & ACTION_REGISTER) {
    register_(response, request->username, request->password);
    return;
  }

  if ((user_id = login(response, request->username, request->password)) < 0)
    return;

}

void noaction(response_t *resp) {
  syslog(LOG_INFO, "%s - Client noaction.", log_time());
  resp->code = r_failure;
  strcpy(resp->content, "No action specified.");
}

void register_(response_t* resp, const char* username, const char* password) {

}

int login(response_t* resp, const char* username, const char* password) {
  int user_id = -1;
  return user_id;
}