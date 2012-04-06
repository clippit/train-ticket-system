CC=gcc
MODEL_DIR=model
CONTROLLER_DIR=controller
CFLAGS=-Wall -I$(MODEL_DIR)
DEBUG=1
ifneq ($(DEBUG),)
	CFLAGS += -g -DDEBUG
else
	CFLAGS += -O2
endif
OBJS_SERVER=server.o cserver.o cserver_fifo.o
OBJS_CLIENT=client.o cclient.o cclient_fifo.o
TARGET_SERVER=ticketd
TARGET_CLIENT=ticket

.PHONY : all client server clean $(OBJS_SERVER) $(OBJS_CLIENT)

all: server client
	

client: $(OBJS_CLIENT)
	$(CC) $(CFLAGS) $^ -o $(TARGET_CLIENT)

server: $(OBJS_SERVER)
	$(CC) $(CFLAGS) $^ -o $(TARGET_SERVER)

server.o: server.c
	$(CC) $(CFLAGS) -c $<

cserver.o: $(CONTROLLER_DIR)/cserver.c
	$(CC) $(CFLAGS) -c $<

cserver_fifo.o: $(CONTROLLER_DIR)/cserver_fifo.c
	$(CC) $(CFLAGS) -c $<

client.o: client.c
	$(CC) $(CFLAGS) -c $<

cclient.o: $(CONTROLLER_DIR)/cclient.c
	$(CC) $(CFLAGS) -c $<

cclient_fifo.o: $(CONTROLLER_DIR)/cclient_fifo.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS_CLIENT) $(OBJS_SERVER) $(TARGET_SERVER) $(TARGET_CLIENT) *~