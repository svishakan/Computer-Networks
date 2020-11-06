#include "DNS.h"

int main(int argc, char **argv){
	struct sockaddr_in server;
	int sockfd, n, addrlen, flag, choice = 1;
	char req_server[100], req_ip[50];

	sockfd = setUpConnection(&server, CLI_PORT, 0, "local DNS server");

	addrlen = sizeof(server);

	while (choice){
		printf("\nEnter the Server's Name :\t");
		scanf("%s", req_server);
		sendto(sockfd, &req_server, sizeof(req_server), 0, (struct sockaddr *)&server, sizeof(server));

		recvfrom(sockfd, &req_ip, sizeof(req_ip), 0, (struct sockaddr *)&server, &addrlen);
		printf("The IP Address is :\t\t%s\n", req_ip);

		printf("\nContinue? (1 - YES, 0 - NO) -> ");
		scanf("%d", &choice);
	}

	close(sockfd);

	return 0;
}
