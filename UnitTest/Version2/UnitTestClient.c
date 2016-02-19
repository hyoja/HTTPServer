#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/param.h>
#include "HTTPServer.h"
#include "../CUnit/CUnit.h"
#include "../CUnit/Console.h"
#include "../CUnit/Basic.h"

#define MYPORT 8877 
#define BACKLOG 10
#define true 1
#define BUFFERSIZE 1024

int AssertFunction(char* chapointer,char* charpoin)
{
		

	return 0;
}

void TestOne()
{
	CU_ASSERT_STRING_EQUAL("name","name");
}


int main()
{
	CU_pSuite suite = NULL;
 	CU_initialize_registry();
 	suite = CU_add_suite("[get_date() 함수 테스트]", NULL, NULL);
 	//CU_add_test(suite, "TestOne", TestOne);
	CU_console_run_tests();
	CU_cleanup_registry();
	//printf("Hello world");
	return 0;
}


