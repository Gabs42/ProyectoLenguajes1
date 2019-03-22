#include <stdio.h>
#include <stdlib.h>
#include "IOHandler.h"
#include <string.h>

//TO-DO add documentation
int writePort(char *ptr_port, char *file)
{
	char IP[10];
	FILE *ptr_file;
	ptr_file = fopen(file, "r");

	if(!ptr_file)
	{
		printf("Could not read file\n");
		return 1;
	}	

	fgets(IP, 10, ptr_file);
	fgets(IP, 10, ptr_file);

	fclose(ptr_file);	

	ptr_file = fopen(file, "w");

	if(!ptr_file || !ptr_port)
		return 1;

	while(*ptr_port != '\0'){
		fprintf(ptr_file, "%c", *ptr_port);
		ptr_port++;
	}
	fprintf(ptr_file, "\n");
	
	char *ptr_IP = IP;
	while(*ptr_IP != '\0'){
		fprintf(ptr_file, "%c", *ptr_IP);
		ptr_IP++;	
	}

	fclose(ptr_file);
	return 0;
}

//Receives an int to be written as a port        MUST CHANGE
//Does a conversion to string and uses writePort()
void writePortInt(int port, char *file)
{
	char strPort[10];
	sprintf(strPort, "%d", port);
	writePort(strPort, file);
}

//Receives a char[] of a given size, and assigns it a value taken from "port.txt" MUST CHANGE
char *readPort(char *buf, char *file)
{
	FILE *ptr_file;
	ptr_file = fopen(file, "r");

	if(!ptr_file)
	{
		printf("Could not read file\n");
		return NULL;
	}	

	fgets(buf, 1024, ptr_file);

	fclose(ptr_file);
    
    return buf;
}

//Converts the result of readPort to int and returns the value; MUST CHANGE
int readPortInt(char *file)
{
	char rPort[1024]; 
	readPort(rPort, file);
	return (int) strtol(rPort, (char **)NULL, 10);
}

//TO-DO documentation
char *readIP(char *buf, char *file)
{
	FILE *ptr_file;
	ptr_file = fopen(file, "r");

	if(!ptr_file)
	{
		printf("Could not read file\n");
		return NULL;
	}	

	fgets(buf, 1024, ptr_file);
	fgets(buf, 1024, ptr_file);
    
    // Cut buf at the first whitespace:
    strtok(buf, "\n");
    strtok(buf, "\r");
    strtok(buf, "\t");
    strtok(buf, " ");

	fclose(ptr_file);
	return buf;
}

//TO-DO documentation
int writeIP(char* ptr_IP, char* file)
{
	char port[10];
	FILE *ptr_file;
	ptr_file = fopen(file, "r");

	if(!ptr_file)
	{
		printf("Could not read file\n");
		return 1;
	}	

	fgets(port, 10, ptr_file);

	fclose(ptr_file);	

	ptr_file = fopen(file, "w");

	if(!ptr_file || !ptr_IP)
		return 1;

	char *ptr_port = port;
	while(*ptr_port != '\0'){
		fprintf(ptr_file, "%c", *ptr_port);
		ptr_port++;	
	}

	while(*ptr_IP != '\0'){
		fprintf(ptr_file, "%c", *ptr_IP);
		ptr_IP++;
	}

	fclose(ptr_file);
	return 0;
}



/*
	char *wPort = "34";
	writePort(wPort, "server.conf");
*/
/*
	int wPortInt = 52;
	writePortInt(wPortInt, "server.conf");
*/
/*	
	char rPort[10]; 
	readPort(rPort, "server.conf");
	printf("%s\n", rPort);
*/
/*
	int rPortInt = readPortInt("server.conf");
	printf("%d\n", rPortInt);
*/
/*
	char rIP[10];
	readIP(rIP, "server.conf");
	printf("%s\n", rIP);
*/
/*
	char *rIP;
	char buf[10];
	rIP = readIP(buf, "server.conf");
	printf("IP: %s\n", rIP);
*/
/*
	char *wIP = "8.8.8.8";
	writeIP(wIP, "server.conf");
*/	



