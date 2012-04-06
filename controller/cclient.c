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
    c.register_ = fc_register;
    c.login     = fc_login;
    c.search    = fc_search;
    c.order     = fc_order;
    c.view      = fc_view;
    c.refund    = fc_refund;
    c.exit_hook = fc_cleanup;
  } else {
    err(EXIT_FAILURE, "Internal Unknown Error.");
  }
  atexit(c.exit_hook);
}

void client_run(user* current_user, client_options* options) {
  c.init();
}