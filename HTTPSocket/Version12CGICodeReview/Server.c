#include "Server.h"

int main(void)
{
	int serverSocket = InitializeServer(); //establish socket(), bind(), listen()
	if (serverSocket == -1)
	{
		printf("initialization was fail, so i'm out.\n");
		return 0;
	}
	WaitClient(serverSocket);
	close(serverSocket);
	printf("end");
	return 0;
}

void *RunThreadToCommunicationMessage(void *clientSocketAddress)
{
	int clientSocket = *((int *)clientSocketAddress);
	char receiveRequestMessageBuffer[BUFFERSIZE];
	memset(receiveRequestMessageBuffer, '\0', BUFFERSIZE);
	int receiveValue = ReceiveRequestMessage(clientSocket,
		receiveRequestMessageBuffer);
	RequestMessage requestMessage;

	int i = 0;
	for (i = 0; i < 10; i++)
	{
		memset(requestMessage.environmentVariable[i], '\0', BUFFERSIZE);
	}
	memset(requestMessage.requestURI, '\0', BUFFERSIZE);
	memset(requestMessage.requestMethod, '\0', 8);

	//i is index of message length for bad request.
	int messageLength = ParseRequestMessage(clientSocket,
		receiveRequestMessageBuffer, &requestMessage);
	printf("!! method %s, URI %s\n", requestMessage.requestMethod,
		requestMessage.requestURI);

	int sendMessageState = SelectSendState(&requestMessage);
	////// send packetMessage
	SendResponseMessage(clientSocket, sendMessageState, &requestMessage);

	if (receiveValue < BUFFERSIZE) // it tererble thing . im not want if!
	{
		close(clientSocket);
	}
}

void WaitClient(int serverSocket)
{
	int clientSocket = 0;
	int clientAddressSize = 0;
	struct sockaddr_in clientAddress;
	pthread_t thread = NULL;
	while (true)
	{
		clientAddressSize = sizeof(clientAddress);
		clientSocket = accept(serverSocket, (struct sockaddr*) &clientAddress,
			&clientAddressSize);
		pthread_create(&(thread), NULL, RunThreadToCommunicationMessage,
			(void*)&clientSocket);
		printf("end!!!!!!");
	}
}


int InitializeServer()
{
	int serverSocket = 0;
	serverSocket = socket(AF_INET, SOCK_STREAM, 0); //AF_INET is supported like address families;
	struct sockaddr_in serverSocketAddress;
	serverSocketAddress.sin_family = AF_INET;
	serverSocketAddress.sin_port = htons(MYPORT);
	serverSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	bzero(&(serverSocketAddress.sin_zero), 8);
	int bufferPointer[5]; // array store to binding wating
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &bufferPointer,
		sizeof(bufferPointer));
	//1 parameter si socket discripter, 2 parameter is sockerAddIn structor pointer, 3 is socker length
	int bindState = bind(serverSocket, (struct sockaddr *) &serverSocketAddress,
		sizeof(struct sockaddr));
	int listenState = listen(serverSocket, 5);

	if (serverSocket > -1)
	{
		printf("i opened server socket %d \n", serverSocket);
	}
	else
	{
		printf("fail server socket\n");
		return -1;
	}
	if (bindState > -1)
	{
		printf("i'm binded %d \n", bindState);
	}
	else
	{
		printf("fail binding\n");
		return -1;
	}
	if (listenState > -1)
	{
		printf("i'm listened %d \n", listenState);
	}
	else
	{
		printf("fail listend\n");
		return -1;
	}
	return serverSocket;
}
