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

struct ARP_PACKET{
	char SRC_IP[100];
	char DEST_IP[100];
	char SRC_MAC[100];
	char DEST_MAC[100];
	char FILE[100];
	char PKT[600];
};

typedef struct ARP_PACKET arp;

arp createARPPacket(void);

int main(int argc, char **argv){
	struct sockaddr_in server, client;
	char buffer[1024];
	int sockfd, newfd;
	int k, i, len, count, child, p_id, n; 
	fd_set newfds;
	arp packet;

	packet = createARPPacket();
	printf("\nDeveloping ARP Request packet\n");
	printf("\t%s", packet.PKT);
	printf("\nThe ARP Request packet is ready to be broacasted.\n");

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(sockfd < 0){
		perror("Unable to open socket.\n");
	}

	bzero(&server, sizeof(server));

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(7228);

	if(bind(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0){
		perror("Bind error occurred.\n");
	}

	listen(sockfd, 10);

	len = sizeof(client);

	while(1){
		newfd = accept(sockfd, (struct sockaddr *)&client, &len);
		child = fork();

		if(child == 0){
			p_id = getpid();

			bzero(buffer, sizeof(buffer));
			strcpy(buffer, packet.PKT);
			printf("\nSending ARP Request Packet to Client %d", p_id);
			n = send(newfd, buffer, sizeof(buffer), 0);
			
			bzero(buffer, sizeof(buffer));
			n = recv(newfd, buffer, sizeof(buffer), 0);
			
			if(strcmp(buffer, "Not matched!") == 0){
				printf("\nClient %d did not match with the destination IP address!\n", p_id);
			}

			else{
				printf("\nClient %d matched with the destination IP address!", p_id);
				printf("\nARP Reply received:\n\t%s", buffer);
				
				printf("\nSending the file details to Client %d...\n", p_id);

				bzero(buffer, sizeof(buffer));
				strcpy(buffer, packet.FILE);
				n = send(newfd, buffer, sizeof(buffer), 0);
				break;
			}

		}
	}

	close(newfd);
	close(sockfd);

	return 0;
}

arp createARPPacket(void){
	arp packet;

	printf("\nEnter the details of packet received.\n");
	printf("Source IP\t: ");
	scanf(" %s", packet.SRC_IP);
	printf("Source MAC\t: ");
	scanf(" %s", packet.SRC_MAC);
	printf("Destination IP\t: ");
	scanf(" %s", packet.DEST_IP);
	printf("File Name\t: ");
	scanf(" %s", packet.FILE);

	strcpy(packet.PKT, packet.SRC_IP);
	strcat(packet.PKT, "|");
	strcat(packet.PKT, packet.SRC_MAC);
	strcat(packet.PKT, "|");
	strcat(packet.PKT, packet.DEST_IP);

	return packet; 
}