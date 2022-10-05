CC=gcc
CFLAGS=-I.

client: client.c libraries/requests/requests.c libraries/helpers/helpers.c libraries/buffer/buffer.c libraries/jsons/parson.c libraries/commands/commands.c
	$(CC) -o client client.c libraries/requests/requests.c libraries/helpers/helpers.c libraries/buffer/buffer.c libraries/jsons/parson.c -Wall libraries/commands/commands.c

run: client
	./client

clean:
	rm -f *.o client
