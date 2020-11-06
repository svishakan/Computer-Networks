#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

int main(int argc, char **argv){
	int sockfd, flag, len;
	struct sockaddr_in server_address, client_address;
	char buffer[1024];

	sockfd = socket(AF_INET, SOCK_STREAM, 0);	//AF_INET : IPv4 Protocol, SOCK_STREAM : reliable 2-way connection based service
	//socket is a file descriptor that lets an applicaton R/W data from/to the network

	if(sockfd < 0){		//Error has occurred.
		perror("Socket cannot be created.\n");
		exit(1);
	}

	bzero(&server_address, sizeof(server_address));

	server_address.sin_family = AF_INET;					//Use the Internet address family, AF_INET : IPv4 Protocol
	server_address.sin_addr.s_addr = inet_addr(argv[1]);	//IP Address, the argument to be entered is the system's IPv4 Address (use ifconfig/ipconfig and find the 'inet' parameter)
	server_address.sin_port = htons(7229);					//Port Number; ntohs: network byte order -> host byte order, short

	connect(sockfd, (struct sockaddr*)&server_address, sizeof(server_address));
	//Attempts to make a connection on a socket.

	while(1){	//client disconnects once "!bye" is sent or received
		printf("Client says : ");
		bzero(buffer, sizeof(buffer));
		scanf(" %[^\n]", buffer);
		send(sockfd, buffer, strlen(buffer), 0);

		if(strcmp(buffer, "!bye") == 0){	//if "!bye" is sent
			close(sockfd);
			printf("Disconnected from server.\n");
			break;
		}

		else{
			bzero(buffer, sizeof(buffer));
			len = recv(sockfd, buffer, 1024, 0);
			printf("Server response: %s\n", buffer);

			if(strcmp(buffer, "!bye") == 0){	//if "!bye" is received
				close(sockfd);
				printf("Disconnected from server.\n");
				break;
			}
		}
	}

	return 0;
}