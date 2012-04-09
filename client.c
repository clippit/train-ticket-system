#include <stdlib.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <unistd.h>
#include <err.h>
#include <getopt.h>
#include <string.h>
#include "common.h"
#include "socket.h"
#include "controller/cclient.h"

request_t payload = {0, -1, "\0", "\0", "\0", "\0", "\0", 0, 1};
char hostname[HOSTNAME_MAX_LENTH + 1] = "\0";

void display_usage() {
  puts( "Train Ticket System Simulation -- Client" );
  puts( "" );
  puts( "Usage:" );
  puts( "  ticket [MODE] [-u USERNAME] [-p PASSWORD] [-r|-s|-o|-v|-f] [OPTIONS ...]" );
  puts( "Two Modes:" );
  puts( "-m, --offline           Client and Server are running locally, default mode.");
  puts( "-M, --online HOSTNAME   Client and Server are running separately in network.");
  puts( "                        You need to specify server hostname.");
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
  puts( "  -A, --amount AMOUNT   How many tickets you want to order. Default is 1." );
  //puts( "" );
  puts( "View Action:" );
  puts( "  -v, --view            List your orders.");
  //puts( "" );
  puts( "Refund Action:" );
  puts( "  -f, --refund          You need provide order info via the option below.");
  puts( "  -O, --orderid ID      Order ID" );

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
       __fpurge is nonstandard and not portable. It introduced in glibc since 2.1.95. */
    __fpurge(stdin);
  }
  read_cli_string_option("Username", username, payload.username, USERNAME_MAX_LENGTH);
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
      read_cli_string_option("Password", password, payload.password, PASSWORD_MAX_LENGTH);
      success_flag = 1;
    }
  }
}

void repeat_password() {
  char *password2 = NULL;
  const char prompt[] = "Repeat Password: ";
  password2 = getpass(prompt);
  while (strcmp(password2, payload.password) != 0) {
    warnx("Two passwords are not the same. Please try again.");
    prompt_password();
    password2 = getpass(prompt);
  }
}

int main(int argc, char **argv) {
  /* Initialize environment. However `payload` is global */
  short int mode = MODE_OFFLINE;
  int action = ACTION_NOACTION;

  /* Start parsing arguments */
  static const struct option longopts[] = {
    {"help", no_argument, NULL, 'h' },
    {"offline", no_argument, NULL, 'm' },
    {"online", required_argument, NULL, 'M' },
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
    {"amount", required_argument, NULL, 'A'},
    {"orderid", required_argument, NULL, 'O'},
    {NULL, no_argument, NULL, 0}
  };
  static const char *shortopts = "hmM:u:p:rsovfN:F:T:A:O:";
  int opt = 0;
  long int amount = 1;

  while ( (opt = getopt_long(argc, argv, shortopts, longopts, NULL)) != -1) {
    switch(opt) {
    case 'h':
      display_usage();
      break;
    case 'm':
      mode = MODE_OFFLINE;
      break;
    case 'M':
      mode = MODE_ONLINE;
      read_cli_string_option("Hostname", optarg, hostname, HOSTNAME_MAX_LENTH);
      break;
    case 'u':
      if (optarg) read_cli_string_option("Username", optarg, payload.username, USERNAME_MAX_LENGTH);
      break;
    case 'p':
      if (optarg) read_cli_string_option("Password", optarg, payload.password, PASSWORD_MAX_LENGTH);
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
      read_cli_string_option("Train No.", optarg, payload.name, TRAIN_NUMBER_MAX_LENGTH);
      break;
    case 'F':
      read_cli_string_option("Originating station", optarg, payload.from, TRAIN_STATION_MAX_LENGTH);
      break;
    case 'T':
      read_cli_string_option("Terminal station", optarg, payload.to, TRAIN_STATION_MAX_LENGTH);
      break;
    case 'A':
      amount = strtoul(optarg, (char **)NULL, 10);
      if (amount < 1) errx(EXIT_FAILURE, "Invalid negative number.");
      if (amount > MAX_AMOUNT_PER_ORDER)
        errx(EXIT_FAILURE, "Your can only order less than %d tickets one time.", MAX_AMOUNT_PER_ORDER);
      payload.amount = amount;
      break;
    case 'O':
      payload.order_id = strtoul(optarg, (char **)NULL, 10);
      if (payload.order_id <= 0) errx(EXIT_FAILURE, "Invalid Order ID.");
      /* We assume the Order ID should not overflow long int, no errno checking here. */
      break;
    default: // invalid argument
      break;
    }
  }

  /* If user does not provide username or password */
  while ( strlen(payload.username) <= 0) {
    prompt_username();
  }
  while ( strlen(payload.password) <= 0 ) {
    prompt_password();
  }

  if (action & ACTION_REGISTER) {
    repeat_password();
  }

  payload.client_pid = getpid();
  payload.action     = action;

  #if DEBUG
  printf("---username:%s  length:%zu\n---password:%s  length:%zu\n",
    payload.username, strlen(payload.username),
    payload.password, strlen(payload.password));
  printf("---action:%d\n", payload.action);
  printf("---client_pid:%d\n", payload.client_pid);
  printf("---options: name:%s,  from:%s,  to:%s,  amount: %d,  order_id:%ld\n",
    payload.name, payload.from, payload.to, payload.amount, payload.order_id);
  #endif

  /* Interactive interface end here. Client Controller on stage. */
  register_running_mode(mode);
  client_run(&payload);


  exit(EXIT_SUCCESS);
}