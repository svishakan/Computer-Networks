#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT 7223

int checkIP(char *buff);

int main(int argc, char **argv){

	int len;
	int sockfd, newfd, n;
	struct sockaddr_in servaddr, cliaddr;
	char buff[1024];
	char str[1024];
	pid_t child;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){
		perror("Socket error.");
        exit(1);
    }

	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);

	if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
		perror("Bind error.");
        exit(1);
    }

	listen(sockfd, 5);

	len = sizeof(cliaddr);

	while (1){
		newfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len);
        child = fork();

		if (child == 0){
            int p_id, flag = 1;
			//p_id = getpid();

			strcpy(buff, "You are connected to the server.");
            n = send(newfd, buff, sizeof(buff), 0);
            n = recv(newfd, &p_id, sizeof(p_id), 0);    //get the client ID from the client

			while (flag){   //chat
				bzero(buff, 1024);
				n = recv(newfd, buff, sizeof(buff), 0);

				printf("Client %d:\t%s\n", p_id, buff);

				printf("Server:\t\t");

				if (strncmp(buff, "Bye", 4) == 0){
					strcpy(buff, "Bye");
					flag = 0;
                    printf("%s\n", buff);
					printf("\nConnection closed with Client %d...\n", p_id);
				}

                else if(checkIP(buff) == 1){
                    bzero(buff, 1024);
                    strcpy(buff, "You entered a valid IP address!");
                    printf("%s\n", buff);
                }

				else{
					bzero(buff, 1024);
					scanf(" %[^\n]", buff);
				}

				n = send(newfd, buff, sizeof(buff), 0);
			}
		}
	}
}

int checkIP(char *buff){
    //Checks if the client sent a valid IP address
    
    int byte, count = 0;
    char *ip, *split;
    ip = (char *)malloc(sizeof(char) * 1024);

    strcpy(ip, buff);

    split = strtok(ip, ".");

    while(split){
        byte = atoi(split);
        
        if(byte < 0 || byte > 255){
            return 0;
        }

        else{
            count++;
            split = strtok(NULL, ".");
        }
    }

    if(count == 4){
        return 1;
    }

    else{
        return 0;
    }
}