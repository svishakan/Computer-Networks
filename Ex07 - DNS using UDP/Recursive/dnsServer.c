/*					Code Owner: Shashanka Venkatesh
Thanks to the great lord for contributing to my repository with this code*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define MAX_SIZE 20
#define BUFF_SIZE 1024
#define LOCAL 1
#define ROOT 2
#define TOP 3
#define AUTH 4

typedef struct
{
	char name[100], IP[17];
} domain;

// Creates and returns a Domain object with the given name and IP
domain createDomain(char *name, char *IP);
// Checks if the IP is valid
int checkValidIP(char *IP);
// Add a domain to the table
int addDomain(domain dnsTable[], int *size, char *name, char *IP);
// Modify Domain with new IP
int modifyDomain(domain dnsTable[], int size, char *name, char *newIP);
// Returns the IP address of the server domain name passed
char * getIP(domain dnsTable[], int size, char *name);
// Initializes the DNS Table with some entries depending on which level it is
void initDNStable(domain dnsTable[], int level, int *size);
// Displays the table in a neat format
void display(domain dnsTable[], int size);

int main(int argc, char *argv[])
{
	// For Socket Programming
	struct sockaddr_in serverAddr, clientAddr, higherServerAddr;
	int len = sizeof(clientAddr), socketfdLow, socketfdHigh;
	char buffer[BUFF_SIZE];

	// For DNS operations
	domain dnsTable[MAX_SIZE];
	int tableSize = 0, level, op, ret;
	char *name = (char *)malloc(100), *IP = (char *)malloc(17), choice[5];

	printf("Level of the Server:\n1. Local Level\n2. Root Level\n3. Top level\n4. Authoritative Level\n>>");
	scanf("%d", &level);

	while (level < 1 || level > 4)
	{
		printf("\nEnter a Valid Level Number!\n");
		printf("\nLevel of the Server:\n1. Local Level\n2. Root Level\n3. Top level\n4. Authoritative Level\n>>");
		scanf("%d", &level);
	}

	// --- Creating, configuring and binding a socket to a lower level --- //

	socketfdLow = socket(AF_INET, SOCK_DGRAM, 0);
	if(socketfdLow < 0)
		perror("Failed to create Socket");

	bzero(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(1270 + level - 1);

	if(bind(socketfdLow, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
		perror("Bind Error");

	// --- Creating and configuring a socket to a higher level server --- //

	// Authoritative Level Server won't have a higher level server to it
	if(level < 4)
	{
		socketfdHigh = socket(AF_INET, SOCK_DGRAM, 0);
		if(socketfdHigh < 0)
			perror("Failed to create Socket");

		bzero(&higherServerAddr, sizeof(higherServerAddr));
		higherServerAddr.sin_family = AF_INET;
		higherServerAddr.sin_addr.s_addr = INADDR_ANY;
		higherServerAddr.sin_port = htons(1270 + level);
	}

	// Initialize the table with some entries and display it
	// Level 1: No Entries
	// Level 2: 2 Entries
	// Level 3: 3 Entries
	// Level 4: 5 Entries
	initDNStable(dnsTable, level, &tableSize);
	display(dnsTable, tableSize);

	// --- Performing Manual modifications to the Server (if needed) --- //

	while(1)
	{
		printf("\nDo you want to Add or Modify Domains? (Yes/No): ");
		scanf("%s", choice);

		if(strcmp(choice, "Yes") == 0 || strcmp(choice, "yes") == 0)
		{
			printf("1. Add\n2. Modify\n>>");
			scanf("%d", &op);

			printf("\nDomain Name: ");
			scanf("%s", name);
			printf("Domain IP  : ");
			scanf("%s", IP);

			ret = 1;

			while(op == 1 && ret != 0)
			{
				ret = addDomain(dnsTable, &tableSize, name, IP);
				switch(ret)
				{
					case 0:	printf("\nDomain Successfully Added\nUpdated Table:\n");
							display(dnsTable, tableSize);
							break;
					case 1:	printf("\nIP Address is not Valid!\n");
							printf("Domain IP  : ");
							scanf("%s", IP);
							break;
					case 2:	printf("\nTable is Full!\n"); break;
					case 3:	printf("\nIP Address already exists!\n");
							printf("\nNew IP  : ");
							scanf("%s", IP);
				}
			}

			while(op == 2 && ret != 0)
			{
				ret = modifyDomain(dnsTable, tableSize, name, IP);
				switch(ret)
				{
					case 0:	printf("\nDomain IP Successfully Modified\nUpdated Table:\n");
							display(dnsTable, tableSize);
							break;
					case 1:	printf("\nIP Address is not Valid!\n");
							printf("\nValid New IP  : ");
							scanf("%s", IP);
							break;
					case 2:	printf("\nDomain Not Found!\n");
							printf("\nDomain Name: ");
							scanf("%s", name);
							break;
					case 3:	printf("\nIP Address already exists!\n");
							printf("\nNew IP  : ");
							scanf("%s", IP);
				}
			}
		}

		else
			break;
	}

	// --- Accepting requests from clients --- //

	if(level == LOCAL)
		printf("\n----- Accepting Requests from Clients -----\n");
	else
		printf("\n----- Accepting Requests from Lower Levels -----\n");

	while(1)
	{
		// Recieve a request
		recvfrom(socketfdLow, buffer, sizeof(buffer), MSG_WAITALL, (struct sockaddr*)&clientAddr, &len);

		// If the client has no more requests
		if(strcmp(buffer, "No") == 0)
			continue;

		if(level == LOCAL)
			printf("\nRequest From Client     : %s\n", buffer);
		else
			printf("\nRequest From Lower Level Server     : %s\n", buffer);

		IP = getIP(dnsTable, tableSize, buffer);

		// If not found at current level (and this isn't Authoritative Level), ask the higher level
		if(strcmp(IP, "404") == 0 && level < AUTH)
		{
			printf("\nNot found at current level\nRequesting from higher level server...\n");
			strcpy(name, buffer);
			// Requesting a higher level server
			sendto(socketfdHigh, buffer, sizeof(buffer), MSG_CONFIRM, (struct sockaddr*)&higherServerAddr, len);

			// Getting response from higher level server
			recvfrom(socketfdHigh, buffer, sizeof(buffer), MSG_WAITALL, (struct sockaddr*)&higherServerAddr, &len);

			// If the higher level server had the domain, add it to the DNS table of current level
			if(strcmp(buffer, "404") != 0)
			{
				printf("\nFound at higher level!\n");
				addDomain(dnsTable, &tableSize, name, buffer);
			}
			else
				printf("\nNot found at higher level\n");
		}
		else
		{
			printf("\nFound at current level!\n");
			strcpy(buffer, IP);
		}
		// Sending the generated information
		sendto(socketfdLow, buffer, sizeof(buffer), MSG_CONFIRM, (struct sockaddr*)&clientAddr, len);

		if(level == 1)
			printf("Response Sent to Client : %s\n", buffer);
		else
			printf("Response Sent to Lower Level Server : %s\n", buffer);
	}
}

// Creates and returns a Domain object with the given name and IP
domain createDomain(char *name, char *IP)
{
	domain newDomain;

	strcpy(newDomain.name, name);
	strcpy(newDomain.IP, IP);

	return (newDomain);
}

// Checks if the IP is valid
int checkValidIP(char *IP)
{
	int n, i, j;
	char num[20];

	for(i = 0, j = 0; IP[i] != '\0'; i++)
	{
		if(IP[i] == '.' || IP[i] == '\0')
		{
			num[j] = '\0';
			n = atoi(num);
			if(n < 0 || n > 255)
				return (1);
			j = 0;
		}
		else
			num[j++] = IP[i];
	}

	num[j] = '\0';
	n = atoi(num);
	if(n < 0 || n > 255)
		return (1);

	return (0);
}

// Add a domain to the table
int addDomain(domain dnsTable[], int *size, char *name, char *IP)
{
	if(checkValidIP(IP))
		return(1);

	if(*size == MAX_SIZE)
		return(2);

	for(int i = 0; i < *size; i++)
		if(strcmp(dnsTable[i].IP, IP) == 0)
			return(3);

	domain newDomain = createDomain(name, IP);
	dnsTable[(*size)++] = newDomain;

	return(0);
}

// Modify Domain with new IP
int modifyDomain(domain dnsTable[], int size, char *name, char *newIP)
{
	int i, j;

	if(checkValidIP(newIP))
		return(1);

	for(i = 0; i < size; i++)
		if(strcmp(dnsTable[i].name, name) == 0)
			break;

	if(i == size)
		return(2);

	for(j = 0; j < size; j++)
		if(j != i && strcmp(dnsTable[j].IP, newIP) == 0)
			return(3);

	strcpy(dnsTable[i].IP, newIP);
	return(0);
}

// Returns the IP address of the server domain name passed
char * getIP(domain dnsTable[], int size, char *domainName)
{
	int i, notFound = 1;
	char *IP = (char *)malloc(17);

	for(i = 0; i < size; i++)
	{
		if(strcmp(dnsTable[i].name, domainName) == 0)
		{
			notFound = 0;
			break;
		}
	}

	if(notFound)
		return("404");

	strcpy(IP, dnsTable[i].IP);
	return (IP);
}

// Initializes the DNS Table with based on the level of the server
void initDNStable(domain dnsTable[], int level, int *size)
{
	switch(level)
	{
		case LOCAL:	break;
		case ROOT:	addDomain(dnsTable, size, "www.yahoo.com", "10.2.45.67");
					addDomain(dnsTable, size, "www.annauniv.edu", "197.34.53.122");
					break;
		case TOP:	addDomain(dnsTable, size, "www.google.com", "172.217.166.110");
					addDomain(dnsTable, size, "www.quora.com", "102.32.42.32");
					addDomain(dnsTable, size, "www.linkedin.com", "12.312.4.31");
					break;
		case AUTH:	addDomain(dnsTable, size, "www.bing.com", "23.11.245.67");
					addDomain(dnsTable, size, "www.medium.com", "17.224.153.192");
					addDomain(dnsTable, size, "www.udemy.com", "82.27.16.10");
					addDomain(dnsTable, size, "www.coursera.com", "12.212.172.232");
					addDomain(dnsTable, size, "www.github.com", "32.182.42.39");
	}
}

// Displays the table in a neat format
void display(domain dnsTable[], int size)
{
	int i;

	printf("\n----------------------------------------");
	printf("\nServer Name\t\tIP Address");
	printf("\n----------------------------------------\n");
	for(int i = 0; i < size; i++)
	{
		printf("%s\t", dnsTable[i].name);
		if(strlen(dnsTable[i].name) < 16)
			printf("\t");
		printf("%s\n", dnsTable[i].IP);
	}
	printf("----------------------------------------\n");
}

/*

Output:

---------------------
Authoritative Server:
---------------------

Level of the Server:
1. Local Level
2. Root Level
3. Top level
4. Authoritative Level
>>4

----------------------------------------
Server Name             IP Address
----------------------------------------
www.bing.com            23.11.245.67
www.medium.com          17.224.153.192
www.udemy.com           82.27.16.10
www.coursera.com        12.212.172.232
www.github.com          32.182.42.39
----------------------------------------
>>4

----------------------------------------
Server Name             IP Address
----------------------------------------
www.bing.com            23.11.245.67
www.medium.com          17.224.153.192
www.udemy.com           82.27.16.10
www.coursera.com        12.212.172.232
www.github.com          32.182.42.39
----------------------------------------

Do you want to Add or Modify Domains? (Yes/No): Yes
1. Add
2. Modify
>>1

Domain Name: www.codechef.com
Domain IP  : 233.344.21.23

IP Address is not Valid!
Domain IP  : 23.122.3.234

Domain Successfully Added
Updated Table:

----------------------------------------
Server Name             IP Address
----------------------------------------
www.bing.com            23.11.245.67
www.medium.com          17.224.153.192
www.udemy.com           82.27.16.10
www.coursera.com        12.212.172.232
www.github.com          32.182.42.39
www.codechef.com        23.122.3.234
----------------------------------------

Do you want to Add or Modify Domains? (Yes/No): Yes
1. Add
2. Modify
>>2

Domain Name: www.coursera.com
Domain IP  : 17.224.153.192

IP Address already exists!

New IP  : 28.331.23.98

IP Address is not Valid!

Valid New IP  : 23.71.199.20

Domain IP Successfully Modified
Updated Table:

----------------------------------------
Server Name             IP Address
----------------------------------------
www.bing.com            23.11.245.67
www.medium.com          17.224.153.192
www.udemy.com           82.27.16.10
www.coursera.com        23.71.199.20
www.github.com          32.182.42.39
www.codechef.com        23.122.3.234
----------------------------------------

Do you want to Add or Modify Domains? (Yes/No): No

----- Accepting Requests from Lower Levels -----

Request From Lower Level Server     : www.coursera.com

Found at current level!
Response Sent to Lower Level Server : 23.71.199.20

Request From Lower Level Server     : www.github.com

Found at current level!
Response Sent to Lower Level Server : 32.182.42.39

Request From Lower Level Server     : www.medium.com

Found at current level!
Response Sent to Lower Level Server : 17.224.153.192

Request From Lower Level Server     : www.bing.com

Found at current level!
Response Sent to Lower Level Server : 23.11.245.67

-------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------

-----------------
Top Level Server:
-----------------

Level of the Server:
1. Local Level
2. Root Level
3. Top level
4. Authoritative Level
>>3

----------------------------------------
Server Name             IP Address
----------------------------------------
www.google.com          172.217.166.110
www.quora.com           102.32.42.32
----------------------------------------

Do you want to Add or Modify Domains? (Yes/No): No

----- Accepting Requests from Lower Levels -----

Request From Lower Level Server     : www.coursera.com

Not found at current level
Requesting from higher level server...

Found at higher level!
Response Sent to Lower Level Server : 23.71.199.20

Request From Lower Level Server     : www.quora.com

Found at current level!
Response Sent to Lower Level Server : 102.32.42.32

Request From Lower Level Server     : www.github.com

Not found at current level
Requesting from higher level server...

Found at higher level!
Response Sent to Lower Level Server : 32.182.42.39

Request From Lower Level Server     : www.medium.com

Not found at current level
Requesting from higher level server...

Found at higher level!
Response Sent to Lower Level Server : 17.224.153.192

Request From Lower Level Server     : www.bing.com

Not found at current level
Requesting from higher level server...

Found at higher level!
Response Sent to Lower Level Server : 23.11.245.67

-------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------

------------
Root Server:
------------

Level of the Server:
1. Local Level
2. Root Level
3. Top level
4. Authoritative Level
>>2

----------------------------------------
Server Name             IP Address
----------------------------------------
www.yahoo.com           10.2.45.67
www.annauniv.edu        197.34.53.122
----------------------------------------

Do you want to Add or Modify Domains? (Yes/No): No

----- Accepting Requests from Lower Levels -----

Request From Lower Level Server     : www.yahoo.com

Found at current level!
Response Sent to Lower Level Server : 10.2.45.67

Request From Lower Level Server     : www.coursera.com

Not found at current level
Requesting from higher level server...

Found at higher level!
Response Sent to Lower Level Server : 23.71.199.20

Request From Lower Level Server     : www.quora.com

Not found at current level
Requesting from higher level server...

Found at higher level!
Response Sent to Lower Level Server : 102.32.42.32

Request From Lower Level Server     : www.github.com

Not found at current level
Requesting from higher level server...

Found at higher level!
Response Sent to Lower Level Server : 32.182.42.39

Request From Lower Level Server     : www.annauniv.edu

Found at current level!
Response Sent to Lower Level Server : 197.34.53.122

Request From Lower Level Server     : www.medium.com

Not found at current level
Requesting from higher level server...

Found at higher level!
Response Sent to Lower Level Server : 17.224.153.192

Request From Lower Level Server     : www.bing.com

Not found at current level
Requesting from higher level server...

Found at higher level!
Response Sent to Lower Level Server : 23.11.245.67

-------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------

-------------
Local Server:
-------------

Level of the Server:
1. Local Level
2. Root Level
3. Top level
4. Authoritative Level
>>1

----------------------------------------
Server Name             IP Address
----------------------------------------
----------------------------------------

Do you want to Add or Modify Domains? (Yes/No): No

----- Accepting Requests from Clients -----

Request From Client     : www.yahoo.com

Not found at current level
Requesting from higher level server...

Found at higher level!
Response Sent to Client : 10.2.45.67

Request From Client     : www.coursera.com

Not found at current level
Requesting from higher level server...

Found at higher level!
Response Sent to Client : 23.71.199.20

Request From Client     : www.quora.com

Not found at current level
Requesting from higher level server...

Found at higher level!
Response Sent to Client : 102.32.42.32

Request From Client     : www.coursera.com

Found at current level!
Response Sent to Client : 23.71.199.20

Request From Client     : www.github.com

Not found at current level
Requesting from higher level server...

Found at higher level!
Response Sent to Client : 32.182.42.39

Request From Client     : www.annauniv.edu

Not found at current level
Requesting from higher level server...

Found at higher level!
Response Sent to Client : 197.34.53.122

Request From Client     : www.medium.com

Not found at current level
Requesting from higher level server...

Found at higher level!
Response Sent to Client : 17.224.153.192

Request From Client     : www.bing.com

Not found at current level
Requesting from higher level server...

Found at higher level!
Response Sent to Client : 23.11.245.67

*/
