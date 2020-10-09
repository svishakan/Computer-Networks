#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define CLI_PORT	7100
#define LOCAL_PORT	7200
#define ROOT_PORT	7300
#define TLD_PORT	7400
#define AUTH_PORT   	7500

#define SIZE 30

struct DNS_Table{					//struct for the DNS table
	char	name[100];
	char	server_list[SIZE][100];
	char	ip_list[SIZE][50];
	int		cur_size;
};

typedef struct DNS_Table dns_table;

char empty[5] = "NULL\0";

int        setUpConnection(struct sockaddr_in *conn, int port, int is_bound, char *conn_name);
int        checkIP(char *ip);
int        addRecord(dns_table *table, char *server, char *ip);
void       printTable(dns_table *table);
void       updateTable(dns_table *table);
dns_table  *initTable(dns_table *table, char *table_name);
char       *fetchAddress(dns_table *table, char *req_server);
char       *toUppercase(char *str);

int setUpConnection(struct sockaddr_in *conn, int port, int is_bound, char *conn_name){
	//sets up the basic socket connection and binds it to a port if specified, and returns socket file descriptor

    int sockfd;

    printf("\nSetting up connection to %s through port %d ...", conn_name, port);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	if(sockfd < 0){
		perror("Error in creating socket.\n");
	}

	bzero(conn, 16);
	conn->sin_family = AF_INET;
	conn->sin_addr.s_addr = INADDR_ANY;
	conn->sin_port = htons(port);

	if(is_bound){
		if(bind(sockfd, (struct sockaddr *)conn, 16) < 0){
			perror("Error in binding.\n");
		}

    	else{
	    	printf("\nConnection to %s successfully established.\n", conn_name);
    	}
	}

	return sockfd;
}

int checkIP(char *ip){
	//Checks for the validity of a given IP address

	int valid = 1, byte;
	char *ip_copy, *split;

	ip_copy = (char *)calloc(50, sizeof(char));
	strcpy(ip_copy, ip);
	split = strtok(ip_copy, ".");

	while(split){	//split pointer points to each "byte" iteratively
		byte = atoi(split);
		if(byte < 0 || byte > 255){
			return 0;
		}

		split = strtok(NULL, ".");
	}

	return 1;
}

int addRecord(dns_table *table, char *server, char *ip){
	//Add a new record into a specific DNS table

	int valid;

	if(table->cur_size == SIZE - 1){	//if table is full, replace first record.
		strcpy(table->server_list[0], server);
		strcpy(table->ip_list[0], ip);

		return table->cur_size;
	}

	valid = checkIP(ip);

	if(valid){
		strcpy(table->server_list[table->cur_size], server);
		strcpy(table->ip_list[table->cur_size], ip);
		table->cur_size++;
	}

	else{
		printf("\tIP Address %s is invalid.\n", ip);

	}

	return table->cur_size;
}

void printTable(dns_table *table){
	//Print the current contents of a given table

	int i = 0;

	printf("\n\t---------------------------------------------------------");
	printf("\n\t\t\t%-30s", table->name);
	printf("\n\t---------------------------------------------------------");
	printf("\n\t%-25s\t%s\n", "Server Name", "IP Address");

	for(i = 0; i < table->cur_size; i++){
		printf("\n\t%-25s\t%s", table->server_list[i], table->ip_list[i]);
	}
	printf("\n\t---------------------------------------------------------\n\n");
}

void updateTable(dns_table *table){
	//Update a given DNS table

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

		for(i = 0; i < table->cur_size; i++){
			if(strcmp(ip, table->ip_list[i]) == 0){
				exists = 1;
				printf("\nIP Address %s is already allocated.\n", ip);
				break;
			}
		}

		if(exists == 0){
			strcpy(table->ip_list[i], ip);
			strcpy(table->server_list[i], serv);
			table->cur_size++;

			printf("\nDo you wish to continue modifying the table? (1 - YES, 0 - NO) -> ");
			scanf("%d", &choice);
		}
	}
}

dns_table *initTable(dns_table *table, char *table_name){
	//Initialize the local, root and auth tables with some prefixed records

	table = (dns_table *)malloc(sizeof(dns_table));
	table->cur_size = 0;
	strcpy(table->name, table_name);

	addRecord(table, "www.google.com", "142.89.78.66");
	addRecord(table, "www.yahoo.com", "10.2.45.67");
	addRecord(table, "www.annauniv.edu", "197.34.53.122");

	return table;
}

char *fetchAddress(dns_table *table, char *req_server){
	//Fetch the address of a given domain name from a given DNS table

	int i = 0;

	for(i = 0; i < table->cur_size; i++){
		if(strcmp(table->server_list[i], req_server) == 0){
			return table->ip_list[i];	//found
		}
	}

	return NULL;	//not found
}

char *toUppercase(char *str){
	//converts a given string to uppercase
	char *upper;
	int i = 0;

	upper = (char *)malloc(sizeof(str));

	for (i = 0; str[i] != '\0'; i++) {
      if(str[i] >= 'a' && str[i] <= 'z') {
         upper[i] = str[i] - 32;
      }
      else{
      	upper[i] = str[i];
      }
   }

	upper[i] = '\0';

	return upper;
}
