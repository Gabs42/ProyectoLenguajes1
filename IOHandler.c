#include <stdio.h>
#include <stdlib.h>
#include "IOHandler.h"

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

/*
char *readPort()
{
	FILE *ptr_file;
	char buf[10];
	ptr_file = fopen("port.txt", "r");

	if(!ptr_file)
	{
		printf("Could not read file\n");
		return NULL;
	}	
	
	while(fgets(buf, 10, ptr_file) != NULL)
	{
		printf("%s", buf);
	}
	printf("\n");

	fclose(ptr_file);

	return &buf;
}
*/

int main()
{
	/*
	char *port = "2234";
	writePort(port);
	*/

	/*
	char *port; 
	port = readPort();	
	printf("%s\n", port);
	*/

	return 0;
}
