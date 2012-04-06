#ifndef _CONTROLLER_SERVER_H
#define _CONTROLLER_SERVER_H

#include "common.h"

struct controller_server_func {
  void (*init)(void);
  void (*listener)(void);
  /*int  (*register_)(user*); // do whatever you want to fuck off the c keyword `register`
  int  (*login)(user*);
  int  (*search)(client_options*);
  int  (*order)(client_options*);
  int  (*view)(order*);
  int  (*refund)(unsigned long int);*/
  void (*exit_hook)(void);
  void (*sigterm_hook)(int);
} c;

void register_running_mode(int mode);
void server_run();

#endif