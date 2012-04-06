#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include "cclient.h"
#include "cclient_fifo.h"


void register_running_mode(int mode) {
  if (mode == MODE_ONLINE) {
    // TODO Not implemented yet
  } else if (mode == MODE_OFFLINE) {
    c.init      = fc_init;
    c.request   = fc_request;
    c.exit_hook = fc_cleanup;
  } else {
    err(EXIT_FAILURE, "Internal Unknown Error.");
  }
  atexit(c.exit_hook);
}

void client_run(payload_t *payload) {
  c.init(payload->client_pid);
  c.request(payload);
  puts(payload->response_content);
}