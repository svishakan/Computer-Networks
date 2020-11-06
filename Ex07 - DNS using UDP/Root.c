#include "DNS.h"

char *findTLD(char *req_server);

int main(int argc, char **argv){
	struct sockaddr_in local;
	int sockfd, n, addrlen;
	char req_server[100], reply[50], *tld_extn;

	sockfd = setUpConnection(&local, ROOT_PORT, 1, "local DNS server");

	addrlen = sizeof(local);
	printf("\nRoot server awaiting requests from local DNS server in port %d...\n", ROOT_PORT);

	while (1){
		bzero(&req_server, sizeof(req_server));
		recvfrom(sockfd, &req_server, sizeof(req_server), 0, (struct sockaddr *)&local, &addrlen);
		printf("\nReceived request from local DNS server for %s", req_server);

		tld_extn = findTLD(req_server);
		strcpy(reply, tld_extn);

		printf("\nReplied back with address to \"%s\" TLD.\n", toUppercase(tld_extn));
		sendto(sockfd, &reply, sizeof(reply), 0, (struct sockaddr *)&local, addrlen);
	}

	close(sockfd);
	return 0;
}

char *findTLD(char *req_server){
	//finds the appropriate TLD server for the given domain name

	char *server_copy, *split;

	server_copy = (char *)calloc(100, sizeof(char));

	strcpy(server_copy, req_server);
	split = strtok(server_copy, ".");
	split = strtok(NULL, ".");
	split = strtok(NULL, ".");

	return split;
}