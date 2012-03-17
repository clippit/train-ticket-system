#ifndef _ERRORS_H
#define _ERRORS_H

#define ERROR_MESSAGE_QUEUE_KEY    11
#define ERROR_SERVER_MQUEUE_CREATE 12
#define ERROR_CLIENT_MQUEUE_CREATE 13
#define ERROR_SERVER_MQUEUE_GET    14
#define ERROR_CLIENT_MQUEUE_GET    15

void exit_on_error(int errornum);
void exit_on_error_detail(int errornum, char* detail_msg);
#endif