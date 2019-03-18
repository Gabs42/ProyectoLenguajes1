
#define _POSIX_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/ioctl.h>


// Macros for ANSI escape codes for changing terminal text color:
#define ANSI_TERMINAL_GREEN "\x1B[32m"
#define ANSI_TERMINAL_RED   "\x1B[34m"
#define ANSI_TERMINAL_RESET "\x1B[0m"


int main(int argc, char * argv[]) {
	
	int socketFd;
	
	char * serverHostname = argv[1];
	char * serverPortString = argv[2];
	
	int serverPort;
	struct hostent * serverInfo;
	struct sockaddr_in serverAddress;
	
	struct winsize terminalWindowInfo;

	
	if (argc < 3) {
		fprintf(stderr, "ERROR: Not enough arguments. Usage: %s <server address> <server port>\n", argv[0]);
		exit(1);
	}
	
	
	serverPort = atoi(serverPortString); // TODO: Change to strtol()
	
	
	// Try to create the client socket:
	socketFd = socket(PF_INET, SOCK_STREAM, 0);
	
	if (socketFd < 0) {
		fprintf(stderr, "ERROR: Could not create the client socket.\n");
		exit(1);
	}
	
	
	// Try to get the server's address (and other info):
	serverInfo = gethostbyname(serverHostname); // TODO: Replace with getaddrinfo()
	
	if(serverInfo == NULL) {
		fprintf(stderr, "ERROR: Could not find info for '%s'.\n", argv[1]);
		exit(1);
	}
	
	
	memset(&serverAddress, 0, sizeof(serverAddress)); // Not really necessary.
	
	serverAddress.sin_family = AF_INET; // IPv4
	serverAddress.sin_port = htons(serverPort); // sockaddr_in stores the port in network byte order
	memcpy(&serverAddress.sin_addr.s_addr, serverInfo->h_addr_list[0], serverInfo->h_length);
	
	// Try to connect to the server:
	if(connect(socketFd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) != 0) {
		fprintf(stderr, "ERROR: Could not connect to the server at address \"%s\".\n", serverHostname);
		exit(1);
	}
	
	
	// Get terminal window information:
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminalWindowInfo);
	
	
	char usernameMessage[64] = "#";
	
	printf("Ingrese su nombre de usuario: ");
	fgets(usernameMessage + 1, 50, stdin);
	
	// TODO: Validate the given username.
	
	// Delete the first '\n' and '\r' characters in usernameMessage
	strtok(usernameMessage, "\r");
	strtok(usernameMessage, "\n");
	
	printf("DEBUG: Sending username: \"%s\".\n", usernameMessage);
	
	int usernameBytesSent = send(socketFd, usernameMessage, strlen(usernameMessage) + 1, 0);
	
	// FIX: Check that all bytes were sent.
	
	if(usernameBytesSent < 0 || usernameBytesSent == 0) {
		fprintf(stderr, "ERROR: Could not send() username request to server.\n");
		exit(1);
	}
	
	
	pid_t forkResult = fork();
	
	if(forkResult < 0) {
		fprintf(stderr, "ERROR: Fork failed./n");
		exit(1);
	}else if(forkResult == 0) {
		// Child process; recv() from here.
		
		char recvBuffer[1024];
		recvBuffer[1023] = '\0';
		int bytesReceived;
		
		while(1) {
			bytesReceived = recv(socketFd, recvBuffer, 1023, 0);
			
			if(bytesReceived == 0) {
				printf("INFO: Connection closed by the server. Press enter to quit.\n");
				exit(0);
			}else if (bytesReceived < 0) {
				fprintf(stderr, "ERROR: recv failed.");
				exit(1);
			}else {
				fprintf(stdout, ANSI_TERMINAL_GREEN "%*s\n" ANSI_TERMINAL_RESET, terminalWindowInfo.ws_col, recvBuffer);
			}
		}
	}else {
		// Parent process; send() from here.
		
		char sendBuffer[1024];
		sendBuffer[1023] = '\0';
		int bytesSent;
		
		printf("\nFormato para enviar un mensaje: Usuario:Mensaje\n");
		printf("Para salir, escribir \"$S\".\n\n");
		
		while(1) {
			fgets(sendBuffer, 1024, stdin);
			
			// Delete the first '\n' and '\r' characters in sendBuffer:
			strtok(sendBuffer, "\r");
			strtok(sendBuffer, "\n");
			
			
			// Try to send the message to the server:
			bytesSent = send(socketFd, sendBuffer, strlen(sendBuffer) + 1, 0);
			
			// FIX: Check that all bytes were sent.
			
			if(bytesSent < 0) {
				fprintf(stderr, "ERROR: send() failed.\n");
				exit(1);
			}else if(strcmp(sendBuffer, "$S") == 0) {
				printf("INFO: Closing connection and quitting program. Bye!\n");
				
				// Kill the child process:
				kill(forkResult, SIGTERM);
				
				// Close the connection from here, even though the server should also close it.
				close(socketFd);
				exit(0);
			}
		}
	}
	
	//close(socketFd);
	return 0;
}

