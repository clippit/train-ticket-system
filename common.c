#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <err.h>
#include "common.h"

static int serv_qid = -1;
static int cli_qid = -1;
static key_t server_mqueue_key;
static key_t client_mqueue_key;

void mqueue_init() {
  server_mqueue_key = ftok(FTOK_PATHNAME, 1);
  client_mqueue_key = ftok(FTOK_PATHNAME, 2);
  if (server_mqueue_key == -1 || client_mqueue_key == -1)
    err(EXIT_FAILURE, "Cannot get Message Queue key. Is there a data.sqlite file?");
}

int mqueue_server_start() {
  #if DEBUG
    printf("%d :- server_start()\n",  getpid());
  #endif

  mqueue_init();
  serv_qid = msgget(server_mqueue_key, 0666 | IPC_CREAT);
  if (serv_qid == -1) err(EXIT_FAILURE, "Cannot create server message queue.");

  cli_qid = msgget(client_mqueue_key, 0666 | IPC_CREAT);
  if (cli_qid == -1) err(EXIT_FAILURE, "Cannot create client message queue.");

  return(1);
}

void mqueue_server_end() {
  #if DEBUG
    printf("%d :- server_end()\n",  getpid());
  #endif

  (void)msgctl(serv_qid, IPC_RMID, 0);
  (void)msgctl(cli_qid, IPC_RMID, 0);

  serv_qid = -1;
  cli_qid = -1;
}

int mqueue_server_get() {
  user_message user_buffer;
  if (msgrcv(serv_qid, (void *)&user_buffer, sizeof(user_message)-sizeof(long int), 0, 0) == -1)
    err(EXIT_FAILURE, "msgrcv fault.");
  printf("%s\n", user_buffer.user_info.username);
  return 1;
}

int mqueue_client_start() {
  #if DEBUG
    printf("%d :- client_starting()\n",  getpid());
  #endif

  mqueue_init();
  serv_qid = msgget((key_t)server_mqueue_key, 0666);
  if (serv_qid == -1) err(EXIT_FAILURE, "Cannot access server message queue. Is the server running?");

  cli_qid = msgget((key_t)client_mqueue_key, 0666);
  if (cli_qid == -1) err(EXIT_FAILURE, "Cannot access client message queue. Is the server running?");

  return(1);
}

void mqueue_client_end() {
  #if DEBUG
    printf("%d :- client_ending()\n",  getpid());
  #endif

  serv_qid = -1;
  cli_qid = -1;
}

int mqueue_client_send(user puser) {
  user_message m = {1, puser, 0};
  if (msgsnd(serv_qid, (void *)&m, sizeof(user_message)-sizeof(long int), 0 ) == -1)
    err(EXIT_FAILURE, "msgsnd fault.");
  return 1;
}