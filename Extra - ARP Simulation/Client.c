/*
ARP Simulation using TCP - Fork Method
and File Transfer
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>

struct ARP_PACKET{
	char SRC_IP[100];
	char DEST_IP[100];
	char SRC_MAC[100];
	char DEST_MAC[100];
	char FILE[100];
	char PKT[600];
};

typedef struct ARP_PACKET arp;

void fileOutputDump(char *fname);

int main(int argc, char **argv){
	struct sockaddr_in server, client;
	char buffer[1024];
	int sockfd, newfd;
	int len, i, count, k;
	arp packet;

	printf("\nEnter the IP Address\t: ");
	scanf("%s", packet.DEST_IP);
	printf("\nEnter the MAC Address\t: ");
	scanf("%s", packet.DEST_MAC);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(sockfd < 0){
		perror("Unable to open socket.\n");
	}

	bzero(&server, sizeof(server));

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(7228);

	connect(sockfd, (struct sockaddr*)&server, sizeof(server));
	len = sizeof(client);

	bzero(buffer, sizeof(buffer));
	recv(sockfd, buffer, sizeof(buffer), 0);
	printf("\nARP Request Received: %s\n", buffer);

	count = 0;
	k = 0;
	for(i = 0; buffer[i]; i++){
		if(count == 2){
			packet.SRC_IP[k++] = buffer[i];
		}
		if(buffer[i] == '|'){
			count++;
		}
	}

	packet.SRC_IP[k] = '\0';

	if(strcmp(packet.SRC_IP, packet.DEST_IP) == 0){
		printf("\nIP Address matches.\n");
		strcat(buffer, "|");
		strcat(buffer, packet.DEST_MAC);
		send(sockfd, buffer, sizeof(buffer), 0);
		printf("\nARP Reply Sent: %s\n", buffer);

		bzero(buffer, sizeof(buffer));
		recv(sockfd, buffer, sizeof(buffer), 0);
		printf("\nReceived File is: %s\n", buffer);
		fileOutputDump(buffer);
	}

	else{
		printf("\nIP Address does not match.\n");
		bzero(buffer, sizeof(buffer));
		strcpy(buffer, "Not matched!");
		send(sockfd, buffer, sizeof(buffer), 0);
	}

	close(sockfd);

	return 0;
}

void fileOutputDump(char *fname){
	//To print the contents of the file specified by the server
	int src, n;
	char buffer[1024];
	
	src = open(fname, O_RDONLY);

	if(src == -1){
		printf("\nSpecified file does not exist.\n");
	}

	else{
		printf("\nFile contents:\n");

		while((n = read(src, buffer, sizeof(buffer))) != 0){
			buffer[n] = '\0';	//To terminate the string
			printf("%s", buffer);
		}
		
		printf("\n");
		close(src);
	}
}