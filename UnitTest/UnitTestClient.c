#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/param.h>

#define MYPORT 8777 
#define BACKLOG 10
#define true 1
#define BUFFERSIZE 1024

int InitializeClient();
char* SendMessage(int clientSocket,char* requestMessage);
int TestUnit()
{
////// variable initialize
	FILE* filePointer = fopen("testCase.txt","r");
	char testCaseBuffer[BUFFERSIZE];
	memset(testCaseBuffer,'\0',BUFFERSIZE);

	int lineCount = 0 ;
	while(fgets(testCaseBuffer,BUFFERSIZE,filePointer) != NULL)
	{
		memset(testCaseBuffer,'\0',BUFFERSIZE);
		lineCount++;
	}
	int i = 0 ,j = 0, k = 0 ;
	
	char*** testCase = (char***)malloc(sizeof(char**) * lineCount);
	for(i = 0; i < lineCount; i++)
	{
		testCase[i] = (char**)malloc(2 * sizeof(char*));
		for(j = 0 ; j < 2 ; j++)
		{
			testCase[i][j] = (char*)malloc(BUFFERSIZE * sizeof(char));
		}			
	}
	for (i = 0; i < lineCount; i++)
        for (j = 0; j <2; j++)
            for (k = 0; k < 1024; k++)
                testCase[i][j][k] = '\0';

////// read testCase.txt
	close(filePointer);
	filePointer = fopen("testCase.txt","r");
	for(i = 0 ; i < lineCount;i++)
	{
		char* tempBuffer;
		memset(testCaseBuffer,'\0',BUFFERSIZE);
		fgets(testCaseBuffer, BUFFERSIZE, filePointer);
		
		tempBuffer = strtok(testCaseBuffer,"\t");
		strcpy(testCase[i][0],tempBuffer);
		
		tempBuffer = strtok(NULL,"\t");
		strcpy(testCase[i][1],tempBuffer);
	}

/////// Program Test
	for(i = 0; i < lineCount; i++)
	{
		char sendMessageBuffer[BUFFERSIZE];
		memset(sendMessageBuffer, '\0', BUFFERSIZE);
		//fgets(sendMessageBuffer, BUFFERSIZE, stdin);
		printf("send : %s , receive : %s \n",testCase[i][0],testCase[i][1]);
		int clientSocket = InitializeClient(MYPORT);//Establish socket and connect.		
		char receivedMessageBuffer[BUFFERSIZE];
		strcpy(receivedMessageBuffer, SendMessage(clientSocket, testCase[i][0]));
		
		int j = 0 ;
		for(j = 0 ; j < strlen(testCase[i][1]); j++)
		{
			if(receivedMessageBuffer[j] == '\0' || 
			   testCase[i][1][j] != receivedMessageBuffer[j])
			{
				printf("Fail : receive : %s \n",receivedMessageBuffer);
				break;
			}
		}
		if(j == strlen(testCase[i][1]))
		{
			printf("Succ \n");
		}
	}
}

int main(int argc, char* argv[])
{
	int i = 0;
	while(i < 5000000000)
	{
		TestUnit();
		i++;
	}
	return 0;
}

char* SendMessage(int clientSocket, char* requestMessage)
{
	//printf("send Message : %s \n", requestMessage);
	send(clientSocket, requestMessage, BUFFERSIZE, 0);

	char receiveBuffer[BUFFERSIZE];
	memset(receiveBuffer, '\0', BUFFERSIZE);
	
	recv(clientSocket, receiveBuffer, BUFFERSIZE, 0);
	//printf("receive Message : %s ", receiveBuffer);
	close(clientSocket);
	//printf("\n\nend end end\n\n");
	return receiveBuffer;
}

int InitializeClient(int port)
{
	int clientSocket = socket(AF_INET, SOCK_STREAM, 0); //AF_INET is supported like address families;
	struct sockaddr_in clientSocketAddress;
	clientSocketAddress.sin_family = AF_INET;
	clientSocketAddress.sin_port = htons(port);
	clientSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	bzero(&(clientSocketAddress.sin_zero), 8);

	if (clientSocket > -1)
	{
		//printf("i open socket\n", clientSocket);
	}
	else
	{
		printf("fail to open socket\n");
		return -1;
	}

	int connectState = connect(clientSocket, (struct sockaddr *)&clientSocketAddress, sizeof(struct sockaddr_in));

	if (connectState > -1)
	{
		//printf("i'm connected \n", connectState);
	}
	else
	{
		printf("fail connection\n");
		return -1;
	}
	return clientSocket;
}
