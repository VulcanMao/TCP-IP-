#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>

#include "MyBinaryReadStream.h"

#define BUF_SIZE 1024
#define RLT_SIZE 4
#define OPSZ 4

int main(int argc, char **argv)
{
    WSADATA wsaData;
    SOCKET hSocket;
    char op[10];
    int  num, i;
    sockaddr_in servAdr;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        ErrorHandling("WSAStartup() error!");
    }

    if (argc != 3)
    {
        printf("Usage %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    string writeBuf;
    MyBinaryWriteStream writeStream(&writeBuf);
    
    hSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (hSocket == INVALID_SOCKET)
    {
        ErrorHandling("socket() error");
    }

    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = inet_addr(argv[1]);
    servAdr.sin_port = htons(atoi(argv[2]));
    if (connect(hSocket, (sockaddr *)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
    {
		printf("err = %d\n", GetLastError());
        ErrorHandling("connect() error");
    }
    else
    {
        puts("Connected ......");
    }
    while (true)
    {
        writeStream.Clear();
        fputs("input first number : ", stdout);
        scanf("%d", &num);
        writeStream.WriteInt32(num);

        fputs("input the operator : ", stdout);
        scanf("%s", op);
        writeStream.WriteCString(op, sizeof(op)-1);

        fputs("input second number : ", stdout);
        scanf("%d", &num);
        writeStream.WriteInt32(num);

        writeStream.Flush();
        //发送数据
        send(hSocket, writeStream.GetData(), writeStream.GetSize(), 0);
        //接收数据
        bool result = false;
        DWORD errNo;
        char readBuf[1024];
        result = readAssignedAmountBytes(hSocket, readBuf, BINARY_PACKLEN_LEN_2, errNo);
        if (result)
        {
            unsigned int msglen = (*(unsigned int *)readBuf);
            //unsigned int msglen = ntohl(*(unsigned int *)readBuf);
            result = readAssignedAmountBytes(hSocket, readBuf + BINARY_PACKLEN_LEN_2,
                msglen - BINARY_PACKLEN_LEN_2, errNo);
            if (result)
            {
				//检验
				if (CheckData(readBuf, msglen) == true)
				{
					MyBinaryReadStream readStream(readBuf, msglen);
					int32_t a;
					readStream.ReadInt32(a);
					printf("result = %d\n", a);
				}
            }
            else
            {
                if (errNo == 10054)
                {
                    printf("远程主机强迫关闭了一个现有的连接。\n");
                    break;
                }

            }
        }
        else
        {
            if (errNo == 10054)
            {
                printf("远程主机强迫关闭了一个现有的连接。\n");
                break;
            }

        }
    }
    system("pause");
    return 0;
}