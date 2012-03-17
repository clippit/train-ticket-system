#include <stdlib.h>
#include <stdio.h>
#include "errors.h"

char * get_error_msg(int errornum) {
	char *error_msg;
	switch (errornum) {
	case ERROR_SERVER_MQUEUE_CREATE:
		error_msg = "Create server message queue failed.";
		break;
	case ERROR_CLIENT_MQUEUE_CREATE:
		error_msg = "Create client message queue failed.";
		break;
	case ERROR_SERVER_MQUEUE_GET:
		error_msg = "Cannot access server message queue. Is the server running?";
		break;
	case ERROR_CLIENT_MQUEUE_GET:
		error_msg = "Cannot access client message queue. Is the server running?";
		break;
	default:
		error_msg = "Unknown Internal Error.";
	}
	return error_msg;
}

void exit_on_error(int errornum) {
	fprintf(stderr, "%s (Error Code: %d)\n", get_error_msg(errornum), errornum);
	exit(errornum);
}

void exit_on_error_detail(int errornum,  char* detail_msg) {
	fprintf(stderr, "%s (Error Code: %d)\nDetails: %s\n", get_error_msg(errornum), errornum, detail_msg);
	exit(errornum);
}