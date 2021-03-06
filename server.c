

#include "server.h"


// Used to clean slots:
const struct ClientSlot EMPTY_CLIENT_SLOT = {1, {{0}, 0, {0}}};


/*    Returns the index of the first empty slot in the clients array,
    or -1 if the array is full.
*/
int firstEmptyClientSlot() {
    // Check each slot
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(clients[i].empty) {
            return i;
        }
    }
    
    // No empty slot was found
    return -1;
}


/*    Finds the index of the client slot that currently has the given username.
    
    Returns the index of the client slot that has the given username,
        or -1 if no such slot was found.
*/
int findClientByUsername(const char * username) {
    for(int index = 0; index < MAX_CLIENTS; index++) {
        if(!(clients[index].empty)) {
            if(strcmp(clients[index].client.username, username) == 0) {
                return index;
            }
        }
    }
    
    // The username was not found.
    return -1;
}


/*    Checks whether the given username is already being used in a
    non-empty slot in the clients array.

    Returns 1 if already used, 0 otherwise.
*/
int isUsernameAlreadyUsed(const char * username) {
    int indexFound = findClientByUsername(username);
    
    if(indexFound == -1) {
        return 0;
    }
    
    return 1;
}


/*    Frees the slot at the given index.

    (By freeing the slot, the username is also freed).
*/
void freeClientSlot(unsigned int clientIndex) {
    clients[clientIndex] = EMPTY_CLIENT_SLOT;
}


int insertNewClient(unsigned int clientSocketFd, struct sockaddr_in clientAddress) {
    int slot = firstEmptyClientSlot();
    
    if(slot == -1) {
        // There are no more empty slots.
        fprintf(stderr, "ERROR: insertNewClient: There are no more slots.\n");
        return -1;
    }
    
    // Mark slot as not empty:
    clients[slot].empty = 0;
    
    // Write the client's socket information:
    *(clients[slot].client.username) = '\0'; // Remove any previous username
    clients[slot].client.socketFd = clientSocketFd;
    clients[slot].client.socketAddress = clientAddress;
    
    return slot;
}


/*    Prints the contents of the given struct Client, with the given indentation.
*/
void printClient(struct Client client, char * indentation) {
    if(indentation == NULL) {
        indentation = "";
    }
    
    char addressBuffer[INET_ADDRSTRLEN];
    
    inet_ntop(AF_INET, &client.socketAddress.sin_addr, addressBuffer, INET_ADDRSTRLEN);
    
    printf("%sclient.username: %s\n", indentation, client.username);
    printf("%sclient.socketFd: %u\n", indentation, client.socketFd);
    printf("%sclient.sockaddr_in.sin_family: AF_INET\n", indentation);
    printf("%sclient.sockaddr_in.sin_port: %d\n", indentation, ntohs(client.socketAddress.sin_port));
    printf("%sclient.sockaddr_in.sin_addr: %s\n\n", indentation, addressBuffer);
}


/*    Prints the contents of the given struct Client, with the given indentation in a single line.
*/
void printClientOneLine(struct Client client, char * indentation) {
    if(indentation == NULL) {
        indentation = "";
    }
    
    char addressBuffer[INET_ADDRSTRLEN];
    
    inet_ntop(AF_INET, &client.socketAddress.sin_addr, addressBuffer, INET_ADDRSTRLEN);
    
    printf("%s(username: \"%s\", ", indentation, client.username);
    printf("socketFd: %u, ", client.socketFd);
    printf("sockaddr_in.sin_family: AF_INET, ");
    printf("sockaddr_in.sin_port: %d, ", ntohs(client.socketAddress.sin_port));
    printf("sockaddr_in.sin_addr: %s)", addressBuffer);
}


void printClientAtIndex(unsigned int index) {
    printClientOneLine(clients[index].client, " ");
}


/*    Prints all clients in the clients array.
*/
void printClients() {
    unsigned int index = 0;
    
    while(index != MAX_CLIENTS) {
        if(!clients[index].empty) {
            printf("clients[%d]:", index);
            printClientAtIndex(index);
            printf("\n");
        }else {
            //printf("\t<Empty slot>\n");
        }
        
        index++;
    }
}


