

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/select.h>


#define MAX_CLIENTS 128


struct Client {
    char username[50];
    unsigned int socketFd; // The socket's file descriptor.
    struct sockaddr_in socketAddress;
};


struct ClientSlot {
    char empty;
    struct Client client;
};


// Used to clean slots:
extern const struct ClientSlot EMPTY_CLIENT_SLOT;


struct ClientSlot clients[MAX_CLIENTS];


/*  Returns the index of the first empty slot in the clients array,
    or -1 if the array is full.
*/
int firstEmptyClientSlot();


/*  Finds the index of the client slot that currently has the given username.
    
    Returns the index of the client slot that has the given username,
        or -1 if no such slot was found.
*/
int findClientByUsername(const char * username) ;


/*  Checks whether the given username is already being used in a
    non-empty slot in the clients array.

    Returns 1 if already used, 0 otherwise.
*/
int isUsernameAlreadyUsed(const char * username);


/*  Frees the slot at the given index.

    (By freeing the slot, the username is also freed).
*/
void freeClientSlot(unsigned int clientIndex);


int insertNewClient(unsigned int clientSocketFd, struct sockaddr_in clientAddress);


/*  Prints the contents of the given struct Client, with the given indentation.
*/
void printClient(struct Client client, char * indentation);


/*  Prints the contents of the given struct Client, with the given indentation in a single line.
*/
void printClientOneLine(struct Client client, char * indentation);


void printClientAtIndex(unsigned int index);


/*  Prints all clients in the clients array.
*/
void printClients();


/*  Recv's data from a client and handles it.
*/
void handleClientRecv(unsigned int clientIndex);


/*  Uses recv on a client socket and stores the received client username.
    
    Should be used once just after a client has connected.
*/
int receiveClientUsername(unsigned int clientIndex);


/*  Clears all the slots in the clients array, using EMPTY_CLIENT_SLOT.
*/
void initializeClientsArray();


/*  Reset readFdSet and exceptFdSet to include only the connected clients, and the server socket.

NOTE: This is could be done more efficiently, by only FD_SET'ing the new fd,
     and FD_ZERO'ing the closed connections.
*/
void resetSelectFdSets(fd_set * readFdSet, fd_set * exceptFdSet, int serverSocketFd, int * highestFd);


void closeAllConnections(int serverSocketFd);


void acceptClientConnection(int serverSocketFd);


void handleActivityInClients(fd_set * exceptFdSet, fd_set * readFdSet);

