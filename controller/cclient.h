#ifndef _CONTROLLER_CLIENT_H
#define _CONTROLLER_CLIENT_H

#include "common.h"

struct controller_client_func {
  void (*init)(pid_t);
  void (*request)(payload_t*);
  void (*exit_hook)(void);
} c;

void register_running_mode(int mode);
void client_run(payload_t *payload);

#endif
