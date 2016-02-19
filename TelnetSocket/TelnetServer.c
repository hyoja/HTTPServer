#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/param.h>
#include <pthread.h>

#define MYPORT 8877 
#define BACKLOG 10
#define true 1
#define false 0
#define PTHREADSIZE 10
#define BUFFERSIZE 1024 //why i need 1024? im really want to use 1024? or just sensitive case? i thought that it just memory ^2 times value, becouse fragmentation, paging always splited 2times value, for exaple 2 4 8 16 32....

int InitializeServer();
void RunAcceptState(int serverSocket);
void *RunThreadFunction(void *clientSocketMemoryAddress);

int main(void)
{
	int serverSocket = InitializeServer();//Establish socket, bind, listen.
	if(serverSocket == -1)
	{
		printf("initialization was fail, so i'm out.\n");
		return 0;
	}
	RunAcceptState(serverSocket);
	return 0;
}

void RunAcceptState(int serverSocket)
{
	int clientSocket = 0;
	int clientAddressSize = 0;
	struct sockaddr_in clientAddress;
	pthread_t threadPool = NULL;

	while(true)
	{
		clientAddressSize = sizeof(clientAddress);
		clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressSize);
		pthread_create(&(threadPool),NULL,RunThreadFunction,(void*)&clientSocket);
		printf("i set threadNumber %d \n",clientSocket);
	}
	close(serverSocket);
}

void *RunThreadFunction(void *serverSocketMemoryAddress)
{
	int recvValue = 0;
	char packetBuffer[BUFFERSIZE];
	memset(packetBuffer, '\0', BUFFERSIZE);
	int serverSocket = *((int *)serverSocketMemoryAddress);

	while((recvValue = recv( serverSocket, packetBuffer, BUFFERSIZE, 0)))
	{
		printf("socket number is %d ,message is %s", serverSocket,packetBuffer);
		send(serverSocket, packetBuffer, BUFFERSIZE, 0);
		memset(packetBuffer,'\0',BUFFERSIZE);
	}
	close(serverSocket);
}

int InitializeServer()
{
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0); //AF_INET is supported like address families;
	int waitingBuffer[5]; // array store to binding wating
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &waitingBuffer, sizeof(waitingBuffer)*5);
	if(serverSocket > -1)
	{
		printf("i opened server socket %d \n",serverSocket);
	}
	else
	{
		printf("fail server socket %d \n",serverSocket);
		return -1;
	}
		
	struct sockaddr_in serverSocketAddress;
	serverSocketAddress.sin_family = AF_INET;
	serverSocketAddress.sin_port = htons(MYPORT);
	serverSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	bzero(&(serverSocketAddress.sin_zero),8);

	//1 parameter si socket discripter, 2 parameter is sockerAddIn structor pointer, 3 is socker length
	int bindState = bind(serverSocket,(struct sockaddr *)&serverSocketAddress,sizeof(struct sockaddr));
	if(bindState > -1)
	{
		printf("i'm binded %d \n",bindState);
	}
	else
	{
		printf("fail binding %d \n",bindState);
		return -1;
	}

	int listenState = listen(serverSocket, 5);
	if(listenState > -1)
	{			
		printf("i'm listened %d \n", listenState);
	}
	else
	{
		printf("fail listend %d \n",listenState);
		return -1;
	}
	return serverSocket;
}
