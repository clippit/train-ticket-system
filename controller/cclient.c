#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include "cclient.h"
#include "cclient_fifo.h"
#include "cclient_socket.h"


void register_running_mode(int mode) {
  if (mode == MODE_ONLINE) {
    c.init      = sc_init;
    c.request   = sc_request;
    c.exit_hook = sc_cleanup;
  } else if (mode == MODE_OFFLINE) {
    c.init      = fc_init;
    c.request   = fc_request;
    c.exit_hook = fc_cleanup;
  } else {
    errx(EXIT_FAILURE, "Internal Unknown Error.");
  }
  atexit(c.exit_hook);
}

void client_run(request_t *payload) {
  response_t response = {0, "\0"};
  c.init(payload->client_pid);
  if (c.request(payload, &response)) {
    puts(response.content);
    puts(response.code == r_success ? "Operation Completed." : "Operation Failed.");
  } else {
    errx(EXIT_FAILURE, "Internal Error. Abort!");
  }
}