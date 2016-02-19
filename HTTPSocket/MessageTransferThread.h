#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/queue.h>
#include <pthread.h>
#include <unistd.h>

#define MYPORT 8677 
#define BACKLOG 10
#define bool int
#define true 1
#define BUFFERSIZE 1024

typedef struct requestMessage
{
	char requestMethod[8];
	char requestURI[100];
	char environmentVariable[10][1024];
	char* requestBody;
	int bodySize;
} RequestMessage;

int ReceiveRequestMessage(int clientSocket, char* receiveRequestMessageBuffer);
void ParsingRequestMessage(int clientSocket, char* requestMessageBuffer,
	RequestMessage* requestMessage);
int SelectSendState(RequestMessage* requestMessage);
void SendResponseMessage(int clientSocket, int sendMessageState,
	RequestMessage* requestMessage);
