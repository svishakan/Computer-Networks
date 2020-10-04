#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 7228
#define SIZE 20

int cur_size = 0;
char server_list[SIZE][100], ip_list[SIZE][50];
char empty[4] = "NULL";

int  checkIP(char *ip);
void allocateIP();
void printTable();
void updateTable();
char *fetchIP(char *req_server);

int main(int argc, char **argv){
	struct sockaddr_in server, client;
	int sockfd, n, addrlen, flag;
	char *req_server, *req_ip;
	req_server = (char *)malloc(sizeof(char) * 100);
	req_ip = (char *)malloc(sizeof(char) * 50);

	allocateIP();
	printTable();
	printf("\n\nDo you wish to alter the allocation table?(1 - YES, 0 - NO) -> ");
	scanf("%d", &flag);

	if(flag == 1){
		updateTable();
		printTable();
	}

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	if(sockfd < 0){
		perror("Error in creating socket.\n");
	}

	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	if(bind(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0){
		perror("Error in binding.\n");
	}

	printf("\nServer awaiting clients on port %d...\n", PORT);

	addrlen = sizeof(client);

	while(1){
		recvfrom(sockfd, req_server, sizeof(req_server), 0, (struct sockaddr*)&client, &addrlen);
		printf("Received a request for IP Address of %s from a client.\n", req_server);
		
		req_ip = fetchIP(req_server);
		
		if(req_ip == NULL){
			sendto(sockfd, &empty, sizeof(empty), 0, (struct sockaddr*)&client, addrlen);
		}
		else{
			sendto(sockfd, req_ip, sizeof(req_ip), 0, (struct sockaddr*)&client, addrlen);
		}

		printf("Replied with IP Address %s\n", req_ip);
	}	

	return 0;
}

int checkIP(char *ip){
	int valid = 1, byte;
	char *ip_copy, *split;

	ip_copy = (char *)malloc(sizeof(char) * 50);
	strcpy(ip_copy, ip);
	split = strtok(ip_copy, ".");

	while(split){
		byte = atoi(split);
		if(byte < 0 || byte > 255){
			return 0;
		}

		split = strtok(NULL, ".");
	}

	return 1;
}

void allocateIP(){
	int i = 0, choice = 1, valid;
	char *ip;
	ip = (char *)malloc(sizeof(char) * 50);

	printf("\tPress 1 to Allocate IP Address.\n");
	printf("\tPress 0 to Exit.");

	while(i < SIZE){
		printf("\n\tAllocate? -> ");
		scanf("%d", &choice);
		
		if(choice == 1){
			printf("\n\tEnter Server Name:\t");
			scanf(" %s", server_list[i]);
			printf("\tEnter IP Address:\t");
			scanf(" %s", ip);

			valid = checkIP(ip);

			if(valid){
				strcpy(ip_list[i], ip);
				cur_size++;
				i++;
			}

			else{
				printf("\tIP Address %s is invalid.\n", ip);
			}
			
		}

		else if(choice == 0){
			break;
		}

		else{
			printf("\tInvalid Option.\n");
		}
	}
}

void printTable(){
	int i = 0;

	printf("\n\t---------------------------------------------------------");
	printf("\n\t%-25s\t%s\n", "Server Name", "IP Address");

	for(i = 0; i < cur_size; i++){
		printf("\n\t%-25s\t%s", server_list[i], ip_list[i]);
	}
	printf("\n\t---------------------------------------------------------\n");
}

void updateTable(){
	char serv[100], ip[50];
	int i = 0, exists = 0, choice = 1, valid;
	
	while(choice){
		printf("\nEnter Server Name:\t");
		scanf("%s", serv);
		printf("\nEnter IP Address:\t");
		scanf("%s", ip);

		valid = checkIP(ip);

		if(!valid){
			printf("\nIP Address %s is invalid.\n", ip);
			continue;
		}

		exists = 0;

		for(i = 0; i < cur_size; i++){
			if(strcmp(ip, ip_list[i]) == 0){
				exists = 1;
				printf("\nIP Address %s is already allocated.\n", ip);
				break;
			}
		}

		if(exists == 0){
			strcpy(ip_list[i], ip);
			strcpy(server_list[i], serv);
			cur_size++;

			printf("\nDo you wish to continue modifying the table (1 - YES, 0 - NO) -> ");
			scanf("%d", &choice);
		}
	}
}

char *fetchIP(char *req_server){
	int i = 0;
	for(i = 0; i < cur_size; i++){
		if(strcmp(server_list[i], req_server) == 0){
			return ip_list[i];
		}
	}

	return NULL;
}