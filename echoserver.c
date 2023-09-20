#include <stdio.h>
#include <stdlib.>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "DieWithMessage.h"

static const int MAXPENDING = 5; 

int main(int argc, char *argv[]) {

	if (argc != 2)
		DieWithUserMessage("Parameter(s)", "<Server Port>");

	in_port_t servPort = atoi(argv[1]); // local port

	// Create socket
	int servSock;
	if ((servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		DieWithSystemMessage("socket() failed");

	// Construct local address structure
	struct sockaddr_in servAddr;
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY) // any incoming interface
	servAddr.sin_port = htons(servPort);

	// Bind to the local address
	if (bind(servSock, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0)
		DieWithSystemMessage("bind() failed");

	// Make socket to listem for incoming connections 
	if (listen(servSock, MAXPENDING) < 0)
		DieWithSystemMessage("listen() failed");

	for (;;) {
		struct sockaddr_in clntAddr;
		// set length of client address structure
		socklen_t clntAddrLen = sizeof(clntAddr);

		// Wait for a client to connect
		int clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);
		if (clntSock < 0)
			DieWithSystemMessage("accept() failed");

		// clntSock is connected to a client

		char clntName[INET_ADDRSTRLEN]; // string to contain client address
		if (inet_ntop(AF_INET, &clntAddr.sin_addr.s_addr, clntName,
					sizeof(clntName)) != NULL)
			printf("Handling client %s/%d\n", clntName, ntohs(clntAddr.sin_port));
		else
			puts("Unable to get client address");

		HandleTCPClient(clntSock);
	}

}
