#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

int main(int argc, char **argv){
	int sockfd, newfd, len, flag, curfd;
	struct sockaddr_in server_address, client_address;
	char buffer[1024];
	pid_t child;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);	//AF_INET : IPv4 Protocol, SOCK_STREAM : reliable 2-way connection based service
	//socket is a file descriptor that lets an applicaton R/W data from/to the network

	if(sockfd < 0){		//Error has occurred.
		perror("Socket cannot be created.\n");
		exit(1);
	}

	bzero(&server_address, sizeof(server_address));	//Erases the data pointed to in the server_address by writing 0s
	
	server_address.sin_family = AF_INET;			//Use the Internet address family, AF_INET : IPv4 Protocol
	server_address.sin_addr.s_addr = INADDR_ANY;	//IP Address
	server_address.sin_port = htons(7229);			//Port Number; htons: host byte order -> network byte order, short

	if(bind(sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0){
		//Binding the socket to the port with server_address
		perror("Bind error occurred.\n");
		exit(1);
	}

	printf("Waiting for client...\n");
	listen(sockfd, 10);		//indicates that server will accept a conection. Parameter 2 indicates backlog (max # of active participants that can wait for a connection)
	len = sizeof(client_address);

	while(1){	//server is always up
		newfd = accept(sockfd, (struct sockaddr*)&client_address, &len);

		if(newfd < 0){
			printf("Error Occurred\n");
			return;
		}

		if((child = fork()) == 0){	//child process becomes new server
			int p_id = getpid();
			close(sockfd);			//closing sockfd since child doesn't handle it

			while(1){				//parent process handles connected client
				bzero(buffer, sizeof(buffer));
				recv(newfd, buffer, 1024, 0);

				if(strcmp(buffer, "!bye") == 0){	//if "!bye" is received
					printf("Client %d says: %s\n", p_id, buffer);
					printf("Disconnected from Client %d.\n", p_id);
					close(newfd);
					return;
				}
				else{
					printf("Client %d says: %s\n", p_id, buffer);
					bzero(buffer, sizeof(buffer));
					printf("Server response: ");
					scanf(" %[^\n]", buffer);
					send(newfd, buffer, strlen(buffer), 0);

					if(strcmp(buffer, "!bye") == 0){	//if "!bye" is sent
						printf("Disconnected from Client %d.\n", p_id);
						close(newfd);
						return;
					}
				}
			}
		}

	}

	close(newfd);

	return 0;
}


/*
socket(int domain, int type, int protocol)
PARAMETER VALUES
 
Domain/family : integer, communication domain

AF_INET 		: IPv4 Protocol
AF_INET6		: IPv6 Protocol
AF_LOCAL		: Unix Domain Protocols
AF_ROUTE		: Routing Sockets
AF_KEY			: Key Socket

Type          : communication type

SOCK_STREAM		: reliable, 2-way communication service
SOCK_DGRAM		: unreliable, connectionless service
SOCK_RAW		: Security

Protocol      : specifies protocol, usually set to 0

*/
