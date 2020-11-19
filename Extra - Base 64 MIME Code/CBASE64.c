/*
Credits to Venkataraman Nagarajan 
for contributing this code to my repository
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 7869
#define BUFF_SIZE 1024

int isvalidIP(char *IP);

int main(int argc, char **argv)
{
    int n, len;
    struct sockaddr_in servaddr, cliaddr;
    int sockfd, newfd;
    char buff[BUFF_SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("cannot create socket ..");
        exit(1);
    }
    else
    {
        printf("\nSocket is established ...\n");
    }

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);

    connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    printf("\nConnection established ...\n ");
    
    while (1)
    {

        printf("\nEnter a 24-Bit Data/ (exit): ");
        scanf(" %[^\n]", buff);

        printf("\nTo server : %s\n", buff);
        send(sockfd, buff, BUFF_SIZE, 0);
        if (strcmp(buff, "exit") == 0)
        {
            break;
        }

        bzero(buff, BUFF_SIZE);
        recv(sockfd, buff, BUFF_SIZE, 0);
        printf("\nFrom Server: \n");
        printf("Decimal Data : %s\n", buff);

        bzero(buff, BUFF_SIZE);
        recv(sockfd, buff, BUFF_SIZE, 0);
        printf("Actual Word from Mapper : %s\n", buff);
    }
    printf("\nClient Disconnected ... \n\n");
    close(sockfd);
    close(newfd);
    return 0;
}
