#include "DNS.h"

char *findAuth(char *req_server);

int main(int argc, char **argv){
    struct sockaddr_in local;
	int sockfd, n, addrlen;
	char req_server[100], reply[50], *auth_extn;

	sockfd = setUpConnection(&local, TLD_PORT, 1, "local DNS server");

	addrlen = sizeof(local);
    printf("\nTLD server awaiting requests from local DNS server in port %d...\n", TLD_PORT);

	while(1){
	    bzero(&req_server, sizeof(req_server));
	    recvfrom(sockfd, &req_server, sizeof(req_server), 0, (struct sockaddr*)&local, &addrlen);
	    printf("\nReceived request from local DNS server for %s", req_server);

	    auth_extn = findAuth(req_server);
	    strcpy(reply, auth_extn);

	    printf("\nReplied back with address to %s's authoritative server.\n", toUppercase(auth_extn));
		sendto(sockfd, &reply, sizeof(reply), 0, (struct sockaddr*)&local, addrlen);

	}

	close(sockfd);
    return 0;
}

char *findAuth(char *req_server){
    //finds the appropriate authoritative server for the given domain name

	char *server_copy, *split;

	server_copy = (char *)calloc(100, sizeof(char));

	strcpy(server_copy, req_server);
	split = strtok(server_copy, ".");
	split = strtok(NULL, ".");

    return split;

}