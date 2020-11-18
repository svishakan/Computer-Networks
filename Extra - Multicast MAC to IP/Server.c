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

int checkIP(char *ip);
char *convertToMAC(char *ip);


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

    if(bind(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0){
        perror("Bind error");
        exit(1);
    }

    printf("Server awaiting clients on port %d\n", PORT);

    len = sizeof(client);

    while(1){
        recvfrom(sockfd, &pkt, sizeof(pkt), 0, (struct sockaddr*)&client, &len);
        printf("\nReceived an IP address: %s", pkt.ip);

        flag = checkIP(pkt.ip);

        if(!flag){
            //Invalid multicast IP address received
            printf("\n%s is not a valid multicast IP address.", pkt.ip);
            pkt.valid = 0;
        }

        else{
            mac = convertToMAC(pkt.ip);
            strcpy(pkt.mac, mac);
            printf("\n%s is the MAC address for the IP address %s\n", pkt.mac, pkt.ip);
            pkt.valid = 1;
        }

        sendto(sockfd, &pkt, sizeof(pkt), 0, (struct sockaddr*)&client, len);
    }

    return 0;
}


int checkIP(char *ip){
	//Checks if a given IP address is a valid ssmulticast address

	int valid = 1, byte, flag = 0;
	char *ip_copy, *split;

	ip_copy = (char *)calloc(100, sizeof(char));
	strcpy(ip_copy, ip);
	split = strtok(ip_copy, ".");

	while(split){ //split pointer points to each "byte" iteratively
		byte = atoi(split);
        if(flag == 0){
            //First byte should be between 239 & 224
            //Class D address
            if (byte > 239 || byte < 224){
			    return 0;
            }
            else{
                //Valid multicast
                flag = 1;
            }
		}

		if (byte < 0 || byte > 255){
            //Other bytes must be between 0 and 255
			return 0;
		}

		split = strtok(NULL, ".");
	}

	return 1;
}

char *convertToMAC(char *ip){
	//Converts the given IP address to a MAC address
	
	//In the Ethernet world, a multicast MAC address is distinguished by a binary '1' 
	//in the least significant bit of the first byte. 
	//For IP multicast specifically, the Ethernet prefix "01-00-5e" is reserved.


	int bit[4];
	char *mac;
	mac = (char *)malloc(sizeof(char) * 100);
	
	sscanf(ip, "%d.%d.%d.%d", &bit[0], &bit[1], &bit[2], &bit[3]);
	
	bit[1] = (bit[1] & (~(1 << 7)));	//Clear the MSB(8th bit) of bit[1]
	
	sprintf(mac, "01:00:5e:%02x:%02x:%02x", bit[1], bit[2], bit[3]);
	
	return mac;
}


/*
OUTPUT:

(base) vishakan@Legion:~/Desktop/Networks/MAC to IP$ gcc Server.c -o s
(base) vishakan@Legion:~/Desktop/Networks/MAC to IP$ ./s
Server awaiting clients on port 7228

Received an IP address: 238.212.24.9
01:00:5e:54:18:09 is the MAC address for the IP address 238.212.24.9

Received an IP address: 240.0.0.0
240.0.0.0 is not a valid multicast IP address.
Received an IP address: 223.0.0.0
223.0.0.0 is not a valid multicast IP address.
Received an IP address: 224.0.0.0
01:00:5e:00:00:00 is the MAC address for the IP address 224.0.0.0

Received an IP address: 239.0.0.0
01:00:5e:00:00:00 is the MAC address for the IP address 239.0.0.0

*/

