#include "HTTPServer.h"

void TransferMessage(char* receiveMessageBuffer,int clientSocket)
{
	char requestURL[BUFFERSIZE];
	char requestMethod[8];
	memset(requestURL,'\0',BUFFERSIZE);
	memset(requestMethod, '\0', 8);
	int i = ParsingRequestMessage(receiveMessageBuffer,clientSocket,requestURL,requestMethod);
	//i is index of message length for bad request.
	int sendPacketState = SelectSendState(requestMethod,requestURL,i);
	printf("!! requestURL %s , requestMethod %s \n",requestURL,requestMethod);
////// send packetMessage
	SendMessage(sendPacketState,requestURL, clientSocket);
}

void SendMessage(int sendPacketState, char* requestURL, int clientSocket)
{
	char* notFoundText = NULL;
	char readedLine[BUFFERSIZE];
	FILE* readFilePointer = NULL;

	switch(sendPacketState)	
	{
		case 200:
			readFilePointer = fopen(requestURL, "r");
			notFoundText = "HTTP/1.1 200 OK\r\n\r\n";
			send(clientSocket,notFoundText,strlen(notFoundText),0);

			memset(readedLine, '\0', BUFFERSIZE);
			while(fread(readedLine, 1, BUFFERSIZE, readFilePointer)) 
			{
				if(send(clientSocket, readedLine, BUFFERSIZE, 0) == -1)
				{
					perror("send");
				}
			}
			break;
		case 400:
			notFoundText = "HTTP/1.1 404 Not Found\r\n\r\n<html><body>Not Found</body></html>";
			send(clientSocket, notFoundText, strlen(notFoundText),0);
			break;
		case 404:
			notFoundText = "HTTP/1.1 400 Bad Request\r\n\r\n<html><body>Bad Request</body></html>";
			send(clientSocket, notFoundText, strlen(notFoundText), 0);
			break;
	}

}

int SelectSendState(char* requestMethod, char* requestURL,int i)
{	
////// select states
	FILE* readFilePointer = NULL;
	int sendPacketState = -1; // -1 is Not select, 0 is defualt page, 400 is bad request
	if((strcmp(requestMethod,"GET") != 0) || (requestURL[0] == '/') || (i > 1000) )
	{
		sendPacketState = 404;
	}
	else if((requestURL[0] == '\0') || (strcmp(requestURL,"main") == 0) || (strcmp(requestURL,"SocketProgram.c") == 0))
	{
		char* tempURL = "HelloWorld.html";
		strcpy(requestURL,tempURL);
		sendPacketState = 200;
	}
	else if((readFilePointer = fopen(requestURL, "r")) == NULL)
	{
		sendPacketState = 400;
	}
	else if((readFilePointer = fopen(requestURL, "r")) != NULL)
	{
		sendPacketState = 200;
	}
	else
	{
		sendPacketState = 404; // defualt bad man!
	}
	return sendPacketState;
}

void AcceptMessage(int serverSocket)
{
	int clientSocket = 0;
	int clientAddressSize = 0;
	struct sockaddr_in clientAddress;
	char receiveMessageBuffer[BUFFERSIZE];
	char sendMessageBuffer[BUFFERSIZE];
	memset(receiveMessageBuffer,'\0',BUFFERSIZE);
	memset(sendMessageBuffer,'\0',BUFFERSIZE);
	while(true)
	{
		clientAddressSize = sizeof(clientAddress);
		clientSocket = accept(serverSocket,(struct sockaddr*)&clientAddress,&clientAddressSize);
		int receiveValue = recv(clientSocket,receiveMessageBuffer,BUFFERSIZE,0);
		printf("!! %d ,%s \n",receiveValue,receiveMessageBuffer);
		TransferMessage(receiveMessageBuffer,clientSocket);
		if(receiveValue < BUFFERSIZE)
		{	
			close(clientSocket);	
		}
	}
}

int ParsingRequestMessage(char* receiveMessageBuffer,int clientSocket,char* requestURL,char* requestMethod)
{
////// pars protocol commands
	int whileCount = 1;	// protocol's 2 splited value is must path.
	char *splitedToken = strtok(receiveMessageBuffer," ");
	char *tempRequestURL = NULL;
	char *tempRequestMethod = NULL;
	while(splitedToken != NULL)
	{
		tempRequestURL = splitedToken;
		splitedToken = strtok(NULL," "); //protocol's second word is must path
		//hadle exception for protocol's length which just 1 or not complite.
		if(splitedToken == NULL || whileCount == 2) 
		{
			break;
		}
		whileCount++;
		tempRequestMethod = tempRequestURL;
	}
	if(whileCount > 1)
	{
		strcpy(requestMethod,tempRequestMethod);
	}

////// putting commands
	char path[BUFFERSIZE];
	memset(path,'\0',BUFFERSIZE);
	//printf("i need stream at %s \n",requestURL);
	int i = 9999;// i used to select state, reason to use 9999 is that command length is always under 1000
	if(path != NULL) // this loop cut off '/'
	{
		for(i = 0 ; i < strlen(tempRequestURL); i++)
		{
			path[i] = '\0';
			if(i !=0)
			{
				path[i-1] = tempRequestURL[i];
			}
		}
		path[i] = '\0';
	}
	strcpy(requestURL,path);

	return i;
}

int InitializeServer()
{
	int serverSocket =0;	
	serverSocket = socket(AF_INET, SOCK_STREAM, 0); //AF_INET is supported like address families;
	struct sockaddr_in serverSocketAddress;
	serverSocketAddress.sin_family = AF_INET;
	serverSocketAddress.sin_port = htons(MYPORT);
	serverSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	bzero(&(serverSocketAddress.sin_zero),8);
	int bufferPointer[5]; // array store to binding wating
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &bufferPointer, sizeof(bufferPointer));
	//1 parameter si socket discripter, 2 parameter is sockerAddIn structor pointer, 3 is socker length
	int bindState = bind(serverSocket,(struct sockaddr *)&serverSocketAddress,sizeof(struct sockaddr));
	int listenState = listen(serverSocket, 5);

	if(serverSocket > -1)
	{
		printf("i opened server socket %d \n",serverSocket);
	}
	else
	{
		printf("fail server socket\n");
		return -1;
	}
	if(bindState > -1)
	{
		printf("i'm binded %d \n",bindState);
	}
	else
	{
		printf("fail binding\n");
		return -1;
	}
	if(listenState > -1)
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
