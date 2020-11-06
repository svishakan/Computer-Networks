#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char **argv){
	int sockfd, flag, newfd, i = 0, len;
	struct sockaddr_in server, client;
	FILE *fp;
	char file_name[100];
	char buffer[1024];

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(sockfd < 0){
		perror("\nSocket cannot be created.");
		exit(0);
	}

	bzero(&server, sizeof(server));

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(7228);

	if(bind(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0){
		perror("\nBind error occurred.");
		exit(0);
	}

	printf("Waiting for client...\n");

	listen(sockfd, 2);

	len = sizeof(client);
	newfd = accept(sockfd, (struct sockaddr*)&client, &len);
	
	flag = read(newfd, file_name, sizeof(file_name));

	printf("File to be read: %s", file_name);

	fp = open(file_name, O_RDONLY);		//Open the specified file in read-only mode

	if(fp < 0){		//If file does not exist.
		perror("\nSpecified file could not be found.\n");
		strcpy("Error 404", buffer);
		return 1;
	}

	else{
		len = read(fp, buffer, 1024);	//len stores the no. of characters read by read()
		close(fp);

		flag = write(newfd, buffer, len);
		printf("\nFile contents transferred.\n");
	}
	
	close(newfd);
	close(sockfd);

	return 0;
}