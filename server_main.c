

#include "server.h"

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>



int main(int argc, char * argv[]) {
    // Clear out all client slots in the clients array:
    initializeClientsArray();
    
    if(argc < 2) {
        fprintf(stderr, "ERROR: No server port given.\n");
        exit(1);
    }
    
    // Variables for the server's information
    int serverPort;
    struct sockaddr_in serverAddress;
    
    // Create the server socket, IPv4, TCP.
    int serverSocketFd = socket(PF_INET, SOCK_STREAM, 0);
    
    if(serverSocketFd < 0) {
        fprintf(stderr, "ERROR: socket could not be created.\n");
        exit(1);
    }

    // Convert port number argument to int
    serverPort = atoi(argv[1]);
    
    // Zero out serverAddress, only necessary on some platforms: 
    memset((char *) &serverAddress, 0, sizeof(serverAddress));
    
    serverAddress.sin_family = AF_INET; // IPv4 socket address
    serverAddress.sin_addr.s_addr = INADDR_ANY; // Listen to any incoming connection
    serverAddress.sin_port = htons(serverPort); // Convert port number to network byte order
    
    // Assign the server's address to the socket
    if(bind(serverSocketFd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        fprintf(stderr, "ERROR: Could not bind the server socket to the given port: %d.\n", serverPort);
        exit(1);
    }
    
    printf("INFO: Server bound to port \"%d\"\n", serverPort);

    // Mark the socket to be able to accept incoming connection requests
    listen(serverSocketFd, 5);
    
    // File descriptor sets for select():
    fd_set readFdSet;
    fd_set exceptFdSet;
    
    // Highest file descriptor value for select():
    int highestFd;
    
    while(1) {
        // Reset readFdSet and exceptFdSet to include only the connected clients, and the server socket:
        highestFd = serverSocketFd;
        resetSelectFdSets(&readFdSet, &exceptFdSet, serverSocketFd, &highestFd);
        
        // Wait for activity in one of the file descriptors in the readFdSet, indefinitely:
        printf("\nINFO: Waiting with select() for any client activity.\n");
        printClients();
        int selectResult = select(highestFd + 1, &readFdSet, NULL, &exceptFdSet, NULL);
        
        // Handle select cases:
        if(selectResult == -1 || selectResult == 0) {
            fprintf(stderr, "ERROR: select() returned %d.\n", selectResult);
            
            closeAllConnections(serverSocketFd);
            exit(1);
        }else {
            // Handle exceptions on the server socket:
            if(FD_ISSET(serverSocketFd, &exceptFdSet)) {
                fprintf(stderr, "ERROR: Exception on server socket.\n");
                
                closeAllConnections(serverSocketFd);
                exit(1);
            }
            
            // Handle new connections to the server:
            if(FD_ISSET(serverSocketFd, &readFdSet)) {
                acceptClientConnection(serverSocketFd);
            }
            
            // Handle activity in any client slot:
            handleActivityInClients(&exceptFdSet, &readFdSet);
        }
    }
    
    return 0;
}

