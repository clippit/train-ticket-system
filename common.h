#ifndef _COMMON_TICKET_H
#define _COMMON_TICKET_H

#include <time.h>


/* Model
   ----------------------------- */
typedef struct _user {
	int id;
	char *username;
	char *password;
	time_t register_time;
} user;

typedef struct _train {
	int id;
	char *name;
	char *start;
	char *end;
	char *start_time;
	char *end_time;
	int price;
	int available;
} train;

typedef struct _order {
	int id;
	user *user;
	train *train;
	int amount;
	time_t order_time;
} order;

/* Message Queue
   ----------------------------- */
typedef struct _user_message {
	long int message_type;
	user* user_info;
} user_message;

typedef struct _train_message {
	long int message_type;
	train* train_info;
} train_message;

typedef struct _order_message {
	long int message_type;
	order* order_info;
} order_message;

#define SERVER_MQUEUE 12306
#define CLIENT_MQUEUE 12307

int server_starting();
void server_ending();
int client_starting();
void client_ending();
#endif