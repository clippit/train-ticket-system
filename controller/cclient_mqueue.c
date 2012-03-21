#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include <sys/msg.h>
#include "cclient_mqueue.h"
#include "cclient.h"

int mqc_init(void) {
  return 0;
}

int mqc_register(user* u) {
  return 0;
}

int mqc_login(user* u) {
  return 0;
}

int mqc_search(client_options* opt) {
  return 0;
}

int mqc_order(client_options* opt) {
  return 0;
}

int mqc_view(order* order_list) {
  return 0;
}

int mqc_refund(unsigned long int order_id) {
  return 0;
}

void mqc_cleanup(void) {
  #ifdef DEBUG
  printf("%s\n", "---mqc_cleanup...");
  #endif
}
