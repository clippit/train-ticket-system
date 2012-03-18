#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "common.h"

int main() {
	mqueue_server_start();
	while(mqueue_server_get()) {
	}


	mqueue_server_end();
	exit(EXIT_SUCCESS);
}