CC=gcc
CFLAGS = -lm -Wall

all: writenoncanonical noncanonical

writenoncanonical: DataLink.h writenoncanonical.c
	$(CC) writenoncanonical.c DataLink.c -o writenoncanonical $(CFLAGS)
	
noncanonical: DataLink.h noncanonical.c
	$(CC) noncanonical.c DataLink.c -o noncanonical $(CFLAGS)
	
clean:
	rm -f writenoncanonical
	rm -f noncanonical
