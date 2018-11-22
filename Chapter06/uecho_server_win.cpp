#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>

#define BUF_SIZE 30 
void ErrorHandling(char * message);

int main(int argc, char **argv)
{
    WSADATA wsaData;
    SOCKET servSock;
    char message[BUF_SIZE];
    int strLen;
    int clntAdrSz;

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
    
    servSock = socket(PF_INET, SOCK_DGRAM, 0);
    if (servSock == INVALID_SOCKET)
    {
        ErrorHandling("UDP socket creation error");
    }

    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAdr.sin_port = htons(atoi(argv[1]));

    if (bind(servSock, (sockaddr*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
    {
        ErrorHandling("bind() error");
    }

    while (1)
    {
        clntAdrSz = sizeof(clntAdr);
		//用servSock接收数据, 不限制数据传输对象
        strLen = recvfrom(servSock, message, BUF_SIZE, 0,
            (sockaddr*)&clntAdr, &clntAdrSz);
		//用servServ向前边获取的clntAdr传输数据
        sendto(servSock, message, strLen, 0,
            (sockaddr*)&clntAdr, sizeof(clntAdr));
    }
    closesocket(servSock);
    WSACleanup();
    return 0;
}

void ErrorHandling(char * message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

