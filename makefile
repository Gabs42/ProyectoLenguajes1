
build: server client

server: Server.c
	gcc -Wall -Wextra -o server Server.c

client: Client.c
	gcc -Wall -Wextra -o client Client.c


