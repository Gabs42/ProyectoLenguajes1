#include <stdio.h>
#include <stdlib.h>
#include "IOHandler.h"

//Recives a string and writes it to a text file, "port-txt"
//Returns 0 if the operation was successful
//Returns 1 if the operation was unsuccessful
int writePort(char *ptr_port)
{
	FILE *ptr_file;
	ptr_file = fopen("port.txt", "w");

	if(!ptr_file || !ptr_port)
		return 1;

	while(*ptr_port != '\0'){
		fprintf(ptr_file, "%c", *ptr_port);
		ptr_port++;
	}

	fclose(ptr_file);
	return 0;
}

//Recives an int to be written as a port
//Does a conversion to string and uses writePort()
int writePortInt(int port)
{
	char strPort[10];
	sprintf(strPort, "%d", port);
	writePort(strPort);
}

//Recieves a char[] of a given size, and assigns it a value taken from "port.txt"
char *readPort(char *buf)
{
	FILE *ptr_file;
	ptr_file = fopen("port.txt", "r");

	if(!ptr_file)
	{
		printf("Could not read file\n");
		return NULL;
	}	
	
	while(fgets(buf, 10, ptr_file) != NULL)
	{
		//printf("%s", buf);
	}

	fclose(ptr_file);
}

//Converts the result of readPort to int and returns the value;
int readPortInt()
{
	char rPort[10]; 
	readPort(rPort);
	return (int) strtol(rPort, (char **)NULL, 10);
}

int main()
{
/*
	char *wPort = "30";
	writePort(wPort);
*/
/*
	int wPortInt = 32;
	writePortInt(wPortInt);
*/
/*	
	char rPort[10]; 
	readPort(rPort);
	printf("%s\n", rPort);
*/
/*
	int rPortInt = readPortInt();
	printf("%d\n", rPortInt);
*/
	return 0;
}


