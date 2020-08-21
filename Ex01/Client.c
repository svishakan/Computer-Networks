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
	}

	bzero(&server_address, sizeof(server_address));

	server_address.sin_family = AF_INET;					//Use the Internet address family, AF_INET : IPv4 Protocol
	server_address.sin_addr.s_addr = inet_addr(argv[1]);	//IP Address, the argument to be entered is the system's IPv4 Address (use ifconfig/ipconfig and find the 'inet' parameter)
	server_address.sin_port = htons(7228);					//Port Number; ntohs: network byte order -> host byte order, short

	connect(sockfd, (struct sockaddr*)&server_address, sizeof(server_address));
	//Attempts to make a connection on a socket.

	printf("\nEnter a message: ");
	scanf("%[^\n]", buffer);
	flag = write(sockfd, buffer, sizeof(buffer));
	//Writes on the socket

	close(sockfd);	//Close the socket

	return 0;
}

/*
OUTPUT:

vishakan@Legion:~/Desktop/$ ./c 192.168.1.4

Enter a message: Hello World!

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

SOCK_STREAM		: reliable, 2-way communication service	- TCP
SOCK_DGRAM		: unreliable, connectionless service	- UDP
SOCK_RAW		: Security

Protocol      : specifies protocol, usually set to 0

*/