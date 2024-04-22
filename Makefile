CC = gcc
CFLAGS = -g

all: webserv

webserv: webserv.c httphelp.h
	$(CC) $(CFLAGS) -lpthread -o webserv webserv.c httphelp.c

clean:
	rm -f *.o *~ webserv