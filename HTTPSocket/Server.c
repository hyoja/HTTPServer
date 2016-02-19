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
#define BUFFERSIZE 1024 //why i need 1024? im really want to use 1024? or just sensitive case? i thought that it just memory ^2 times value, becouse fragmentation, paging always splited 2times value, for exaple 2 4 8 16 32....

typedef struct requestMessage
{
	char requestMethod[8];
	char requestURI[100];
	char environmentVariable[10][1024];
	char* requestBody;
	int bodySize;
} RequestMessage;

int InitializeServer();
void WaitClient(int serverSocket);
int ReceiveRequestMessage(int clientSocket, char* receiveRequestMessageBuffer);
void ParsingRequestMessage(int clientSocket, char* requestMessageBuffer,
	RequestMessage* requestMessage);
int SelectSendState(RequestMessage* requestMessage);
void SendResponseMessage(int clientSocket, int sendMessageState,
	RequestMessage* requestMessage);

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

void SendResponseMessage(int clientSocket, int sendPacketState,
	RequestMessage* requestMessage)
{
	char* notFoundText = NULL;
	char readedLine[BUFFERSIZE];
	FILE* readFilePointer = NULL;
	char* cgiFileName;

	char tempBuff[1024];
	char uRIPath[1024];
	char programName[1024];
	char tempnumber[5];
	sprintf(tempnumber, "%d", clientSocket);
	FILE* childOutputPointer = NULL;
	int pipeFileDescriptor[2];
	int pipeINFileDescriptor[2];
	int childStates = -1;
	int i = 0, j = 0;
	int n = 0, receiveBufferSize = 0;

	int leng = atoi(requestMessage->environmentVariable[2]);
	char sendingTemper[1];

	switch (sendPacketState)
	{
	case 200:
		readFilePointer = fopen(requestMessage->requestURI, "r");
		notFoundText = "HTTP/1.1 200 OK\r\n\r\n";
		printf("[server Send :] 200 OK\n");
		send(clientSocket, notFoundText, strlen(notFoundText), 0);
		memset(readedLine, '\0', BUFFERSIZE);
		while (fread(readedLine, 1, BUFFERSIZE, readFilePointer))
		{
			if (send(clientSocket, readedLine, BUFFERSIZE, 0) == -1)
			{
				perror("send");
			}
		}
		break;
	case 2001:
		notFoundText = "HTTP/1.1 200 OK\r\n";
		send(clientSocket, notFoundText, strlen(notFoundText), 0);
		memset(uRIPath, '\0', BUFFERSIZE);
		memset(tempBuff, '\0', BUFFERSIZE);
		memset(programName, '\0', BUFFERSIZE);
		strcpy(uRIPath, "./cgi-bin");
		for (i = strlen(uRIPath); i < BUFFERSIZE; i++, n++)
		{
			if (requestMessage->environmentVariable[1][n] == '\0')
			{
				break;
			}
			uRIPath[i] = requestMessage->environmentVariable[1][n];
		}
		cgiFileName = strtok(requestMessage->environmentVariable[1], "/");

		if (fork() == 0)
		{
			setenv("PATH_INFO", requestMessage->environmentVariable[0], 1); // URI 가 들어있는 폴더 경로 0
			setenv("SCRIPT_NAME", requestMessage->environmentVariable[1], 1); // URI 경로 1
			setenv("CONTENT_LENGTH", requestMessage->environmentVariable[2], 1); // 2 uri 길이
			setenv("REQUEST_METHOD", requestMessage->environmentVariable[3], 1); // 3uri 메서드 
			setenv("CONTENT_TYPE", requestMessage->environmentVariable[4], 1); // request header에서 가저오는 값.
			setenv("QUERY_STRING", requestMessage->environmentVariable[5], 1); // name=value&name1=value
			setenv("REQUEST_URI", uRIPath, 1); // name=value&name1=value

			pipe(pipeFileDescriptor);
			pipe(pipeINFileDescriptor);
			if (fork() == 0)
			{
				close(pipeFileDescriptor[0]);
				dup2(pipeFileDescriptor[1], 1);

				close(pipeINFileDescriptor[1]);
				dup2(pipeINFileDescriptor[0], 0);
				execl(uRIPath, cgiFileName, NULL);
			}
			else
			{
				close(pipeFileDescriptor[1]);
				dup2(pipeFileDescriptor[0], 0);

				close(pipeINFileDescriptor[0]);
				dup2(pipeINFileDescriptor[1], 1);

				if (strstr(requestMessage->environmentVariable[4],
					"multipart/form-data") != NULL)
				{
					printf("%s", requestMessage->requestBody);
				}
				else
				{
					printf("%s", requestMessage->environmentVariable[5]);
				}

				while (0 < read(0, tempBuff, BUFFERSIZE))
				{
					send(clientSocket, tempBuff, BUFFERSIZE, 0);
					memset(tempBuff, '\0', BUFFERSIZE);
				}
			}
		}
		break;
	case 400:
		notFoundText =
			"HTTP/1.1 400 Bad Request\r\n\r\n<html><body>Bad Request</body></html>";
		send(clientSocket, notFoundText, strlen(notFoundText), 0);
		break;
	case 404:
		notFoundText =
			"HTTP/1.1 404 Not Found\r\n\r\n<html><body>Not Found</body></html>";
		send(clientSocket, notFoundText, strlen(notFoundText), 0);
		break;
	default:
		notFoundText =
			"HTTP/1.1 501 Not Implemented\r\n\r\n<html><body>Not Implemented</body></html>";
		send(clientSocket, notFoundText, strlen(notFoundText), 0);
		break;
	}
}

