#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>

#define BUF_SIZE 30 
void ErrorHandling(char * message);

int main(int argc, char **argv)
{
	WSADATA wsaData;
	SOCKET hServSock;
	SOCKET hClntSock;
	char buf[BUF_SIZE];
	int strLen;
	int clntAdrSz;
	FILE *fp;

	sockaddr_in servAdr, clntAdr;
	if (argc != 2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		ErrorHandling("WSAStartup() error!");
	}

	hServSock = socket(PF_INET, SOCK_STREAM, 0);
	if (hServSock == INVALID_SOCKET)
	{
		ErrorHandling("UDP socket creation error");
	}

	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons(atoi(argv[1]));

	if (bind(hServSock, (sockaddr*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
	{
		ErrorHandling("bind() error");
	}

	listen(hServSock, 5);

	clntAdrSz = sizeof(clntAdr);
	hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &clntAdrSz);

	fp = fopen("Chapter07.sln","rb");
	int readCnt;
	while (true)
	{
		readCnt = fread((void*)buf, 1, BUF_SIZE, fp);
		if (readCnt < BUF_SIZE)
		{
			send(hClntSock, (char*)buf, readCnt, 0);
			break;
		}
		send(hClntSock, (char*)buf, BUF_SIZE, 0);
	}

	//断开输出流,
	shutdown(hClntSock, SD_SEND);
	//此时还可以读
	recv(hClntSock, (char*)buf, BUF_SIZE, 0);
	
	printf("Message from client : %s\n", buf);

	fclose(fp);
	closesocket(hClntSock);
    closesocket(hServSock);
    WSACleanup();
    return 0;
}

void ErrorHandling(char * message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

