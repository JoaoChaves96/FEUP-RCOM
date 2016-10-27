CC=gcc
CFLAGS = -lm -Wall

all: writenoncanonical noncanonical

writenoncanonical: DataLink.h writenoncanonical.c alarm.h
	$(CC) writenoncanonical.c DataLink.c alarm.c -o writenoncanonical $(CFLAGS)

noncanonical: DataLink.h noncanonical.c alarm.h
	$(CC) noncanonical.c DataLink.c alarm.c -o noncanonical $(CFLAGS)

clean:
	rm -f writenoncanonical
	rm -f noncanonical
