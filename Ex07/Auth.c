#include "DNS.h"

char *findIP(char *req_server);

int main(int argc, char **argv){
    struct sockaddr_in local;
	int sockfd, n, addrlen;
	char req_server[100], reply[50], *ip;

	sockfd = setUpConnection(&local, AUTH_PORT, 1, "local DNS server");

	addrlen = sizeof(local);
    printf("\nAuthoritative server awaiting requests from local DNS server in port %d...\n", AUTH_PORT);

	while(1){
	    bzero(&req_server, sizeof(req_server));
	    recvfrom(sockfd, &req_server, sizeof(req_server), 0, (struct sockaddr*)&local, &addrlen);
	    printf("\nReceived request from local DNS server for %s", req_server);

	    ip = findIP(req_server);

	    if(ip == NULL){
	        printf("\nReplied back with address to %s authoritative server.\n", empty);
	        sendto(sockfd, &empty, sizeof(empty), 0, (struct sockaddr*)&local, addrlen);
	    }
	    else{
	        strcpy(reply, ip);
	        printf("\nReplied back with address to %s's authoritative server.\n", ip);
		    sendto(sockfd, &reply, sizeof(reply), 0, (struct sockaddr*)&local, addrlen);
	    }

	}

	close(sockfd);
    return 0;
}

char *findIP(char *req_server){
    //fetches the IP address for the given domain name
    //returns NULL if IP address does not exist

	struct hostent *he;
	char *ip;

	he = gethostbyname(req_server);		//gets the host entry for the domain name

	if(he == NULL){						//if host entry does not exist for the domain
		return NULL;
	}

	ip = inet_ntoa(*((struct in_addr *)he->h_addr_list[0]));	//get the IP address from host entry

    return ip;
}