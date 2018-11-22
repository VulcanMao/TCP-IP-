#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "MyBinaryReadStream.h"

#define BUF_SIZE 1024

int main(int argc, char **argv)
{

    WSADATA wsaData;
    SOCKET hServSock, hClntSock;
    char opinfo[BUF_SIZE];
    sockaddr_in servAdr, clntAdr;
    int clntAdrSize;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        ErrorHandling("WSAStartup() error");
    }

    if (argc != 2)
    {
        printf("Usage : %s <port> \n", argv[0]);
        exit(1);
    }

    hServSock = socket(PF_INET, SOCK_STREAM, 0);
    
    if (hServSock == INVALID_SOCKET)
    {
        ErrorHandling("socket() error");
    }
    
    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAdr.sin_port = htons(atoi(argv[1]));

	//设置端口
	socklen_t optlen;
	int option;
	optlen = sizeof(option);
	option = 1;
	setsockopt(hServSock, SOL_SOCKET, SO_REUSEADDR, (const char *)&option, optlen);

    if (bind(hServSock, (sockaddr*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
    {
		DWORD err = GetLastError();
        ErrorHandling("bind() error");
    }
    if (listen(hServSock, 5) == SOCKET_ERROR)
    {
        ErrorHandling("listen() error");
    }

    clntAdrSize = sizeof(clntAdr);

    for (int i = 0; i < 5; ++i)
    {
        hClntSock = accept(hServSock, (sockaddr*)&clntAdr, &clntAdrSize);
        if (hClntSock != INVALID_SOCKET)
        {
            bool result;
            DWORD errNo;
            char readBuf[1024];
            while (true)
            {
                result = readAssignedAmountBytes(hClntSock, readBuf, BINARY_PACKLEN_LEN_2, errNo);
                if (result)
                {
					//实际不用转
                    //unsigned int msglen = ntohl(*(unsigned int *)readBuf);
                    unsigned int msglen = (*(unsigned int *)readBuf);
                    result = readAssignedAmountBytes(hClntSock, readBuf + BINARY_PACKLEN_LEN_2,
                        msglen - BINARY_PACKLEN_LEN_2, errNo);
                    if (result)
                    {
						//检测
						if (CheckData(readBuf, msglen) == true)
						{

							MyBinaryReadStream readStream(readBuf, msglen);
							int firstNum, secondNum;
							char op[10];
							readStream.ReadInt32(firstNum);
							size_t len;
							readStream.ReadCString(op, sizeof(op) - 1, len);

							readStream.ReadInt32(secondNum);
							printf("%d %s %d\n", firstNum, op, secondNum);

							//发送计算结果
							string writeBuf;
							MyBinaryWriteStream writeStream(&writeBuf);
							int writeResult = 0;
							switch (op[0])
							{
							case '+':
								writeResult = firstNum + secondNum;
								break;
							case '-':
								writeResult = firstNum - secondNum;
								break;
							case '*':
								writeResult = firstNum * secondNum;
								break;
							case '/':
								writeResult = firstNum / secondNum;
								break;
							default:
								writeResult = -1;
								break;
							}
							writeStream.WriteInt32(writeResult);
							writeStream.Flush();
							send(hClntSock, writeStream.GetData(), writeStream.GetSize(), 0);

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
            closesocket(hClntSock);
        }
    }
    closesocket(hServSock);
    system("pause");
    return 0;
}