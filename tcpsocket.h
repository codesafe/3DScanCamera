#pragma once

#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <netinet/tcp.h>

struct SocketBuffer
{
	int		totalsize;		// 전체 길이
	int		currentsize;	// 진행중인 버퍼 위치 (송/수신 모두)
	char	buffer[TCP_BUFFER];
	SocketBuffer()
	{
		totalsize = -1;
		currentsize = 0;
		memset(buffer, 0, TCP_BUFFER);
	}
};

class TCP_Socket
{
public:
	TCP_Socket();
	~TCP_Socket();

	bool init();
	void destroy();
	bool connect();
	int update();

	// 실제로 보내는것 아님
	void	addsendpacket(char* packet);
	void	senddone();
	// 수신된 패킷있으면 받어
	bool	getrecvpacket(char* buffer);

	int send(char* buf);
	int recv(char* buf);

private:

	int	sock;
	struct sockaddr_in server;
	struct sockaddr_in clntAddr;

	std::deque<SocketBuffer>	recvbufferlist;
	std::deque<SocketBuffer>	sendbufferlist;

	SocketBuffer sendbuffer;
	SocketBuffer recvbuffer;

};
