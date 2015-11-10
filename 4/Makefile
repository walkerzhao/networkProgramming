CC=g++
CFLAGS=-g  

all:  client server

client: client.cpp raw.c
	$(CC) client.cpp raw.c $(CFLAGS) -o client

server: server.cpp
	$(CC)  server.cpp $(CFLAGS) -o server

clean:
	rm server client
