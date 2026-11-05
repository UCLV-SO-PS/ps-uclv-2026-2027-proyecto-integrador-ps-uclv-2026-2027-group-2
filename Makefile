CC     = gcc
CFLAGS = -Wall -Wextra

all: libmonitor.a server client


monitor.o : monitor.c monitor.h
	$(CC) $(CFLAGS) -c monitor.c -o monitor.o

libmonitor.a: monitor.o
	ar rcs libmonitor.a monitor.o

server: server.c libmonitor.a
	$(CC) $(CFLAGS) -o server server.c libmonitor.a

client: client.c
	$(CC) $(CFLAGS) -o client client.c


clean:
	rm -f *.o libmonitor.a server client