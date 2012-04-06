#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include <sys/msg.h>
#include "cclient_fifo.h"
#include "cclient.h"

int fc_init(void) {
  #ifdef DEBUG
  printf("%s\n", "---fc_init...");
  #endif
  return 0;
}

int fc_register(user* u) {
  return 0;
}

int fc_login(user* u) {
  return 0;
}

int fc_search(client_options* opt) {
  return 0;
}

int fc_order(client_options* opt) {
  return 0;
}

int fc_view(order* order_list) {
  return 0;
}

int fc_refund(unsigned long int order_id) {
  return 0;
}

void fc_cleanup(void) {
  #ifdef DEBUG
  printf("%s\n", "---fc_cleanup...");
  #endif
}
