#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include <string.h>
#include "cclient_fifo.h"
#include "cclient.h"

void fc_init(void) {
  #ifdef DEBUG
  printf("%s\n", "---fc_init...");
  #endif
}

void fc_request(payload_t *payload) {
  strcpy(payload->response_content, "Hello");
}

void fc_cleanup(void) {
  #ifdef DEBUG
  printf("%s\n", "---fc_cleanup...");
  #endif
}
