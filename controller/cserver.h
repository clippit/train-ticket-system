#ifndef _CONTROLLER_SERVER_H
#define _CONTROLLER_SERVER_H

#include <sqlite3.h>
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

void action_dispatch(const request_t*, response_t*);
void noaction(response_t*);
int  register_(sqlite3*, response_t*, const char*, const char*);
int  _do_register(sqlite3*, const char*, const char*);
int  login(sqlite3*, response_t*, const char*, const char*);

#endif