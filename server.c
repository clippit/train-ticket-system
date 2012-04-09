#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <err.h>
#include "common.h"
#include "controller/cserver.h"

void display_usage() {
  puts( "Train Ticket System Simulation -- Server" );
  puts( "" );
  puts( "Usage:" );
  puts( "  ticketd [MODE]" );
  puts( "Two Modes:" );
  puts( "-m, --offline           Client and Server are running locally, default mode.");
  puts( "-M, --online            Client and Server are running separately in network.");
}

int main(int argc, char **argv) {
  short int mode = MODE_OFFLINE;

  static const struct option longopts[] = {
    {"help", no_argument, NULL, 'h' },
    {"offline", no_argument, NULL, 'm' },
    {"online", no_argument, NULL, 'M' }
  };
  static const char *shortopts = "hmM";
  int opt = 0;

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
      break;
    default: // invalid argument
      break;
    }
  }

  #if DEBUG
  printf("mode: %d\n", mode);
  #endif

  puts( "Train Ticket System Simulation -- Server" );
  register_running_mode(mode);
  server_run();

  exit(EXIT_SUCCESS);
}