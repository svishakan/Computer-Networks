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

// Validates the given MAC address
int validateMAC(char *mac);

int main(int argc, char *argv[]){
	struct sockaddr_in server, client;
	int len = sizeof(client), sockfd;
	char buffer[BUFF_SIZE];
	FILE *fp;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0){
		perror("Socket error.");
        exit(1);
    }

	bzero(&server, len);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	if (bind(sockfd, (struct sockaddr*)&server, len) < 0){
		perror("Bind error.");
        exit(1);
    }

	printf("\nServer awaiting client at port %d\n", PORT);

    //Receive MAC and validate

	recvfrom(sockfd, buffer, BUFF_SIZE, MSG_WAITALL, (struct sockaddr*)&client, &len);
	printf("\nClient MAC: %s\n", buffer);

	if (validateMAC(buffer))
		strcpy(buffer, "The MAC address is valid.");
	else
		strcpy(buffer, "The MAC address is invalid.");

	printf("\n%s\n", buffer);

	sendto(sockfd, buffer, BUFF_SIZE, MSG_CONFIRM, (struct sockaddr*)&client, len);

    //Send MAC to be validated

	printf("\nServer IP: ");
	scanf(" %[^\n]", buffer);

	sendto(sockfd, buffer, BUFF_SIZE, MSG_CONFIRM, (struct sockaddr*)&client, len);

	recvfrom(sockfd, buffer, BUFF_SIZE, MSG_WAITALL, (struct sockaddr*)&client, &len);
	printf("\nClient: %s\n", buffer);
	
	printf("\n-----------------------------------------------\n");

    //Request list of files stored in client

	strcpy(buffer, "List of Files");
	sendto(sockfd, buffer, BUFF_SIZE, MSG_CONFIRM, (struct sockaddr*)&client, len);

	printf("\nRequest sent for file list of client.\n");

	recvfrom(sockfd, buffer, BUFF_SIZE, MSG_WAITALL, (struct sockaddr*)&client, &len);
	printf("\nList of files stored in client: \n%s\n", buffer);

	//Storing the list of files in another file

	fp = fopen("Client1-FileList.txt", "w");

	if (fp < 0){
		perror("Error in opening file.");
        exit(1);
    }

	fprintf(fp, "%s", buffer);

	printf("List of files successfully written to: 'Client-FileList.txt'\n");

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