#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/param.h>

#define MYPORT 8877 
#define BACKLOG 10
#define true 1
#define BUFFSIZE 1024

int InitializeClient();
int EchoMessage(int clientSocket);

int main(int argc,char* argv[])
{
	while(true)
	{
		int clientSocket = InitializeClient(atoi(argv[1]));//Establish socket and connect.
		EchoMessage(clientSocket);//send, and receive Message.
		printf("\n\nend end end\n\n");
	}
	return 0;
}

int EchoMessage(int clientSocket)
{
	char inputStringBuffer[BUFFSIZE];	
	memset(inputStringBuffer, '\0', BUFFSIZE);
	fgets(inputStringBuffer, BUFFSIZE, stdin);
	printf("!!");
	printf("send Message : %s", inputStringBuffer);
	send(clientSocket, inputStringBuffer, BUFFSIZE, 0);

	memset(inputStringBuffer, '\0', BUFFSIZE);

	recv(clientSocket, inputStringBuffer, BUFFSIZE, 0);
	printf("receive Message : %s",inputStringBuffer);
	close(clientSocket);
}
 
int InitializeClient(int port)
{
	int clientSocket = socket(AF_INET, SOCK_STREAM, 0); //AF_INET is supported like address families;
	struct sockaddr_in clientSocketAddress;
	clientSocketAddress.sin_family = AF_INET;
	clientSocketAddress.sin_port = htons(port);
	clientSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	bzero(&(clientSocketAddress.sin_zero),8);
	
	if(clientSocket > -1)
	{
		printf("i open socket\n", clientSocket);
	}
	else
	{
		printf("fail to open socket\n");
		return -1;
	}
	
	int connectState = connect(clientSocket, (struct sockaddr *)&clientSocketAddress, sizeof(struct sockaddr_in));

	if(connectState > -1)
	{			
		printf("i'm connected \n", connectState);
	}
	else
	{
		printf("fail connection\n");
		return -1;
	}
	return clientSocket;
}
