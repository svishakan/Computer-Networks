#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

int main(int argc, char **argv){
	int sockfd, newfd, len, flag;
	struct sockaddr_in server_address, client_address;
	char buffer[1024];

	sockfd = socket(AF_INET, SOCK_STREAM, 0);	//AF_INET : IPv4 Protocol, SOCK_STREAM : reliable 2-way connection based service
	//socket is a file descriptor that lets an applicaton R/W data from/to the network

	if(sockfd < 0){		//Error has occurred.
		perror("Socket cannot be created.\n");
		exit(1);
	}

	bzero(&server_address, sizeof(server_address));	//Erases the data pointed to in the server_address by writing 0s
	
	server_address.sin_family = AF_INET;			//Use the Internet address family, AF_INET : IPv4 Protocol
	server_address.sin_addr.s_addr = 115.97.100.118;	//IP Address
	server_address.sin_port = htons(7228);			//Port Number; htons: host byte order -> network byte order, short

	if(bind(sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0){
		//Binding the socket to the port with server_address
		perror("Bind error occurred.\n");
	}

	listen(sockfd, 2);		//indicates that server will accept a conection. Parameter 2 indicates backlog (max # of active participants that can wait for a connection)
	len = sizeof(client_address);
	newfd = accept(sockfd, (struct sockaddr*)&client_address, &len);
	//Accepts the first request on queue, creates another socket with the same props. of sockfd.
	//If no connection request pending, blocks the server until it receives connection request from client.
	//newfd : the new socket used for data transfer

	flag = read(newfd, buffer, sizeof(buffer));
	//Reads on the socket

	printf("Received message is: %s", buffer);
	printf("\n");

	close(sockfd);
	close(newfd);	//Close the sockets

	return 0;
}


/*
OUTPUT:

vishakan@Legion:~/Desktop/$ ./s

Received message is: Hello World!

*/


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