#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    printf("Content-type: text/html\r\n\r\n");
	printf("SCRIPT String %s<br>\n", getenv("SCRIPT_NAME"));	// ������ ��ġ�ϴ� ��Ʈ ���丮
	printf("CONTENT length String %s<br>\n", getenv("CONTENT_LENGTH"));	// ������ ��ġ�ϴ� ��Ʈ ���丮
	printf("REQUEST String %s<br>\n", getenv("REQUEST_METHOD"));	// ������ ��ġ�ϴ� ��Ʈ ���丮
	printf("CONTENT type String %s<br>\n", getenv("CONTENT_TYPE"));	// ������ ��ġ�ϴ� ��Ʈ ���丮
	printf("QUERY String %s<br>\n", getenv("QUERY_STRING"));	// ������ ��ġ�ϴ� ��Ʈ ���丮
	printf("REQUEST uri %s<br>\n", getenv("REQUEST_URI"));	// ������ ��ġ�ϴ� ��
	
	char tempbuff[1024];
	int messageLenght = atoi(getenv("CONTENT_LENGTH"));
	int count = 0;
	memset(tempbuff,'\0',1024);
	
	while(count < messageLenght)
	{
		read(STDIN_FILENO, tempbuff , 1);
		printf("%s",tempbuff);
		memset(tempbuff,'\0',1024);
		count++;
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



