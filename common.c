#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/msg.h>
#include "common.h"
#include "errors.h"

static int serv_qid = -1;
static int cli_qid = -1;

int server_starting() {
    #if DEBUG
        printf("%d :- server_starting()\n",  getpid());
    #endif

    serv_qid = msgget((key_t)SERVER_MQUEUE, 0666 | IPC_CREAT);
    if (serv_qid == -1) exit_on_error(ERROR_SERVER_MQUEUE_CREATE);

    cli_qid = msgget((key_t)CLIENT_MQUEUE, 0666 | IPC_CREAT);
    if (cli_qid == -1) exit_on_error(ERROR_CLIENT_MQUEUE_CREATE);

    return(1);
}

void server_ending() {
    #if DEBUG
        printf("%d :- server_ending()\n",  getpid());
    #endif

    (void)msgctl(serv_qid, IPC_RMID, 0);
    (void)msgctl(cli_qid, IPC_RMID, 0);

    serv_qid = -1;
    cli_qid = -1;
}

int client_starting() {
    #if DEBUG
        printf("%d :- client_starting()\n",  getpid());
    #endif

    serv_qid = msgget((key_t)SERVER_MQUEUE, 0666);
    if (serv_qid == -1) exit_on_error(ERROR_SERVER_MQUEUE_GET);

    cli_qid = msgget((key_t)CLIENT_MQUEUE, 0666);
    if (cli_qid == -1) exit_on_error(ERROR_CLIENT_MQUEUE_GET);

    return(1);
}

void client_ending() {
    #if DEBUG
        printf("%d :- client_ending()\n",  getpid());
    #endif

    serv_qid = -1;
    cli_qid = -1;
}