/*    Recv's data from a client and handles it.
*/
void handleClientRecv(unsigned int clientIndex) {
    if(clientIndex >= MAX_CLIENTS) {
        fprintf(stderr, "ERROR: clientMessageRecvSendLoop(%u): client index >= MAX_CLIENTS.\n", clientIndex);
        return;
    }
    
    if(clients[clientIndex].empty) {
        fprintf(stderr, "ERROR: clientMessageRecvSendLoop(%u): given client slot is empty.\n", clientIndex);
        return;
    }
    
    char recvBuffer[1024];
    char sendBuffer[1024];
    
    unsigned int socketFd = clients[clientIndex].client.socketFd;
    
    int bytesReceived = recv(socketFd, recvBuffer, 1023, 0);
    
    if(bytesReceived < 0) {
        fprintf(stderr, "ERROR: clientMessageRecvSendLoop(%u): error on recv.\n", clientIndex);
        return;
    }else if(bytesReceived == 0) {
        printf("INFO: clientMessageRecvSendLoop: Received empty message from client at index %u. Closing connection.\n", clientIndex);
        close(socketFd);
        freeClientSlot(clientIndex);
        return;
    }
    
    // Mark the end of the received string:
    recvBuffer[bytesReceived] = '\0';
    
    char * delimPosition = strchr(recvBuffer, ':');
    
    if(strcmp(recvBuffer, "$S") == 0) {
        printf("INFO: clientMessageRecvSendLoop: Received close connection message from client at index %u. Closing connection.\n", clientIndex);
        close(socketFd);
        freeClientSlot(clientIndex);
        return;
    }else if(delimPosition != NULL) {
        char * targetUsername = strtok(recvBuffer, ":");
        char * textMessage = delimPosition + 1; //FIX: Limit the length of the text message.
        
        // size_t targetUsernameLength = strlen(targetUsername); //TODO: Check that targetUsername is not too long
        
        int targetClientIndex = findClientByUsername(targetUsername);
        
        if(targetClientIndex == -1) {
            printf("clientMessageRecvSendLoop(%d): target client (%s) was not found.\n", clientIndex, targetUsername);
        }else {
            strcpy(sendBuffer, clients[clientIndex].client.username);
            strcat(sendBuffer, ":");
            strcat(sendBuffer, textMessage); //FIX: Properly check that the text message is not written outside the buffer.
            
            int targetClientSocketFd = clients[targetClientIndex].client.socketFd;
            
            printf("DEBUG: sending \"%s\" to client with username \"%s\".\n", sendBuffer, targetUsername);
            int bytesSent = send(targetClientSocketFd, sendBuffer, strlen(sendBuffer) + 1, 0);
            
            if(bytesSent < 0) {
                fprintf(stderr, "ERROR: clientMessageRecvSendLoop: could not send message to fd %d.\n", targetClientSocketFd);
                // Maybe tell the sender that the message could not be delivered.
            }
        }
    }else {
        fprintf(stderr, "ERROR: clientMessageRecvSendLoop(%u): Ignored unrecognized data received: \"%s\".\n", clientIndex, recvBuffer);
    }
}


int receiveClientUsername(unsigned int clientIndex) {
    if(clientIndex >= MAX_CLIENTS) {
        fprintf(stderr, "ERROR: receiveClientUsername(%d): clientIndex >= MAX_CLIENTS.\n", clientIndex);
        return 0;
    }
    
    if(clients[clientIndex].empty) {
        fprintf(stderr, "ERROR: receiveClientUsername(%d): Empty client slot.\n", clientIndex);
        return 0;
    }
    
    // Wait for the client to send the username request:
    char recvBuffer[128];
    int receivedBytes = recv(clients[clientIndex].client.socketFd, recvBuffer, 127, 0);
    
    printf("DEBUG: receiveClientUsername: '%s'\n", recvBuffer);
    
    if(receivedBytes == -1) {
        fprintf(stderr, "ERROR: receiveClientUsername(%d): recv returned -1.\n", clientIndex);
        return 0;
    }
    
    int receivedUsernameLength = strlen(recvBuffer + 1); // + 1 to ignore the '#'
    
    //TODO: Change 50 to a macro, like CLIENT_USERNAME_SIZE
    
    if(recvBuffer[0] != '#' || receivedUsernameLength >= 50) {
        fprintf(stderr, "ERROR: receiveClientUsername(%d): Invalid username request by client.\n", clientIndex);
        return 0;
    }
    
    if(isUsernameAlreadyUsed(recvBuffer + 1)) {
        fprintf(stderr, "ERROR: receiveClientUsername(%d): Username was already taken: %s.\n", clientIndex, recvBuffer + 1);
        return 0;
    }
    
    // Write the received username to client.username
    strcpy(clients[clientIndex].client.username, recvBuffer + 1);
    
    return 1;
}


