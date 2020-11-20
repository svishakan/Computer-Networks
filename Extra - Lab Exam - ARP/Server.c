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

arp makeARPPacket(void);

int main(int argc, char **argv){
	//Usage: gcc Server.c -o s
	//		 ./s
	
	struct sockaddr_in server, client;
	char buffer[1024];
	int sockfd, newfd;
	int k, i, len, count, child, c_id, n; 
	arp packet;

	/*---Taking the server side network details and developing an ARP request packet---*/

	packet = makeARPPacket();
	printf("\nDeveloping ARP Request packet\n");
	printf("\t%s", packet.PKT);
	printf("\n\nThe ARP Request packet is ready to be broacasted.\n");

	/*---Creating the socket and binding it to the server port---*/

	sockfd = socket(AF_INET, SOCK_STREAM, 0);		//IPv4, TCP socket

	if(sockfd < 0){
		perror("Unable to open socket.\n");
		exit(1);
	}
	else{
		printf("Socket opened successfully.\n");
	}

	bzero(&server, sizeof(server));

	server.sin_family = AF_INET;						//IPv4
	server.sin_addr.s_addr = inet_addr("127.0.0.1");	//Localhost
	server.sin_port = htons(7228);						//Port Number

	if(bind(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0){
		perror("Bind error occurred.\n");
		exit(1);
	}
	else{
		printf("Socket binded to the port successfully.\n");
	}

	listen(sockfd, 10);

	printf("\n------------------------------Server ready------------------------------\n");

	len = sizeof(client);

	/*---Client-Server communication---*/

	while(1){
		//To broadcast the ARP request packet to every connecting client
		newfd = accept(sockfd, (struct sockaddr *)&client, &len);
		child = fork();

		if(child == 0){
			//p_id = getpid();	//To identify the client with process ID

			/*---Sending acknowledgement & receiving client ID---*/

			bzero(buffer, sizeof(buffer));
			strcpy(buffer, "You are connected to server.");
			n = send(newfd, buffer, sizeof(buffer), 0);
			n = recv(newfd, &c_id, sizeof(c_id), 0);

			/*---Broadcasting ARP request packet & responding to the ARP reply packet, if received---*/

			bzero(buffer, sizeof(buffer));
			strcpy(buffer, packet.PKT);
			printf("\nSending ARP Request Packet to Client %d", c_id);
			n = send(newfd, buffer, sizeof(buffer), 0);
			
			bzero(buffer, sizeof(buffer));
			n = recv(newfd, buffer, sizeof(buffer), 0);
			
			if(strcmp(buffer, "Does not match!") == 0){
				//ARP reply was not obtained
				printf("\nClient %d did not match with the destination IP address!\n", c_id);
			}

			else{
				//ARP reply obtained, send file details to client.
				printf("\nClient %d matched with the destination IP address!", c_id);
				printf("\n\nARP Reply received:\n\t%s", buffer);
				
				printf("\nSending the file details to Client %d...\n", c_id);

				bzero(buffer, sizeof(buffer));
				strcpy(buffer, packet.FILE);
				n = send(newfd, buffer, sizeof(buffer), 0);
			}

		}
	}

	close(newfd);
	close(sockfd);

	return 0;
}

arp makeARPPacket(void){
	//Creates the ARP Request packet
	arp packet;

	printf("\nEnter the details of packet received.\n");
	printf("Enter the Source IP\t\t: ");
	scanf(" %s", packet.SRC_IP);
	printf("Enter the Source MAC\t\t: ");
	scanf(" %s", packet.SRC_MAC);
	printf("Enter the Destination IP\t: ");
	scanf(" %s", packet.DEST_IP);
	printf("Enter the File Name\t\t: ");
	scanf(" %s", packet.FILE);

	//Creating the packet
	strcpy(packet.PKT, packet.SRC_IP);
	strcat(packet.PKT, "|");
	strcat(packet.PKT, packet.SRC_MAC);
	strcat(packet.PKT, "|");
	strcat(packet.PKT, packet.DEST_IP);

	return packet; 
}
