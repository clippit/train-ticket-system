#ifndef _CONTROLLER_SERVER_H
#define _CONTROLLER_SERVER_H

#include <sqlite3.h>
#include "common.h"

struct controller_server_func {
  void (*init)(void);
  void (*listener)(void);
  void (*exit_hook)(void);
  void (*sigterm_hook)(int);
} c;

void register_running_mode(int mode);
void server_run();

void action_dispatch(const request_t*, response_t*);
void noaction(response_t*);
int  register_(sqlite3*, response_t*, const char*, const char*);
int  login(sqlite3*, response_t*, const char*, const char*);
void search(sqlite3*, response_t*, const char*, const char*, const char*);
void take_order(sqlite3*, response_t*, const int, const char*, const unsigned short int);
void view(sqlite3*, response_t*, const int);

int  _do_register(sqlite3*, const char*, const char*);
const char* _encrypt_password(const char*);
void _order_failed(sqlite3*, response_t*, sqlite3_stmt*, sqlite3_stmt*, sqlite3_stmt*, const char*);
void _generate_order(response_t*, const int, const char*, const char*, const char*, const char*, const char*, const int, const int, const char*);

#endif