#pragma once

#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <netinet/tcp.h>

struct SocketBuffer
{
	int		totalsize;		// ��ü ����
	int		currentsize;	// �������� ���� ��ġ (��/���� ���)
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

	// ������ �����°� �ƴ�
	void	addsendpacket(char* packet);
	void	senddone();
	// ���ŵ� ��Ŷ������ �޾�
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
