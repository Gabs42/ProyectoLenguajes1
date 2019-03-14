
GCC_OPTIONS = -Wall -Wextra

build: server client

server: Server.c
	gcc $(GCC_OPTIONS) -o server Server.c

client: Client.c
	gcc $(GCC_OPTIONS) -o client Client.c

clean:
	rm -f client
	rm -f server

