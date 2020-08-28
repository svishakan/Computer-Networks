#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char **argv){
	int sockfd, flag, i = 0, len;
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
	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_port = htons(7228);

	printf("\nEnter the File Location: ");
	scanf(" %[^\n]", file_name);

	connect(sockfd, (struct sockaddr*)&server, sizeof(server));

	flag = write(sockfd, file_name, sizeof(file_name));
	flag = read(sockfd, buffer, sizeof(buffer));

	if(strcmp(buffer, "Error 404") == 0){	//if file was not found
		printf("File %s was not found.\n", file_name);
		close(sockfd);
		exit(0);
	}

	printf("File contents transferred.");

	fp = open("newfile.txt", O_WRONLY|O_CREAT, S_IRWXU);	
	//create a newfile to store the content in client machine with proper permissions

	len = strlen(buffer);
	write(fp, buffer, len);
	
	close(fp);

	printf("\nContents of %s written to \"newfile.txt.\"\n", file_name);
	
	close(sockfd);

	return 0;
}