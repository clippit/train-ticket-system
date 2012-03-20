#ifndef _COMMON_TICKET_H
#define _COMMON_TICKET_H

#include <time.h>

/* Running mode
   ----------------------------- */
#define MODE_OFFLINE 1
#define MODE_ONLINE  2

/* Model
   ----------------------------- */
#define USERNAME_MAX_LENGTH      30
#define PASSWORD_MAX_LENGTH      34
#define TRAIN_NUMBER_MAX_LENGTH   5
#define TRAIN_STATION_MAX_LENGTH 20
#define TRAIN_TIME_LENGTH         5
#define MAX_AMOUNT_PER_ORDER     10

typedef struct _user {
  int id;
  char username [USERNAME_MAX_LENGTH + 1];
  char password [PASSWORD_MAX_LENGTH + 1];
  time_t register_time;
} user;

typedef struct _train {
  int id;
  char name [TRAIN_NUMBER_MAX_LENGTH   + 1];
  char start [TRAIN_STATION_MAX_LENGTH + 1];
  char end   [TRAIN_STATION_MAX_LENGTH + 1];
  char start_time [TRAIN_TIME_LENGTH   + 1];
  char end_time   [TRAIN_TIME_LENGTH   + 1];
  int  price;
  int  available;
} train;

typedef struct _order {
  unsigned long int id;
  user user;
  train train;
  unsigned short int amount;
  time_t order_time;
} order;


/* Client Actions
   ----------------------------- */

#define ACTION_NOACTION (0     )
#define ACTION_REGISTER (1 << 0)
#define ACTION_SEARCH   (1 << 1)
#define ACTION_ORDER    (1 << 2)
#define ACTION_VIEW     (1 << 3)
#define ACTION_REFUND   (1 << 4)


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