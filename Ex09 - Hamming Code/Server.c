#include "Hamming.h"
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 7229

int main(void){
    int sockfd, newfd, len, flag, i, *hammed_data, *error_data;
    int data_bits, total_bits, parity_bits;
	struct sockaddr_in server_address, client_address;
	char buffer[1024];

	printf("\n\t\tHamming Code\n");
	printf("\nEnter the no. of data bits\t:\t");
	scanf("%d", &data_bits);

	printf("\nEnter the data\t\t\t:\t");

	int data[data_bits];

	for(i = 0; i < data_bits; i++){
	    scanf("%1d", &data[i]);
	}

	parity_bits = findParityBits(data_bits);
	printf("\nThe no. of parity bits\t\t:\t%d", parity_bits);
	total_bits = parity_bits + data_bits;

	hammed_data = putParityBits(data, data_bits);
	printf("\nHamming Encoded Data\t\t:\t");
	printMessage(hammed_data, total_bits);

	printf("\nSimulating error by flipping a random bit.");
	error_data = flipABit(hammed_data, total_bits);

	printf("\nData with error\t\t\t:\t");
	printMessage(error_data, total_bits);

	for(i = 0; i < total_bits; i++){
		buffer[i] = error_data[i] + '0';
	}

	buffer[i] = '\0';

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(sockfd < 0){		//Error has occurred.
		perror("Socket cannot be created.\n");
		exit(1);
	}

	bzero(&server_address, sizeof(server_address));

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(PORT);

	if(bind(sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0){
		perror("Bind error occurred.\n");
		exit(1);
	}

	printf("\n\nWaiting for client at port %d...\n", PORT);
	listen(sockfd, 2);
	len = sizeof(client_address);

	newfd = accept(sockfd, (struct sockaddr*)&client_address, &len);

    flag = send(newfd, buffer, sizeof(buffer), 0);
    printf("\nSent the data\t\t:\t\t%s\n", buffer);

	close(sockfd);
	close(newfd);

    return 0;
}
