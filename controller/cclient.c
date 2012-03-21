#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include "cclient.h"
#include "cclient_mqueue.h"


void register_running_mode(int mode) {
  if (mode == MODE_ONLINE) {
    // TODO Not implemented yet
  } else if (mode == MODE_OFFLINE) {
    c.init      = mqc_init;
    c.register_ = mqc_register;
    c.login     = mqc_login;
    c.search    = mqc_search;
    c.order     = mqc_order;
    c.view      = mqc_view;
    c.refund    = mqc_refund;
    c.exit_hook = mqc_cleanup;
  } else {
    err(EXIT_FAILURE, "Internal Unknown Error.");
  }
  atexit(c.exit_hook);
}

void client_run(user* current_user, client_options* options) {

}