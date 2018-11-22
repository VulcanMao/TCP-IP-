#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#define BUF_SIZE 30 
void ErrorHandling(char * message);

int main(int argc, char **argv)
{
	WSADATA wsaData;
	SOCKET tcp_sock,udp_socket;
	int sock_type;
	int state;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		ErrorHandling("WSAStartup() error!");
	}

	socklen_t optlen;

	optlen = sizeof(sock_type);
	tcp_sock = socket(PF_INET, SOCK_STREAM, 0);
	udp_socket = socket(PF_INET, SOCK_DGRAM, 0);
	printf("SOCK_STREAM : %d \n", SOCK_STREAM);
	printf("SOCK_DGRAM: %d \n", SOCK_DGRAM);

	state = getsockopt(tcp_sock, SOL_SOCKET, SO_TYPE, (char*)&sock_type, &optlen);
	if (state)
	{
		ErrorHandling("getsockopt() error!");
	}
	printf("socket type one : %d\n", sock_type);

	state = getsockopt(udp_socket, SOL_SOCKET, SO_TYPE, (char*)&sock_type, &optlen);
	if (state)
	{
		ErrorHandling("getsockopt() error!");
	}
	printf("socket type two : %d\n", sock_type);
	return 0;
}


void ErrorHandling(char * message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

