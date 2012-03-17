#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "common.h"

int main() {
	mqueue_client_start();
	mqueue_client_end();
	exit(EXIT_SUCCESS);
}