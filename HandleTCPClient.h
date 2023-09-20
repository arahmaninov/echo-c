#include <stdio.h>
#include <stdlib.h>

void HandleTCPClient(int clntSocket) {
	char buffer[BUFSIZE];

	// Receive message from client
	ssize_t numBytesRcvd = recv(clntSocket, buffer, BUFFSIZE, 0);
	if (numBytesRcvd < 0)
		DieWithSystemMessage("recv() failed");

	// Send received string and receive again until EOF
	while (numBytesRcvd > 0) {
		// Echo message back to client
		ssize_t numBytesSent = send(clntSocket, buffer,
				numBytesRcvd, 0);
		if (numBytesSent < 0)
			DieWithSystemMessage("send() failed");
		else if (numBytesSent != numBytesRcvd)
			DieWithUserMessage("send()", "sent unexpected number of bytes");

		// See if tgere is more data to receive
		numBytesRcvd = recv(clntSocket, buffer, BUFSIZE, 0);
		if (numBytesRcvd < 0)
			DieWithSystemMessage("recv() failed");
	}

	close(clntSocket);
}
