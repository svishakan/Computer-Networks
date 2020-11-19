#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT 7223

void fileDumper(int c_id, char *person, char *msg);

int main(int argc, char **argv){
    //Usage: ./c <client_id>

	int len, c_id;
	int sockfd,n,newfd;
	struct sockaddr_in servaddr,cliaddr;
	char str[1024];
	char buff[1024];

    c_id = atoi(argv[1]);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(sockfd < 0){
		perror("Socket error.");
        exit(1);
    }

	bzero(&servaddr,sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);


	connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

    n = send(sockfd, &c_id, sizeof(c_id), 0);       //Send the client ID to the server
	
	while(strcmp(buff, "Bye") != 0){    //chat
		bzero(buff, 1024);
		n = recv(sockfd,buff,sizeof(buff), 0);
		printf("Server:\t%s\n",buff);
		fileDumper(c_id, "Server", buff);

		bzero(buff, 1024);
		printf("Client:\t");
		scanf(" %[^\n]",buff);

		n = send(sockfd,buff,sizeof(buff), 0);
        fileDumper(c_id, "Client", buff);
	}

	printf("Client Exited...\n");

	close(sockfd);
	return 0;
}

void fileDumper(int c_id, char *person, char *msg){
    //To perform file dumping of the chat log

    FILE *fp;
    char *fname;
    
    sprintf(fname, "Chat Log - Client %d", c_id);

    fp = fopen(fname, "a+");
    fprintf(fp, "%s:\t\t%s\n", person, msg);

    fclose(fp);
}
