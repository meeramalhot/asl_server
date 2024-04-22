CC = gcc
CFLAGS = -g

all: webserv httphelp

webserv: webserv.c httphelp.h
	$(CC) $(CFLAGS) -lpthread -o webserv webserv.c

httphelp: httphelp.c httphelp.h
	$(CC) $(CFLAGS) -o httphelp httphelp.c
clean:
	rm -f *.o *~