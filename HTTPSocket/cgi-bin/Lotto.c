#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>



int main(int argc, char* argv[])
{
	wait(2);
	printf("Content-type: text/html\r\n\r\n");
	
	char tempbuff[1024];
	int messageLenght = atoi(getenv("CONTENT_LENGTH"));
	int count = 0;
	int offset = 0;
	int total = 0;
	memset(tempbuff,'\0',1024);
	char* buf = (char *)malloc(sizeof(char)*atoi(getenv("CONTENT_LENGTH")));
	char* tokenValue;
	memset(buf,'\0', sizeof(char)*atoi(getenv("CONTENT_LENGTH")));
	
	while(1)
	{
		count = read(STDIN_FILENO, buf + total, 1);
		total += 1;
		if (total >= atoi(getenv("CONTENT_LENGTH")))
		{
			//printf("%s",buf);
			break;
		}
	}
	tokenValue = strtok(buf,"\n");
	tokenValue = strtok(NULL,"\n");
	tokenValue = strtok(NULL,"\n");
	tokenValue = strtok(NULL,"\n");
	int i = 0, j = 0;
	char tempNumber[2] = {'\0',};
	int lottoNumber[46] ={ 0,};
	while(1)
	{
		tokenValue = strtok(NULL,"\n");
		if(strstr(tokenValue,"--") != NULL)
		{
			break;
		}
		else
		{
			for(i = 0; i < strlen(tokenValue); i++)
			{
				if(tokenValue[i] == ',')
				{
					if( j == 1)
					{
						tempNumber[1] = '\0';
					}
					if(atoi(tempNumber)<46 && 0<atoi(tempNumber))
					{
						lottoNumber[atoi(tempNumber)] += 1;
					}
					j = 0;
					memset(tempNumber,'\0',2);
				}
				else if( tokenValue[i] == ' ')
				{	
					j = 0;
					memset(tempNumber,'\0',2);
				}
				else
				{
					tempNumber[j] = tokenValue[i];
					j++;
				}
			}
		}
	}
	int mostBigValue = 0;
	int number =0;
	for(j = 0; j< 5; j++)
	{
		for(i = 1 ; i<46;i++)
		{
			if(lottoNumber[i] > mostBigValue)
			{
				mostBigValue = lottoNumber[i];
				number = i;
				lottoNumber[i] = 0;
			}
		}
		printf("%d,",number);
		mostBigValue = 0;
		number = 0;
	}

/*
	while(1)
	{
		memset(tempbuff,'\0',1024);
		read(STDIN_FILENO, tempbuff , 1024);
		if(count>messageLenght)
		{
			break;
		}
		printf("%s\n",tempbuff);
		count += 1024;
	}
*/
	//printf("%s im so \n",argv[1]);
	//send( atoi(argv[1]), message,strlen(message) ,0);#include <stdio.h>
//	printf("Query string %s<br>\n", getenv("QUERY_STRING"));    // ���� ���ڿ� 
//	printf("request uri %s<br>\n", getenv("REQUEST_URI"));    // URI����
/*
   printf( "HOSTNAME = %sn", getenv( "HOSTNAME"));
   printf( "TERM     = %sn", getenv( "TERM"    ));
   printf( "SHELL    = %sn", getenv( "SHELL"   ));
   printf( "QTDIR    = %sn", getenv( "QTDIR"   ));
   printf( "HOME     = %sn", getenv( "HOME"    ));

	printf("gate String %s<br>\n", getenv("GATEWAY_INTERFACE"));    // ���� gateway �������̽� (�̰�� CGI 1.1) 
	printf("http accept String %s<br>\n", getenv("HTTP_ACCEPT"));
	printf("http String %s<br>\n", getenv("HTTP_REFERER"));    // POST����� ��� ��û�� ������ �̸�
	printf("http accept String %s<br>\n", getenv("HTTP_ACCEPT_CHARSET"));
	printf("http accept language String %s<br>\n", getenv("HTTP_ACCEPT_LANGUAGE"));
	printf("http connection %s<br>\n", getenv("HTTP_CONNECTION"));    // ������ (���� Keep-Alive) 
	printf("http host %s<br>\n", getenv("HTTP_HOST"));    // �������� IP�� PORT���� 
	printf("http user agent %s<br>\n", getenv("HTTP_USER_AGENT"));    // ���ῡ ����� ������ ����
	printf("remote addr %s<br>\n", getenv("REMOTE_ADDR"));    // Ŭ���̾�Ʈ �ּ�
	printf("remote host %s<br>\n", getenv("REMOTE_HOST"));
	printf("remote port %s<br>\n", getenv("REMOTE_PORT"));    // Ŭ���̾�Ʈ ��Ʈ
	printf("request method %s<br>\n", getenv("REQUEST_METHOD"));    // ��û���, ���� GET�̳� POST
	printf("script filename %s<br>\n", getenv("SCRIPT_FILENAME"));
	printf("script name %s<br>\n", getenv("SCRIPT_NAME"));    // ������ CGI ����?׷?�̸� 
	printf("server admin %s<br>\n", getenv("SERVER_ADMIN"));    // ���������� ����, ���� email�ּ�
	printf("server name %s<br>\n", getenv("SERVER_NAME"));    // �����̸�, ���� �������̸�
	printf("server port %s<br>\n", getenv("SERVER_PORT"));    // ���� ��Ʈ(80, 8080)
	printf("server protocol %s<br>\n", getenv("SERVER_PROTOCOL"));    // ���� ��������(HTTP1.0, HTTP1.1)
	printf("server software %s<br>\n", getenv("SERVER_SOFTWARE"));    // ���� ���α׷�
	printf("tz %s<br>\n", getenv("TZ"));    // Ÿ����*/
}