int SelectSendState(RequestMessage* requestMessage)
{
	////// select states
	FILE* readFilePointer = NULL;
	int sendPacketState = -1; // -1 is Not select, 0 is defualt page, 400 is bad request
	char cGIProgram[BUFFERSIZE];
	memset(cGIProgram, '\0', BUFFERSIZE);
	strcpy(cGIProgram, "./cgi-bin/");
	int i = 0;
	int j = 0;
	for (i = 0; i < BUFFERSIZE; i++)
	{
		if (cGIProgram[i] == '\0')
		{
			cGIProgram[i] = requestMessage->requestURI[j];
			if (requestMessage->requestURI[j] == '\0')
			{
				break;
			}
			j++;
		}
	}

	if (((strcmp(requestMessage->requestMethod, "GET") != 0)
		&& (strcmp(requestMessage->requestMethod, "POST") != 0))
		&& (requestMessage->requestURI[0] == '/')
		|| ((atoi(requestMessage->environmentVariable[2]) > 1000)
		&& (strcmp(requestMessage->requestMethod, "POST") != 0)))
	{
		sendPacketState = 404;
	}
	else if ((requestMessage->requestURI[0] == '\0')
		|| (strcmp(requestMessage->requestURI, "main") == 0)
		|| (strcmp(requestMessage->requestURI, "SocketProgram.c") == 0))
	{
		char* tempURL = "HelloWorld.html";
		strcpy(requestMessage->requestURI, tempURL);
		sendPacketState = 200;
	}
	else if (((readFilePointer = fopen(requestMessage->requestURI, "r")) == NULL)
		&& ((readFilePointer = fopen(cGIProgram, "r")) == NULL))
	{
		sendPacketState = 400;
	}
	else if (strcmp(requestMessage->requestMethod, "POST") == 0)
	{
		sendPacketState = 2001; //2001 is for CGI  200-1, 200-2, 200-3
	}
	else if ((readFilePointer = fopen(requestMessage->requestURI, "r")) != NULL)
	{
		sendPacketState = 200;
	}
	else
	{
		sendPacketState = 404; // defualt bad man!
	}
	return sendPacketState;
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

int ParseRequestMessage(int clientSocket, char* receiveMessageBuffer,
	RequestMessage* requestMessage)
{
	char tempRequestForEnv[1024];
	strcpy(tempRequestForEnv, receiveMessageBuffer);
	char tempMessageBuffer[1024];
	strcpy(tempMessageBuffer, receiveMessageBuffer);

	////// pars protocol commands
	int whileCount = 1;	// protocol's 2 splited value is must path.
	char* splitedToken = strtok(tempMessageBuffer, " ");
	char* tempRequestURL = NULL;
	char* tempRequestMethod = NULL;

	while (splitedToken != NULL)
	{
		tempRequestURL = splitedToken;
		splitedToken = strtok(NULL, " "); //protocol's second word is must path
		//hadle exception for protocol's length which just 1 or not complite.
		if (splitedToken == NULL || whileCount == 2)
		{
			break;
		}
		whileCount++;
		tempRequestMethod = tempRequestURL;
	}
	if (whileCount > 1)
	{
		strcpy(requestMessage->requestMethod, tempRequestMethod);
	}
	///////pars environment variable
	char tempRequestMessageLine[15][1024];
	int i = 0;
	strcpy(tempRequestForEnv, receiveMessageBuffer);
	splitedToken = strtok(tempRequestForEnv, "\r\n");

	while (i < 15 && splitedToken != NULL)
	{
		strcpy(tempRequestMessageLine[i], splitedToken);
		i++;
		splitedToken = strtok(NULL, "\r\n");
	}

	int j = 0;
	strcpy(requestMessage->environmentVariable[0], "/cgi-bin/");
	if (tempRequestURL != NULL)
	{
		strcpy(requestMessage->environmentVariable[1], tempRequestURL);
	}

	strcpy(requestMessage->environmentVariable[3],
		requestMessage->requestMethod);
	for (j = 0; j < i; j++)
	{
		if ((strstr(tempRequestMessageLine[j], "Content-Type:") != NULL) ||( strstr(tempRequestMessageLine[j], "Content-Length:") != NULL))
		{
			int k = 0;
			int flag = 0;
			int m = 0;
			char tempBuffer[1024];
			memset(tempBuffer, '\0', BUFFERSIZE);
			for (k = 0; k < strlen(tempRequestMessageLine[j]); k++)
			{
				if (flag == 1)
				{
					tempBuffer[m] = tempRequestMessageLine[j][k];
					m++;
				}
				if (tempRequestMessageLine[j][k] == ':')
				{
					flag = 1;
					k++;
				}
			}
			if (strstr(tempRequestMessageLine[j], "Content-Length:") != NULL)
			{
				sprintf(requestMessage->environmentVariable[2], "%d",
					atoi(tempBuffer));
			}
			else if (strstr(tempRequestMessageLine[j], "Content-Type:") != NULL)
			{
				strcpy(requestMessage->environmentVariable[4], tempBuffer);
			}
		}
	}
	strcpy(requestMessage->environmentVariable[5],
		tempRequestMessageLine[i - 1]);
	////// putting commands
	char path[BUFFERSIZE];
	memset(path, '\0', BUFFERSIZE);
	i = 9999;// i used to select state, reason to use 9999 is that command length is always under 1000
	if (path != NULL) // this loop cut off '/'
	{
		for (i = 0; i < strlen(tempRequestURL); i++)
		{
			path[i] = '\0';
			if (i != 0)
			{
				path[i - 1] = tempRequestURL[i];
			}
		}
		path[i] = '\0';
	}
	strcpy(requestMessage->requestURI, path);
	requestMessage->requestBody = (char *)malloc(
		sizeof(char)* atoi(requestMessage->environmentVariable[2]));

	memset(requestMessage->requestBody, '\0',
		atoi(requestMessage->environmentVariable[2]));
	strcpy(tempMessageBuffer, receiveMessageBuffer);

	if (strstr(tempMessageBuffer, "------") != NULL)
	{
		strncpy(requestMessage->requestBody, strstr(tempMessageBuffer, "-----"),
			1000);
		requestMessage->bodySize = strlen(requestMessage->requestBody);
		j = strlen(requestMessage->requestBody);

		while (j < atoi(requestMessage->environmentVariable[2]))
		{
			j += recv(clientSocket, requestMessage->requestBody + j, BUFFERSIZE, 0);
		}
	}
	else if (strstr(requestMessage->environmentVariable[4],
		"multipart/form-data") != NULL)
	{
		j = 0;
		while (j < atoi(requestMessage->environmentVariable[2]))
		{
			j += recv(clientSocket, requestMessage->requestBody + j, BUFFERSIZE, 0);
		}
	}

	return i;
}

int ReceiveRequestMessage(int clientSocket, char* receiveRequestMessageBuffer)
{
	int receiveValue = recv(clientSocket, receiveRequestMessageBuffer,
		BUFFERSIZE, 0);
	printf("!! %d ,%s \n", receiveValue, receiveRequestMessageBuffer);
	return receiveValue;
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
