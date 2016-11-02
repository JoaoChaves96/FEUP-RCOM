CC=gcc
CFLAGS = -lm -Wall

all: send receive

send: DataLink.h writenoncanonical.c alarm.h ApplicationLayer.h Statistics.h
	$(CC) writenoncanonical.c DataLink.c alarm.c ApplicationLayer.c Statistics.c -o send $(CFLAGS)

receive: DataLink.h noncanonical.c alarm.h ApplicationLayer.h Statistics.h
	$(CC) noncanonical.c DataLink.c alarm.c ApplicationLayer.c Statistics.c -o receive $(CFLAGS)

clean:
	rm -f send
	rm -f receive
