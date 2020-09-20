#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 7228

int main(int argc, char **argv){
	time_t cur_time;
	struct sockaddr_in serv_addr, cli_addr;
	int sockfd, n, addrlen;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	if(sockfd < 0){
		perror("Error in opening socket.\n");
		exit(1);
	}

	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(PORT);

	if(bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
		//Binding the socket to the port with serv_addr
		perror("Bind error occurred.\n");
		exit(1);
	}

	printf("Server awaiting clients on port %d...\n", PORT);
	
	addrlen = sizeof(cli_addr);	
	
	while(1){	//server is always up
		recvfrom(sockfd, &n, sizeof(n), 0, (struct sockaddr*)&cli_addr, &addrlen);
		cur_time = time(NULL);
		printf("\nClient %d sent a request through port %d.\n", n, PORT);
		sendto(sockfd, &cur_time, sizeof(cur_time), 0, (struct sockaddr*)&cli_addr, addrlen);
		printf("Sent current time: %s", ctime(&cur_time));
	}

	return 0;
}