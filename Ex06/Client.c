#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>

struct ARP_PACKET{
	char SRC_IP[100];
	char DEST_IP[100];
	char SRC_MAC[100];
	char DEST_MAC[100];
	char DATA[16];
	char PKT[600];
};

typedef struct ARP_PACKET arp;

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
	server.sin_addr.s_addr = inet_addr(argv[1]);
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
		printf("\nReceived Packet is: %s\n", buffer);
	}

	else{
		printf("\nIP Address does not match.\n");
	}

	close(sockfd);

	return 0;
}