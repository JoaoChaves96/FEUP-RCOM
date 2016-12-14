CC=gcc
CFLAGS=-Wall -I.

download: download.c
				$(CC) download.c tcpClient.c get_url.c -o download $(CFLAGS)

clean:
				rm -f download *.o
