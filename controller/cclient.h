#ifndef _CONTROLLER_CLIENT_H
#define _CONTROLLER_CLIENT_H

#include "common.h"

struct controller_client_func {
  void (*init)(pid_t);
  int  (*request)(request_t*, response_t*);
  void (*exit_hook)(void);
} c;

void register_running_mode(int mode);
void client_run(request_t *payload);

#endif
