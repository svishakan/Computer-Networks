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

int main(int argc, char *argv[])
{
	// For Socket Programming
	struct sockaddr_in serverAddr, clientAddr;
	int len = sizeof(clientAddr), socketfd;
	char buffer[1024];

	// For Requesting
	char name[100], IP[17], choice[5];

	// --- Creating and configuring a socket --- //

	socketfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(socketfd < 0)
		perror("Failed to create Socket");

	bzero(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(1270);

	// Displaying the menu

	do
	{
		// Getting client's option
		printf("\nDomain     : ");
		scanf("%s", name);

		// Copying to buffer
		strcpy(buffer, name);

		// Sending to server
		sendto(socketfd, buffer, sizeof(buffer), MSG_CONFIRM, (struct sockaddr*)&serverAddr, len);

		// Read response from server
		recvfrom(socketfd, buffer, sizeof(buffer), MSG_WAITALL, (struct sockaddr*)&serverAddr, &len);

		if(strcmp(buffer, "404") == 0)
			printf("Error 404  : Not Found");
		else
			printf("IP Address : %s", buffer);

		printf("\n\nDo you want to continue? (Yes/No): ");
		scanf("%s", choice);
	} while(strcmp(choice, "Yes") == 0 || strcmp(choice, "yes") == 0);

	close(socketfd);
}

/*

Output:

---------
Client 1:
---------

Domain     : www.yahoo.com
IP Address : 10.2.45.67

Do you want to continue? (Yes/No): Yes

Domain     : www.coursera.com
IP Address : 23.71.199.20

Do you want to continue? (Yes/No): Yes

Domain     : www.quora.com
IP Address : 102.32.42.32

Do you want to continue? (Yes/No): yes

Domain     : www.bing.com
IP Address : 23.11.245.67

Do you want to continue? (Yes/No): No

-------------------------------------------------------------------------------------------------------------

---------
Client 2:
---------

Domain     : www.coursera.com
IP Address : 23.71.199.20

Do you want to continue? (Yes/No): Yes

Domain     : www.github.com
IP Address : 32.182.42.39

Do you want to continue? (Yes/No): Yes

Domain     : www.annauniv.edu
IP Address : 197.34.53.122

Do you want to continue? (Yes/No): Yes

Domain     : www.medium.com
IP Address : 17.224.153.192

Do you want to continue? (Yes/No): No

*/
