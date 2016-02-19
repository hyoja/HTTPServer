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
#include "MessageTransferThread.h"

#define MYPORT 8677 
#define BACKLOG 10
#define bool int
#define true 1
#define BUFFERSIZE 1024 //why i need 1024? im really want to use 1024? or just sensitive case? i thought that it just memory ^2 times value, becouse fragmentation, paging always splited 2times value, for exaple 2 4 8 16 32....

int InitializeServer();
void WaitClient(int serverSocket);