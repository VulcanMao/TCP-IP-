#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>

#define BUF_SIZE 30 
void ErrorHandling(char * message);

int main(int argc, char **argv)
{
    WSADATA wsaData;
    SOCKET sock;
    char message[BUF_SIZE];
    int strLen;

    sockaddr_in servAdr;
    if (argc != 3)
    {
        printf("Usgae : %s <IP> <port> \n", argv[0]);
        exit(1);
    }
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        ErrorHandling("WSAStartup() error!");
    }

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET)
    {
        ErrorHandling("socket() error");
    }

    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = inet_addr(argv[1]);
    servAdr.sin_port = htons(atoi(argv[2]));
	//已连接udp套接字, 多次向同一地址发送数据时, connect可省去每次设置ip:port,清除ip:port的过程
	//并且
	connect(sock, (sockaddr*)&servAdr, sizeof(servAdr));

	//bind不能用,即使成功后也不能发数据, bind和connect对udp的区别还未知
	//if (bind(sock, (struct sockaddr*)&servAdr, sizeof(servAdr)) == -1)
	//{
	//	ErrorHandling("bind() error");
	//}

    while (true)
    {
        fputs("Input message(q to quit) : ", stdout);
        fgets(message, sizeof(message), stdin);
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
        {
            break;
        }

        send(sock, message, strlen(message), 0);
        strLen = recv(sock, message, sizeof(message)-1, 0);
        message[strLen] = 0;
        printf("Message from server : %s", message);
    }
    closesocket(sock);
    WSACleanup();
    return 0;
}

void ErrorHandling(char * message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

