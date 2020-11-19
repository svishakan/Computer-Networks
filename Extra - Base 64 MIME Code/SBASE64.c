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

void concatspaces(char *buff);
void splitBy6(char *buff, int *arr);
int binToDec(char *a, int start, int stop);

int main(int argc, char **argv)
{
    int n, len;
    struct sockaddr_in servaddr, cliaddr;
    int sockfd, newfd;
    char buff[BUFF_SIZE];
    char word_space[100];
    int ans[4] = {};
    int j = 0;

    for (int i = 0; i < 26; i++)
    {
        word_space[j++] = i + 'A';
    }

    for (int i = 0; i < 26; i++)
    {
        word_space[j++] = i + 'a';
    }

    for (int i = 0; i < 10; i++)
    {
        word_space[j++] = i + '0';
    }

    word_space[j++] = '+';
    word_space[j++] = '/';
    word_space[j] = 0;

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

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("Bind error ...");
        exit(1);
    }
    else
    {
        printf("\nBind is successful ...\n");
    }

    listen(sockfd, 5);

    len = sizeof(cliaddr);
    newfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len);
    printf("\nConnection established ... \n");

    while (1)
    {
        recv(newfd, buff, BUFF_SIZE, 0);
        printf("\nFrom Client : %s\n", buff);

        if (strcmp(buff, "exit") == 0)
        {
            break;
        }

        concatspaces(buff);
        splitBy6(buff, ans);

        bzero(buff, BUFF_SIZE);
        sprintf(buff, "%d %d %d %d", ans[0], ans[1], ans[2], ans[3]);
        printf("To Client : %s\n", buff);
        send(newfd, buff, BUFF_SIZE, 0);

        bzero(buff, BUFF_SIZE);
        sprintf(buff, "%c%c%c%c", word_space[ans[0]], word_space[ans[1]], word_space[ans[2]], word_space[ans[3]]);
        printf("          : %s\n", buff);
        send(newfd, buff, BUFF_SIZE, 0);
    }

    printf("\nServer Disconnected ...\n\n");
    close(sockfd);
    close(newfd);
    return 0;
}

void concatspaces(char *buff)
{
    char b[BUFF_SIZE];
    int i, j = 0;
    ;

    for (i = 0; buff[i]; i++)
    {
        if (buff[i] == ' ')
            continue;
        b[j++] = buff[i];
    }
    b[j] = 0;
    strcpy(buff, b);
}

void splitBy6(char *buff, int *arr)
{
    for (int i = 0; i < 4; i++)
    {
        arr[i] = binToDec(buff, i * 6, i * 6 + 6);
    }
}

int binToDec(char *a, int start, int stop)
{
    int val = 0;
    for (int i = start; i < stop; i++)
    {
        val = val * 2 + (a[i] - '0');
    }
    return val;
}

// char* decTObin(int val) {
//     char ans[10];
//     return ans;
// }
