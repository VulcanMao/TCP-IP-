#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>

#define BUF_SIZE 30 
void ErrorHandling(char * message);

int main(int argc, char **argv)
{
    WSADATA wsaData;
    SOCKET hSocket;
	FILE *fp;

    char buf[BUF_SIZE];
    int strLen;

    SOCKADDR_IN servAdr;
    if (argc != 3)
    {
        printf("Usgae : %s <IP> <port> \n", argv[0]);
        exit(1);
    }
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        ErrorHandling("WSAStartup() error!");
    }

    hSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (hSocket == INVALID_SOCKET)
    {
        ErrorHandling("socket() error");
    }

    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = inet_addr(argv[1]);
    servAdr.sin_port = htons(atoi(argv[2]));
	connect(hSocket, (sockaddr*)&servAdr, sizeof(servAdr));

	fp = fopen("receive.dat", "wb");

	int readCnt;
	while ((readCnt = recv(hSocket, buf, BUF_SIZE, 0)) != 0)
	{
		fwrite((void*)buf, 1, readCnt, fp);
	}
	puts("Received file data");
	send(hSocket, "Thank you", 10, 0);
	fclose(fp);
    closesocket(hSocket);
    WSACleanup();
    return 0;
}

void ErrorHandling(char * message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

