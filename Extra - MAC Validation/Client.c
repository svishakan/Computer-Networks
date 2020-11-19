/*
Credits to Shashanka Venkatesh the real lord for 
providing 75% of the base code for this program. GG.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define PORT    	7780
#define BUFF_SIZE	1024

int validateMAC(char *mac);
char *getFileList(char *path);

int main(int argc, char *argv[]){
	struct sockaddr_in server, client;
	int len = sizeof(client), sockfd;
	char buffer[BUFF_SIZE], *list;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0){
		perror("Socket error.");
        exit(1);
    }

	bzero(&server, len);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(PORT);

	//Send MAC to be validated
	
	printf("\nClient ready...\n");

	printf("\nClient MAC: ");
	scanf(" %[^\n]", buffer);

	sendto(sockfd, buffer, BUFF_SIZE, MSG_CONFIRM, (struct sockaddr*)&server, len);

	recvfrom(sockfd, buffer, BUFF_SIZE, MSG_WAITALL, (struct sockaddr*)&server, &len);
	printf("\nServer: %s\n", buffer);

	//Request list of files stored in client

	recvfrom(sockfd, buffer, BUFF_SIZE, MSG_WAITALL, (struct sockaddr*)&server, &len);
	printf("\nServer MAC: %s\n", buffer);
	
	if (validateMAC(buffer))
		strcpy(buffer, "The MAC address is valid.");
	else
		strcpy(buffer, "The MAC address is invalid.");

	printf("\n%s\n", buffer);
	sendto(sockfd, buffer, BUFF_SIZE, MSG_CONFIRM, (struct sockaddr*)&server, len);

	printf("\n-----------------------------------------------\n");

	//Accept request for list of files from server

	recvfrom(sockfd, buffer, BUFF_SIZE, MSG_WAITALL, (struct sockaddr*)&client, &len);
	printf("\nServer: %s\n", buffer);

	list = getFileList(".");      //current directory as path

	if (list == NULL)
		strcpy(buffer, "Error while obtaining list of files.");
	else
		strcpy(buffer, list);

	printf("\nList of Files: \n%s\n", buffer);

	sendto(sockfd, buffer, BUFF_SIZE, MSG_CONFIRM, (struct sockaddr*)&server, len);

	printf("List of Files sent to server!\n");

	close(sockfd);

	return 0;
}

int validateMAC(char *mac){
    //Validate the given MAC address
	int fieldCount = 0, i, len = strlen(mac);
	int bytes[6];

	for (i = 0; i < len; i++)
		if (mac[i] == ':')
			fieldCount++;

	if (fieldCount != 5)
		return 0;

	if((sscanf(mac, "%x:%x:%x:%x:%x:%x", &bytes[0], &bytes[1], &bytes[2], &bytes[3], &bytes[4], &bytes[5])) != 6){
        //It did not assign 6 fields currently, thus MAC is invalid
        return 0;
    }

	for (i = 0; i < 4; i++)
        //Checking the range of the MAC bytes
		if (bytes[i] < 0 || bytes[i] > 255)
			return 0;

	return 1;
}

char *getFileList(char *path){
    //Makes a string of list of files stored in the given path

	FILE *fp;
	char *list = (char *)malloc(1024), command[100], ch;
	int i = 0;

	strcpy(command, "ls ");         //system command to get the list of files
	strcat(command, path);          //give the path argument to ls

	fp = popen(command, "r");       //read the output of the system call
	
	if(fp == NULL){
		perror("Unable to call system command: ls\n");
		return NULL;
	}

	while((ch = fgetc(fp)) != EOF){  //put the output of the file stream(ls command) to char array
		list[i++]=ch;
    }

	pclose(fp);                     //close the process

	return list;
}