
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
