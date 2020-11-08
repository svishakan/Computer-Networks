#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<netinet/tcp.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<netdb.h>

int connectToHost(char *host, in_port_t port);


int main(int argc, char *argv[]){
	int sockfd;
	char buffer[1024];

	if(argc < 3){
		//example: ./w www.google.com 80
		//80 is the HTTP port
		printf("Usage: %s <hostname> <port>\n", argv[0]);
		exit(1);
	}

	sockfd = connectToHost(argv[1], atoi(argv[2]));
	
	//sending a GET request to the domain to download the webpage
	
	//GET request to retrieve full webpage information
	//send(sockfd, "GET / \r\n\r\n", strlen("GET /\r\n\r\n"), 0);
	//\r\n\r\n used to denote carriage-return & newlines
	
	//HEAD request (same as GET) to get status line & header section only
	send(sockfd, "HEAD / HTTP/1.0\r\n\r\n", 23, 0);
	bzero(buffer, 1024);

	while(recv(sockfd, buffer, 1023, 0) != 0){
		//prints the reply recvd. from the webpage onto the terminal
		printf("%s", buffer);
		bzero(buffer, 1024);
	}

	//shutdown and close the socket
	shutdown(sockfd, SHUT_RDWR);
	close(sockfd);

	return 0;
}



int connectToHost(char *host, in_port_t port){
	struct hostent *he;
	struct sockaddr_in addr;
	int sockfd, on = 1;

	//getting the host entry for the given domain name
	if((he = gethostbyname(host)) == NULL){
		perror("GetHostByName error occurred.\n");
		exit(1);
	}

	//bcopy(s1, s2, n);
	//The bcopy() function shall copy n bytes from the area pointed to by s1 to the area pointed to by s2.
	//copying the host's IP address to local addr variable
	bcopy(he->h_addr, &addr.sin_addr, he->h_length);
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;

	//creating a TCP socket
	sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	//setting the TCP_NODELAY option on the sockfd
	setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));

	if(sockfd == -1){
		perror("SetSockOpt error occurred.\n");
		exit(1);
	}

	//Establishing a connection to the specified IP address & Port thro' the socket
	if(connect(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1){
		perror("Connect error occurred.\n");
		exit(1);
	}

	return sockfd;

}
