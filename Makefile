CC=gcc
CFLAGS=-Wall
DEBUG=1
ifneq ($(DEBUG),)
	CFLAGS += -g -DDEBUG
else
	CFLAGS += -O2
endif
OBJS_COMMON=errors.o common.o
OBJS_SERVER=server.o
OBJS_CLIENT=client.o
TARGET_SERVER=ticketd
TARGET_CLIENT=ticket

.PHONY : all client server clean $(OBJS_COMMON) $(OBJS_SERVER) $(OBJS_CLIENT)

all: server client
	

client: $(OBJS_COMMON) $(OBJS_CLIENT)
	$(CC) $(CFLAGS) $^ -o $(TARGET_CLIENT)

server: $(OBJS_COMMON) $(OBJS_SERVER)
	$(CC) $(CFLAGS) $^ -o $(TARGET_SERVER)

errors.o: errors.c errors.h
	$(CC) $(CFLAGS) -c $<

common.o: common.c common.h
	$(CC) $(CFLAGS) -c $<

server.o: server.c $(OBJS_COMMON)
	$(CC) $(CFLAGS) -c $<

client.o: client.c $(OBJS_COMMON)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS_COMMON) $(OBJS_CLIENT) $(OBJS_SERVER) $(TARGET_SERVER) $(TARGET_CLIENT)