#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "common.h"
#include "errors.h"

int main() {
	server_starting();
	server_ending();
	exit(EXIT_SUCCESS);
}