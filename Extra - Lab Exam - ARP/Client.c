/*
1. Write a socket program for simple client server connectivity using TCP.
2. Create 3 clients and a server. Exchange message between them.
3. Create an ARP request packet and the matching client will send ARP reply. Use structures.
4. To the matched client, server will send a file. Client will read and display it.
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
	//ARP Packet Structure
	char SRC_IP[100];
	char DEST_IP[100];
	char SRC_MAC[100];
	char DEST_MAC[100];
	char FILE[100];
	char PKT[600];
};

typedef struct ARP_PACKET arp;

void fileDumper(char *fname);

int main(int argc, char **argv){
	//Usage: gcc Client.c -o c
	//		 ./c <client_id>

	struct sockaddr_in server, client;
	char buffer[1024];
	int sockfd, newfd;
	int len, i, count, k, c_id;
	arp packet;

	/*---Taking the client side network details---*/

	c_id = atoi(argv[1]);					//Client ID
	printf("\nEnter the IP Address\t: ");	//Client IP
	scanf("%s", packet.DEST_IP);
	printf("\nEnter the MAC Address\t: ");	//Client MAC
	scanf("%s", packet.DEST_MAC);

	/*---Creating the socket and connecting it to the server port---*/

	sockfd = socket(AF_INET, SOCK_STREAM, 0);	//IPv4, TCP socket

	if(sockfd < 0){
		perror("Unable to open socket.\n");
		exit(1);
	}
	else{
		printf("\nSocket opened successfully.\n");
	}

	bzero(&server, sizeof(server));

	server.sin_family = AF_INET;						//IPv4
	server.sin_addr.s_addr = inet_addr("127.0.0.1");	//Localhost
	server.sin_port = htons(7228);						//Port number

	connect(sockfd, (struct sockaddr*)&server, sizeof(server));
	len = sizeof(client);

	printf("\n------------------------------Client ready------------------------------\n");

	/*---Sending client identifier to server on connection---*/

	bzero(buffer, sizeof(buffer));
	recv(sockfd, buffer, sizeof(buffer), 0);
	send(sockfd, &c_id, sizeof(c_id), 0);
	printf("\nServer ACK: %s\n", buffer);

	/*---Receiving ARP request packet & responding to it---*/

	bzero(buffer, sizeof(buffer));
	recv(sockfd, buffer, sizeof(buffer), 0);
	printf("\nARP Request Received: %s\n", buffer);

	count = 0;
	k = 0;
	for(i = 0; buffer[i]; i++){
		//Getting the IP from the received ARP packet
		if(count == 2){
			packet.SRC_IP[k++] = buffer[i];
		}
		if(buffer[i] == '|'){
			count++;
		}
	}

	packet.SRC_IP[k] = '\0';

	if(strcmp(packet.SRC_IP, packet.DEST_IP) == 0){
		//Comparing the IP addresses, case for matched destination IP address
		printf("\nDestination IP Address matches.\n");
		strcat(buffer, "|");
		strcat(buffer, packet.DEST_MAC);
		send(sockfd, buffer, sizeof(buffer), 0);
		printf("\nARP Reply Sent: %s\n", buffer);

		bzero(buffer, sizeof(buffer));
		recv(sockfd, buffer, sizeof(buffer), 0);
		printf("\nReceived File is: %s\n", buffer);
		fileDumper(buffer);
	}

	else{
		//Case for unmatched destination IP address
		printf("\nDestination IP Address does not match.\n");
		bzero(buffer, sizeof(buffer));
		strcpy(buffer, "Does not match!");
		send(sockfd, buffer, sizeof(buffer), 0);
	}

	close(sockfd);

	return 0;
}

void fileDumper(char *fname){
	//To print the contents of the file specified by the server
	int src, n;
	char buffer[1024];
	
	src = open(fname, O_RDONLY);	//opening a file in Read-Only mode

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
