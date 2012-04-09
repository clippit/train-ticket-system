CC=gcc
MODEL_DIR=model
CONTROLLER_DIR=controller
CFLAGS=-Wall -I$(MODEL_DIR)
#DEBUG=1
ifneq ($(DEBUG),)
	CFLAGS += -g -DDEBUG
else
	CFLAGS += -O2
endif
OBJS_COMMON=socket.o
OBJS_SERVER=server.o cserver.o cserver_fifo.o cserver_socket.o
OBJS_CLIENT=client.o cclient.o cclient_fifo.o cclient_socket.o
TARGET_SERVER=ticketd
TARGET_CLIENT=ticket

.PHONY : all client server clean $(OBJS_SERVER) $(OBJS_CLIENT) stop

all: server client
	

socket.o: $(MODEL_DIR)/socket.c
	$(CC) $(CFLAGS) -c $<

client: $(OBJS_CLIENT) $(OBJS_COMMON)
	$(CC) $(CFLAGS) $^ -o $(TARGET_CLIENT)

server: $(OBJS_SERVER) $(OBJS_COMMON)
	$(CC) $(CFLAGS) -lsqlite3 -lcrypt $^ -o $(TARGET_SERVER)

server.o: server.c
	$(CC) $(CFLAGS) -c $<

cserver.o: $(CONTROLLER_DIR)/cserver.c
	$(CC) $(CFLAGS) -c $<

cserver_fifo.o: $(CONTROLLER_DIR)/cserver_fifo.c
	$(CC) $(CFLAGS) -c $<

cserver_socket.o: $(CONTROLLER_DIR)/cserver_socket.c
	$(CC) $(CFLAGS) -c $<

client.o: client.c
	$(CC) $(CFLAGS) -c $<

cclient.o: $(CONTROLLER_DIR)/cclient.c
	$(CC) $(CFLAGS) -c $<

cclient_fifo.o: $(CONTROLLER_DIR)/cclient_fifo.c
	$(CC) $(CFLAGS) -c $<

cclient_socket.o: $(CONTROLLER_DIR)/cclient_socket.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS_CLIENT) $(OBJS_SERVER) $(TARGET_SERVER) $(TARGET_CLIENT) *~ /tmp/client_*_pipe /tmp/server_pipe

stop:
	killall -v ticketd