#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "DieWithMessage.h"

int main(int argc, char *argv[])
{
	if (argc < 3 || argc > 4)  
		DieWithUserMessage("Parameter(s)",
		"<Server Address> <Echo Word> [<Server Port>]");

	char *serverIP = argv[1]; // First argument: server IP address
	char *echoString = argv[2]; // Second argument: string to echo
	
	// Third argument (optional): server port (numeric). 7 is well-known echo port
	in_port_t serverPort = (argc == 4) ? atoi(argv[3]) : 7; // in_port_t = 16 bits unsigned int (netinet/in.h) 
	
	// Create a stream socket using TCP
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // args: IPv4, stream protocol, tcp
	if (sock < 0)
		DieWithSystemMessage("socket() failed");

	// Construct the server address structure
	struct sockaddr_in serverAddress; // Server address
	memset(&serverAddress, 0, sizeof(serverAddress)); // Zero out structure
	serverAddress.sin_family = AF_INET; // IPv4 address family
	// Convert address
	int rtnVal = inet_pton(AF_INET, serverIP, &serverAddress.sin_addr.s_addr);
	if (rtnVal == 0)
		DieWithUserMessage("inet_pton() failed", "invalid address string");
	else if (rtnVal < 0)
		DieWithSystemMessage("inet_pton() failed");
	serverAddress.sin_port = htons(serverPort); // Server port

	// Establish the connectiion to the echo server
	if (connect(sock, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
		DieWithSystemMessage("connect() failed");

	size_t echoStringLen = strlen(echoString); // Determine input length

	// Send the string to the server
	ssize_t numBytes = send(sock, echoString, echoStringLen, 0);
	if (numBytes < 0)
		DieWithSystemMessage("send() failed");
	else if (numBytes != echoStringLen)
			DieWithUserMessage("send()", "sent unexpected number of bytes");

	// Receive the same string back from the server
	unsigned int totalBytesRcvd = 0; // Count of total bytes received
	fputs("Received: ", stdout); // Print echoed string
	while (totalBytesRcvd < echoStringLen){
		char buffer[BUFSIZ]; // I/O buffer
		// Receive up to the buffer size (minus 1 to leave space for a null
		// terminator) bytes from the sender
		numBytes = recv(sock, buffer, BUFSIZ - 1, 0);
		if (numBytes < 0)
			DieWithSystemMessage("recv() failed");
		else if (numBytes == 0)
			DieWithUserMessage("recv()", "connection closed prematurely");
		totalBytesRcvd += numBytes;
		buffer[numBytes] = '\0'; // Terminate the string
		fputs(buffer, stdout); // Print the echo buffer
	}

	fputc('\n', stdout); 

	close(sock);
	exit(0);
}
