#include "DNS.h"

dns_table *local;

int cli_fd, root_fd, tld_fd, auth_fd;
struct sockaddr_in client_addr, local_addr, root_addr, tld_addr, auth_addr;

char *iterativeQuery(char *req_server);

int main(int argc, char **argv){

	int n, addrlen, flag;
	char req_server[100], *ip, req_ip[50];

	local = initTable(local, "Local");
	printTable(local);

	printf("\n\nDo you wish to alter the allocation table? (1 - YES, 0 - NO) -> ");
	scanf("%d", &flag);

	if (flag == 1){
		updateTable(local);
		printTable(local);
	}

	cli_fd = setUpConnection(&local_addr, CLI_PORT, 1, "client");				 //bind here since server
	root_fd = setUpConnection(&root_addr, ROOT_PORT, 0, "root server");			 //do not bind here
	tld_fd = setUpConnection(&tld_addr, TLD_PORT, 0, "top-level domain server"); //do not bind here
	auth_fd = setUpConnection(&auth_addr, AUTH_PORT, 0, "authoritative server"); //do not bind here

	printf("\nLocal DNS Server awaiting clients on port %d...\n", CLI_PORT);

	addrlen = sizeof(client_addr);

	while (1){
		bzero(req_server, sizeof(req_server));
		recvfrom(cli_fd, &req_server, sizeof(req_server), 0, (struct sockaddr *)&client_addr, &addrlen);
		printf("\nReceived a request for IP Address of %s from a client.\n", req_server);

		ip = iterativeQuery(req_server);

		if (ip == NULL){ //IP address does not exist
			strcpy(req_ip, empty);
			sendto(cli_fd, &empty, sizeof(empty), 0, (struct sockaddr *)&client_addr, addrlen);
		}
		else{
			//pointer -> char_array conversion is necessary since pointer only sends 8 bits of data
			strcpy(req_ip, ip);
			sendto(cli_fd, &req_ip, sizeof(req_ip), 0, (struct sockaddr *)&client_addr, addrlen);
		}

		printf("\nReplied with IP Address %s\n", req_ip);
	}

	return 0;
}

char *iterativeQuery(char *req_server){
	//Queries the DNS table according to the iterative resolution method.

	char *address, reply[50], request[100];
	int addrlen;

	strcpy(request, req_server);

	printf("\nSearching the %s DNS table...", local->name);
	address = fetchAddress(local, request);
	addrlen = sizeof(root_addr);

	if (address == NULL){
		printf("\nLocal table does not have an entry for %s, requesting root...", req_server);

		bzero(&reply, sizeof(reply));
		sendto(root_fd, &request, sizeof(request), 0, (struct sockaddr *)&root_addr, addrlen);
		recvfrom(root_fd, &reply, sizeof(reply), 0, (struct sockaddr *)&root_addr, &addrlen);

		printf("\nRoot replied with address to ping \"%s\" TLD server.", toUppercase(reply));

		bzero(&reply, sizeof(reply));
		sendto(tld_fd, &request, sizeof(request), 0, (struct sockaddr *)&tld_addr, addrlen);
		recvfrom(tld_fd, &reply, sizeof(reply), 0, (struct sockaddr *)&tld_addr, &addrlen);

		printf("\nTLD replied with address to ping %s's authoritative server.", toUppercase(reply));

		bzero(&reply, sizeof(reply));
		sendto(auth_fd, &request, sizeof(request), 0, (struct sockaddr *)&auth_addr, addrlen);
		recvfrom(auth_fd, &reply, sizeof(reply), 0, (struct sockaddr *)&auth_addr, &addrlen);

		if (strcmp(reply, "NULL") == 0){
			printf("\nAuthoritative server did not respond with an IP address.");
			return NULL;
		}

		else{
			printf("\nAuthoritative server replied with IP address: %s", reply);
			addRecord(local, req_server, reply);
			printf("\nAdded record for Server: %s with IP address: %s in local table.", req_server, reply);
			address = reply;
		}
	}

	return address;
}