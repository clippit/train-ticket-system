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
  puts( "Train Ticket System Simulation -- Client" );
  puts( "" );
  puts( "Usage:" );
  puts( "  ticket [-u USERNAME] [-p PASSWORD] [-r|-s|-o|-v|-f] [OPTIONS ...]" );
  puts( "" );
  puts( "Login Action:" );
  puts( "  -u, --username USERNAME" );
  puts( "  -p, --password PASSWORD" );
  //puts( "" );
  puts( "Register Action:" );
  puts( "  -r, --register        Provide above login details for registeration. You need to");
  puts( "                        confirm your password by typing it twice." );
  //puts( "" );
  puts( "Search Action:" );
  puts( "  -s, --search          You need provide search criteria via the options below.");
  puts( "  -N, --number NUMBER   Train No." );
  puts( "  -F, --from STATION    Originating station");
  puts( "  -T, --to STATION      Terminal station");
  //puts( "" );
  puts( "Order Action:" );
  puts( "  -o, --order           You need provide order criteria via the option below.");
  puts( "  -N, --number NUMBER   Train No." );
  //puts( "" );
  puts( "View Action:" );
  puts( "  -v, --view            List your orders.");
  //puts( "" );
  puts( "Refund Action:" );
  puts( "  -f, --refund          You need provide order info via the option below.");
  puts( "  -O, --order ID        Order ID" );

  exit(EXIT_SUCCESS);
}

void read_cli_string_option(const char *name, const char *input_value, char* target, const int max_legnth) {
  if (!input_value)
    errx(EXIT_FAILURE, "%s is required.", name);
  if (strlen(input_value) > max_legnth) {
    errx(EXIT_FAILURE, "%s cannot be longer than %d characters.", name, max_legnth);
  } else {
    strcpy(target, input_value);
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
  read_cli_string_option("Username", username, current_user.username, USERNAME_MAX_LENGTH);
  free(username);
}

void prompt_password() {
  int success_flag = 0;
  char *password = NULL;
  while (!success_flag) {
    password = getpass("Password: ");
    if (strlen(password) < 1 || strlen(password) > PASSWORD_MAX_LENGTH) {
      warnx("Password cannot be less than 1 character or longer than %d characters.", PASSWORD_MAX_LENGTH);
    } else {
      read_cli_string_option("Password", password, current_user.password, PASSWORD_MAX_LENGTH);
      success_flag = 1;
    }
  }
}

void repeat_password() {
  char *password2 = NULL;
  const char prompt[] = "Repeat Password: ";
  password2 = getpass(prompt);
  while (strcmp(password2, current_user.password) != 0) {
    warnx("Two passwords are not the same. Please try again.");
    prompt_password();
    printf("%s\n", current_user.password);
    password2 = getpass(prompt);
  }
}

int main(int argc, char **argv) {
  int action = ACTION_NOACTION;
  struct options_t {
    char name [TRAIN_NUMBER_MAX_LENGTH + 1];
    char from[TRAIN_STATION_MAX_LENGTH + 1];
    char to  [TRAIN_STATION_MAX_LENGTH + 1];
    long int order_id;
  } options = {"\0", "\0", "\0", -1};

  static const struct option longopts[] = {
    {"help", no_argument, NULL, 'h' },
    {"username", optional_argument, NULL, 'u'},
    {"password", optional_argument, NULL, 'p'},
    {"register", no_argument, NULL, 'r'},
    {"search", no_argument, NULL, 's'},
    {"order", no_argument, NULL, 'o'},
    {"view", no_argument, NULL, 'v'},
    {"refund", no_argument, NULL, 'f'},
    {"number", required_argument, NULL, 'N'},
    {"from", required_argument, NULL, 'F'},
    {"to", required_argument, NULL, 'T'},
    {"order", required_argument, NULL, 'O'},
    {NULL, no_argument, NULL, 0}
  };
  static const char *shortopts = "hu:p:rsovfN:F:T:O:";
  int opt = 0;

  while ( (opt = getopt_long(argc, argv, shortopts, longopts, NULL)) != -1) {
    switch(opt) {
    case 'h':
      display_usage();
      break;
    case 'u':
      if (optarg) read_cli_string_option("Username", optarg, current_user.username, USERNAME_MAX_LENGTH);
      break;
    case 'p':
      if (optarg) read_cli_string_option("Password", optarg, current_user.password, PASSWORD_MAX_LENGTH);
      break;
    case 'r':
      action |= ACTION_REGISTER;
      break;
    case 's':
      action |= ACTION_SEARCH;
      break;
    case 'o':
      action |= ACTION_ORDER;
      break;
    case 'v':
      action |= ACTION_VIEW;
      break;
    case 'f':
      action |= ACTION_REFUND;
      break;
    case 'N':
      read_cli_string_option("Train No.", optarg, options.name, TRAIN_NUMBER_MAX_LENGTH);
      break;
    case 'F':
      read_cli_string_option("Originating station", optarg, options.from, TRAIN_STATION_MAX_LENGTH);
      break;
    case 'T':
      read_cli_string_option("Terminal station", optarg, options.to, TRAIN_STATION_MAX_LENGTH);
      break;
    case 'O':
      options.order_id = strtol(optarg, (char **)NULL, 10);
      if (options.order_id <= 0) errx(EXIT_FAILURE, "Invalid Order ID.");
      /* We assume the Order ID should not overflow long int, no errno checking here. */
      break;
    default: // invalid argument
      break;
    }
  }

  while ( strlen(current_user.username) <= 0) {
    prompt_username();
  }
  while ( strlen(current_user.password) <= 0 ) {
    prompt_password();
  }
  #if DEBUG
  printf("---username:%s  length:%zu\n---password:%s  length:%zu\n",
    current_user.username, strlen(current_user.username),
    current_user.password, strlen(current_user.password));
  printf("---action:%d\n", action);
  printf("---options: name:%s,  from:%s,  to:%s,  order_id:%ld\n",
    options.name, options.from, options.to, options.order_id);
  #endif

  if (action & ACTION_REGISTER) 
    repeat_password();


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