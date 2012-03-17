#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "common.h"
#include "errors.h"

int main() {
	mqueue_server_start();
	mqueue_server_end();
	exit(EXIT_SUCCESS);
}