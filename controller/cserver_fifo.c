#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <syslog.h>
#include "cserver_fifo.h"

void fs_init(void) {
  #ifdef DEBUG
  printf("%s\n", "---fs_init...");
  #endif
  puts("Offline Mode, using FIFOs(named pipes)");
}

void fs_listener(void) {
  while(1) {
    syslog(LOG_INFO, "server running.....");
    sleep(10);
  }
}

void fs_cleanup(void) {
  #ifdef DEBUG
  syslog(LOG_INFO, "fs_cleanup");
  #endif
}

void fs_sigterm(int sig) {
  #ifdef DEBUG
  syslog(LOG_INFO, "receive signal %d", sig);
  #endif
  exit(EXIT_SUCCESS);
}
