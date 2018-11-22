#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#define BUF_SIZE 30 
void ErrorHandling(char * message);

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	char message[30];
	WSADATA wsaData;
	SOCKET serv_sock;
	SOCKET clnt_sock;
	int str_len;
	socklen_t clnt_adr_sz;
	struct sockaddr_in serv_adr, clnt_adr;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		ErrorHandling("WSAStartup() error!");
	}
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (serv_sock == -1)
	{
		ErrorHandling("socket() error");
	}
	
#if 0
	int option;
	socklen_t optlen;
	optlen = sizeof(option);
	option = 1;
	setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&option, optlen);
#endif

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));

	if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)))
	{
		ErrorHandling("bind() error");
	}
	if (listen(serv_sock, 5) == -1)
	{
		ErrorHandling("listen error");
	}
	clnt_adr_sz = sizeof(clnt_adr);
	clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);

	while ((str_len = recv(clnt_sock, message, sizeof(message),0)) != 0)
	{
		send(clnt_sock, message, str_len,0);
		message[str_len] = 0;
	}
	closesocket(clnt_sock);
	closesocket(serv_sock);
	WSACleanup();
	return 0;
}


void ErrorHandling(char * message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

