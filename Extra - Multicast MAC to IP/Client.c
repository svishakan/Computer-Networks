#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT 7228

struct packet{
    char ip[100];
    char mac[100];
    int valid;
};

typedef struct packet PKT;

int main(void){
    int sockfd, newfd, n, len, flag;
    char buff[1024], *mac;
    struct sockaddr_in server, client;
    PKT pkt;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if(sockfd == -1){
        perror("Socket error");
        exit(1);
    }
    
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);
    
    len = sizeof(server);
    
    while(1){
        printf("\nEnter the Multicast IP Address: ");
        scanf("%s", pkt.ip);
        sendto(sockfd, &pkt, sizeof(pkt), 0, (struct sockaddr *)&server, len);
        recvfrom(sockfd, &pkt, sizeof(pkt), 0, (struct sockaddr *)&server, &len);
        if(pkt.valid == 0){
            printf("\nThe IP address %s was not a valid multicast address.\n", pkt.ip);
        }
        else{
            printf("\nMulticast MAC address: %s\n", pkt.mac);
        }
        
    }
    

    return 0;
}


/*
OUTPUT:

(base) vishakan@Legion:~/Desktop/Networks/MAC to IP$ gcc Client.c -o c
(base) vishakan@Legion:~/Desktop/Networks/MAC to IP$ ./c

Enter the Multicast IP Address: 238.212.24.9

Multicast MAC address: 01:00:5e:54:18:09

Enter the Multicast IP Address: 240.0.0.0

The IP address 240.0.0.0 was not a valid multicast address.

Enter the Multicast IP Address: 223.0.0.0

The IP address 223.0.0.0 was not a valid multicast address.

Enter the Multicast IP Address: 224.0.0.0

Multicast MAC address: 01:00:5e:00:00:00

Enter the Multicast IP Address: 239.0.0.0

Multicast MAC address: 01:00:5e:00:00:00


*/
