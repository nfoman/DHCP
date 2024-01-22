CC = gcc
CFLAGS = -Wall

server: server.c dhcp.h
	$(CC) $(CFLAGS) -o server server.c

client: client.c dhcp.h
	$(CC) $(CFLAGS) -o client client.c

.PHONY: clean
clean:
	rm -f server client