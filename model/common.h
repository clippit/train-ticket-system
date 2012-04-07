#ifndef _COMMON_TICKET_H
#define _COMMON_TICKET_H

/* Running mode
   ----------------------------- */
#define MODE_OFFLINE 1
#define MODE_ONLINE  2

/* Model
   ----------------------------- */
#define DATABASE "data.sqlite"

#define USERNAME_MAX_LENGTH      30
#define PASSWORD_MAX_LENGTH      34
#define TRAIN_NUMBER_MAX_LENGTH   5
#define TRAIN_STATION_MAX_LENGTH 20
#define TRAIN_TIME_LENGTH         5
#define MAX_AMOUNT_PER_ORDER     10

typedef struct _user {
  unsigned long int id;
  char username [USERNAME_MAX_LENGTH + 1];
  char password [PASSWORD_MAX_LENGTH + 1];
  time_t register_time;
} user;

typedef struct _train {
  unsigned int id;
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


/* Server Response type
   ----------------------------- */

typedef enum _server_response_e {
  r_success = 0,
  r_failure
} server_response_e;


/* Communication Payload
   ----------------------------- */

#define RESPONSE_BUFFER 1200

typedef struct _request_t {
  pid_t client_pid;
  int   action;
  char  username [USERNAME_MAX_LENGTH + 1];
  char  password [PASSWORD_MAX_LENGTH + 1];
  char  name [TRAIN_NUMBER_MAX_LENGTH + 1];
  char  from[TRAIN_STATION_MAX_LENGTH + 1];
  char  to  [TRAIN_STATION_MAX_LENGTH + 1];
  unsigned long int order_id;
  unsigned short int amount;
} request_t;

typedef struct _response_t {
  server_response_e code;
  char              content[RESPONSE_BUFFER + 1];
} response_t;

#endif