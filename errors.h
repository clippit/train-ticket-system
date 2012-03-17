#ifndef _ERRORS_H
#define _ERRORS_H

#define ERROR_SERVER_MQUEUE_CREATE 10
#define ERROR_CLIENT_MQUEUE_CREATE 11
#define ERROR_SERVER_MQUEUE_GET    12
#define ERROR_CLIENT_MQUEUE_GET    13

void exit_on_error(int errornum);
void exit_on_error_detail(int errornum, char* detail_msg);
#endif