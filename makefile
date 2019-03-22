
GCC_OPTIONS = -Wall -Wextra -std=c11

build: server client

server: server_main.c server.c server.h
	gcc $(GCC_OPTIONS) -o server server_main.c server.c

client: client_main.c client.c client.h
	gcc $(GCC_OPTIONS) -o client client_main.c client.c

clean:
	rm -f client
	rm -f server

