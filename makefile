
GCC_OPTIONS = -Wall -Wextra -std=c11

build: server client

server: server_main.c server.c server.h IOHandler.c IOHandler.h
	gcc $(GCC_OPTIONS) -o server IOHandler.c server_main.c server.c

client: client_main.c IOHandler.c IOHandler.h
	gcc $(GCC_OPTIONS) -o client IOHandler.c client_main.c

clean:
	rm -f client
	rm -f server

