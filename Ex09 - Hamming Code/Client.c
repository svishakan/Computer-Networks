#include "Hamming.h"
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 7229

int main(void){
    int sockfd, flag, len, i;
    int *data, data_bits, parity_bits, total_bits;
	struct sockaddr_in server_address, client_address;
	char buffer[1024];

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(sockfd < 0){
		perror("Socket cannot be created.\n");
		exit(1);
	}

	bzero(&server_address, sizeof(server_address));

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_address.sin_port = htons(7229);

	connect(sockfd, (struct sockaddr*)&server_address, sizeof(server_address));

	flag = recv(sockfd, buffer, sizeof(buffer), 0);

	printf("Server sent the data\t:\t%s\n", buffer);

	total_bits = strlen(buffer);

    int hammed_data[total_bits];

	for(i = 0; i < total_bits; i++){
        hammed_data[i] = buffer[i] - '0';
	}

	parity_bits = findParityBits(total_bits);

	printf("\nChecking for errors in data.");
	data = detectError(hammed_data, total_bits, parity_bits);

	printf("\nRetrieving the original message.");
	data = getMessage(data, total_bits, parity_bits);

    printf("\nOriginal message\t:\t");
    printMessage(data, total_bits - parity_bits);
	printf("\n");


	close(sockfd);


    return 0;
}