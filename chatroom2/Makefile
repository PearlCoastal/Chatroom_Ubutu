CC=gcc

CFLAGS=-Wall -c



all:server client


server:server.o
	$(CC) $< -o $@


server.o:server.c
	$(CC) $(CFLAGS) $< -o $@


client:client.o
	$(CC) $< -o $@


client.o:client.c
	$(CC) $(CFLAGS) $< -o $@


.PHONY:
	clean


clean:
	$(RM) *.o server client

