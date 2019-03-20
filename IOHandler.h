#ifndef IOHandler_H
#define IOHandler_H

int writePort(char* ptr_port, char* file);
int writePortInt(int port, char* file);
int writeIP(char* ptr_IP, char* file);
char *readPort(char* buffer, char* file);
int readPortInt(char* file);
char *readIP(char* buffer, char* file);


#endif 
