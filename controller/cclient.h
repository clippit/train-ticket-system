#ifndef _CONTROLLER_CLIENT_H
#define _CONTROLLER_CLIENT_H

#include "common.h"

typedef struct client_options_t {
  char name [TRAIN_NUMBER_MAX_LENGTH + 1];
  char from[TRAIN_STATION_MAX_LENGTH + 1];
  char to  [TRAIN_STATION_MAX_LENGTH + 1];
  unsigned long int order_id;
  unsigned short int amount;
} client_options;

struct controller_client_func {
  int  (*init)(void);
  int  (*register_)(user*); // do whatever you want to fuck off the c keyword `register`
  int  (*login)(user*);
  int  (*search)(client_options*);
  int  (*order)(client_options*);
  int  (*view)(order*);
  int  (*refund)(unsigned long int);
  void (*exit_hook)(void);
} c;

void register_running_mode(int mode);
void client_run(user* current_user, client_options* options);

#endif
