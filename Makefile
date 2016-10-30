CC=gcc
CFLAGS = -lm -Wall

all: writenoncanonical noncanonical

writenoncanonical: DataLink.h writenoncanonical.c alarm.h ApplicationLayer.h
	$(CC) writenoncanonical.c DataLink.c alarm.c ApplicationLayer.c -o writenoncanonical $(CFLAGS)

noncanonical: DataLink.h noncanonical.c alarm.h ApplicationLayer.h
	$(CC) noncanonical.c DataLink.c alarm.c ApplicationLayer.c -o noncanonical $(CFLAGS)

clean:
	rm -f writenoncanonical
	rm -f noncanonical
