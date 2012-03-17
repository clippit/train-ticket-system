#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "common.h"
#include "errors.h"

static int serv_qid = -1;
static int cli_qid = -1;
static key_t server_mqueue_key;
static key_t client_mqueue_key;

void mqueue_init() {
    server_mqueue_key = ftok(FTOK_PATHNAME, 1);
    client_mqueue_key = ftok(FTOK_PATHNAME, 2);
    if (server_mqueue_key == -1 || client_mqueue_key == -1)
        exit_on_error(ERROR_MESSAGE_QUEUE_KEY);
}

int mqueue_server_start() {
    #if DEBUG
        printf("%d :- server_starting()\n",  getpid());
    #endif

    mqueue_init();
    serv_qid = msgget(server_mqueue_key, 0666 | IPC_CREAT);
    if (serv_qid == -1) exit_on_error(ERROR_SERVER_MQUEUE_CREATE);

    cli_qid = msgget(client_mqueue_key, 0666 | IPC_CREAT);
    if (cli_qid == -1) exit_on_error(ERROR_CLIENT_MQUEUE_CREATE);

    return(1);
}

void mqueue_server_end() {
    #if DEBUG
        printf("%d :- server_ending()\n",  getpid());
    #endif

    (void)msgctl(serv_qid, IPC_RMID, 0);
    (void)msgctl(cli_qid, IPC_RMID, 0);

    serv_qid = -1;
    cli_qid = -1;
}

int mqueue_client_start() {
    #if DEBUG
        printf("%d :- client_starting()\n",  getpid());
    #endif

    mqueue_init();
    serv_qid = msgget((key_t)server_mqueue_key, 0666);
    if (serv_qid == -1) exit_on_error(ERROR_SERVER_MQUEUE_GET);

    cli_qid = msgget((key_t)client_mqueue_key, 0666);
    if (cli_qid == -1) exit_on_error(ERROR_CLIENT_MQUEUE_GET);

    return(1);
}

void mqueue_client_end() {
    #if DEBUG
        printf("%d :- client_ending()\n",  getpid());
    #endif

    serv_qid = -1;
    cli_qid = -1;
}