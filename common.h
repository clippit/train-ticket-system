#ifndef _COMMON_TICKET_H
#define _COMMON_TICKET_H

#include <time.h>


/* Model
   ----------------------------- */
#define USERNAME_MAX_LENGTH    30
#define PASSWORD_MAX_LENGTH    34
#define TRAIN_NAME_MAX_LENGTH   5
#define TRAIN_PLACE_MAX_LENGTH 20
#define TRAIN_TIME_LENGTH       5

typedef struct _user {
  int id;
  char username [USERNAME_MAX_LENGTH + 1];
  char password [PASSWORD_MAX_LENGTH + 1];
  time_t register_time;
} user;

typedef struct _train {
  int id;
  char name  [TRAIN_NAME_MAX_LENGTH  + 1];
  char start [TRAIN_PLACE_MAX_LENGTH + 1];
  char end   [TRAIN_PLACE_MAX_LENGTH + 1];
  char start_time [TRAIN_TIME_LENGTH + 1];
  char end_time   [TRAIN_TIME_LENGTH + 1];
  int  price;
  int  available;
} train;

typedef struct _order {
  int id;
  user user;
  train train;
  int amount;
  time_t order_time;
} order;

/* Message Queue
   ----------------------------- */

typedef struct _user_message {
  long int message_type;
  user user_info;
  int action;
} user_message;

typedef struct _train_message {
  long int message_type;
  train train_info;
} train_message;

typedef struct _order_message {
  long int message_type;
  order order_info;
  int action;
} order_message;

#define FTOK_PATHNAME "data.sqlite"

int mqueue_server_start();
void mqueue_server_end();
int mqueue_client_start();
void mqueue_client_end();
int mqueue_server_get();
int mqueue_client_send();
#endif