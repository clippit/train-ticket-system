#include <stdlib.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <unistd.h>
#include <err.h>
#include <getopt.h>
#include <string.h>
#include "common.h"

user current_user = {-1, "\0", "\0", -1};

void display_usage() {
  puts("Train Ticket System Simulation -- Client");
  exit(EXIT_SUCCESS);
}

void input_username(char *input) {
  if (strlen(input) > USERNAME_MAX_LENGTH) {
    errx(EXIT_FAILURE, "Username cannot be longer than %d characters.", USERNAME_MAX_LENGTH);
  } else {
    strcpy(current_user.username, input);
  }
}

void prompt_username() {
  size_t buffer = sizeof(((user *)0)->username) << 1; 
  #if DEBUG
  printf("-----username input buffer: %d\n", (int)buffer);
  #endif
  char *username = (char*)malloc(buffer);
  int success_flag = 0;
  while (!success_flag) {
    printf("Username: ");
    if ( fgets(username, buffer, stdin) == NULL )
      err(EXIT_FAILURE, "Cannot read input.");
    #if DEBUG
    printf("-----username strlen: %d\n", (int)strlen(username));
    #endif
    size_t last = strlen(username) - 1;
    if (username[last] == '\n') 
      username[last] = '\0';
    if (strlen(username) < 1 || strlen(username) > USERNAME_MAX_LENGTH) 
      warnx("Username cannot be less than 1 character or longer than %d characters.", USERNAME_MAX_LENGTH);
    else 
      success_flag = 1;
    /* flush stdin, if input is longer than buffer, the next fgets will not prompt user.
       __fpurge is nonstandard and not portable. It introduced in glic since 2.1.95. */
    __fpurge(stdin);
  }
  input_username(username);
  free(username);
}

void input_password(char *input) {
  if (strlen(input) > PASSWORD_MAX_LENGTH) {
    errx(EXIT_FAILURE, "Password cannot be longer than %d characters.", USERNAME_MAX_LENGTH);
  } else {
    strcpy(current_user.password, input);
  }
}

int main(int argc, char **argv) {

  static const struct option longopts[] = {
    {"help", no_argument, NULL, 'h' },
    {"register", no_argument, NULL, 'r'},
    {"username", required_argument, NULL, 'u'},
    {"password", optional_argument, NULL, 'p'},
    {NULL, no_argument, NULL, 0}
  };
  static const char *shortopts = "hp:ru:";
  int opt = 0;

  while ( (opt = getopt_long(argc, argv, shortopts, longopts, NULL)) != -1) {
    switch(opt) {
    case 'h':
      display_usage();
      break;
    case 'u':
      input_username(optarg);
      break;
    case 'p':
      input_password(optarg);
      break;
    default: // invalid argument
      break;
    }
  }

  if ( strlen(current_user.username) <= 0) {
    prompt_username();
  }
  // printf("username:%s  length:%d\npassword:%s\n", current_user.username, strlen(current_user.username), current_user.password );
  // prompt_username();
  // mqueue_client_start();
  // user u = {1, "test", "pass", 0};

  // int i;
  // for (i = 0; i < 5; ++i)
  // {
  //  mqueue_client_send(u);

  //  sleep(5);
  // }
  // mqueue_client_end();
  exit(EXIT_SUCCESS);
}