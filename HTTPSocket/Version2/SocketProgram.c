#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/param.h>

#define MYPORT 8877 // port number
#define BACKLOG 10 // how many pending connections queue will hold
#define true 1
#define BUFFERSIZE 1024


int main(char *argc,char *argv[])
{

	int serverSocket = socket(AF_INET,SOCK_STREAM,0); //AF_INET is supported address families
	// so af_inet measning is internet ip ptrotocol , and then, socket stream is socket types

	struct sockaddr_in serverSocketAddress;//sockaddr_in is port and, internet address, size of sockaddr, address familly size , it is used to get target infomation

	serverSocketAddress.sin_family = AF_INET;
	serverSocketAddress.sin_port = htons(MYPORT);
	//sockAddrIn.sin_addr.s_addr = htonl(inet_addr("52.192.132.151"));
	serverSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	bzero(&(serverSocketAddress.sin_zero),8);

	int bindState = bind(serverSocket,(struct sockaddr *)&serverSocketAddress,sizeof(struct sockaddr));	   //1 parameter si socket discripter, 2 parameter is sockerAddIn structor pointer, 3 is socker length
	printf("hello socket %d \n",serverSocket);
	printf("hello bind %d \n",bindState);

	int listeState = listen(serverSocket, 5);
	printf("hello listen %d\n", listeState);

	int clientSocket = 0;
	int clientAddressSize = 0;
	struct sockaddr_in clientAddress;
	
	char recivedBuffer[BUFFERSIZE];
	char sendedBuffer[BUFFERSIZE];
	
	int optionNumber = 1;
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &optionNumber, sizeof(int)); 	

	while(true)
	{
		clientAddressSize = sizeof(clientAddress);
		clientSocket = accept(serverSocket,(struct sockaddr*)&clientAddress,&clientAddressSize);
		recv(clientSocket,recivedBuffer,1024,0);
		printf("%s \n",recivedBuffer);
		char* path = strtok(recivedBuffer," ");
		path = strtok(NULL," ");
		char beforeTok[BUFFERSIZE];
		int i = 0;
		for(i = 0 ; i < strlen(path);i++)
		{	
			if(i !=0) beforeTok[i-1] = path[i];
		}
		beforeTok[i] = '\0';
		printf("%s, %d \n",beforeTok,i);
		FILE *readFilePointer = fopen(beforeTok, "r");
		if (readFilePointer  == NULL && beforeTok[i] != '\0') 
		{
			char readedLine[BUFFERSIZE];
			readFilePointer = fopen("NotFoundPage.html","r");
			char* protocolText = "HTTP/1.1 404 Not Found \r\n\r\n\r\n\r\n";
			send(clientSocket,protocolText,strlen(protocolText),0);
			while(fgets(readedLine,BUFFERSIZE,readFilePointer))
			{
				if(send(clientSocket,readedLine,strlen(readedLine),0)==-1)
				{
					perror("send");
				}
			}
		}
		else
		{
			if(beforeTok[0] == '\0') 
				
			char readedLine[BUFFERSIZE];
			while  (fgets(readedLine,BUFFERSIZE,readFilePointer)) 
			{
				if(send(clientSocket,readedLine,strlen(readedLine),0)==-1)
				{
					perror("send");
				}
			}
		}
		close(clientSocket);
	}
	close(serverSocket);
	return 0;
}
