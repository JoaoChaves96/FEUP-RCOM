CC=gcc
CFLAGS = -lm -Wall

all: writenoncanonical noncanonical

writenoncanonical: DataLink.h writenoncanonical.c alarm.h utils.h
	$(CC) writenoncanonical.c DataLink.c alarm.c utils.c -o writenoncanonical $(CFLAGS)
	
noncanonical: DataLink.h noncanonical.c alarm.h utils.h
	$(CC) noncanonical.c DataLink.c alarm.c utils.c -o noncanonical $(CFLAGS)
	
clean:
	rm -f writenoncanonical
	rm -f noncanonical