void initializeClientsArray() {
    // Mark all clients as empty:
    for(unsigned int i = 0; i < MAX_CLIENTS; i++) {
        clients[i] = EMPTY_CLIENT_SLOT;
    }
}


void resetSelectFdSets(fd_set * readFdSet, fd_set * exceptFdSet, int serverSocketFd, int * highestFd) {
    FD_ZERO(readFdSet);
    FD_ZERO(exceptFdSet);
    FD_SET(serverSocketFd, readFdSet);
    FD_SET(serverSocketFd, exceptFdSet);
    
    for(unsigned int clientIndex = 0; clientIndex < MAX_CLIENTS; clientIndex++) {
        if(!clients[clientIndex].empty) {
            // If this slot is not empty, add it to the sets for select:
            int clientSocketFd = clients[clientIndex].client.socketFd;
            FD_SET(clientSocketFd, readFdSet);
            FD_SET(clientSocketFd, exceptFdSet);
            
            // Keep highestFd updated:
            if(clientSocketFd > *highestFd) {
                *highestFd = clientSocketFd;
            }
        }
    }
}


void closeAllConnections(int serverSocketFd) {
    close(serverSocketFd);
    for(unsigned int clientIndex = 0; clientIndex < MAX_CLIENTS; clientIndex++) {
        close(clients[clientIndex].client.socketFd);
    }
}



void acceptClientConnection(int serverSocketFd) {
    // Accept new incoming connection:
    printf("INFO: accept()'ing client connection.\n");
    
    // Used to store information about a client that has been accepted:
    struct sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(clientAddress);
    
    
    // Wait for the next client connection:
    int clientSocketFd = accept(serverSocketFd, (struct sockaddr *) &clientAddress, &clientAddressSize);
    if(clientSocketFd < 0) {
        fprintf(stderr, "ERROR: Could not accept a new client.\n");
        exit(1); //TODO: Check if we could ignore this and keep going.
    }
    
    printf("INFO: New client connected with socket FD: %hu.\n", clientSocketFd);
    
    
    // Try to insert the new client in the clients array:
    int clientIndex = insertNewClient(clientSocketFd, clientAddress);
    if(clientIndex < 0) {
        fprintf(stderr, "ERROR: Client rejected because there are no more client slots.\n");
        
        // Close the client socket:
        close(clientSocketFd);
    }
}


void handleActivityInClients(fd_set * exceptFdSet, fd_set * readFdSet) {
    // Handle activity in any client slot:
    for(unsigned int clientIndex = 0; clientIndex < MAX_CLIENTS; clientIndex++) {
        int clientSocketFd = clients[clientIndex].client.socketFd;
        int slotNotEmpty = !clients[clientIndex].empty;
        int activityAvailable = FD_ISSET(clientSocketFd, exceptFdSet);
        
        if(slotNotEmpty && activityAvailable) {
            fprintf(stderr, "ERROR: clientSocketFd %d had an exception, closing connection.\n", clientSocketFd);
            // Close the client socket, and free its slot:
            close(clientSocketFd);
            freeClientSlot(clientIndex);
        }else if(FD_ISSET(clientSocketFd, readFdSet)) {
            // Check if this client socket has sent anything:
            
            int clientHasNoName = *(clients[clientIndex].client.username) == '\0';
            
            if(clientHasNoName) {
                // Try to assign the client's username:
                printf("INFO: calling receiveClientUsername(%d).\n", clientSocketFd);
                int usernameWasSet = receiveClientUsername(clientIndex);
                
                if(!usernameWasSet) {
                    fprintf(stderr, "ERROR: Client at index %d did not ask for a valid username:\n", clientIndex);
                    printClientAtIndex(clientIndex);
                    
                    close(clientSocketFd);
                    freeClientSlot(clientIndex);
                }
            }else {
                // Normal messages are handled here:
                printf("INFO: calling handleClientRecv(%d).\n", clientSocketFd);
                handleClientRecv(clientIndex);
            }
        }
    }
}
