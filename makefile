
GCC_OPTIONS = -Wall -Wextra -std=c11

build: server client

server: server_main.c
	gcc $(GCC_OPTIONS) -o server server_main.c

client: client_main.c
	gcc $(GCC_OPTIONS) -o client client_main.c

clean:
	rm -f client
	rm -f server

