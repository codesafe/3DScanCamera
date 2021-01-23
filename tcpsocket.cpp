
#include "predef.h"
#include "tcpsocket.h"


TCP_Socket::TCP_Socket()
{
	sock = -1;
}

TCP_Socket::~TCP_Socket()
{
}

bool TCP_Socket::init()
{
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		Logger::log("Could not create socket.");
		return false;
	}

	Logger::log("init tcp server address : %s", global_server_address.c_str());
	memset((void*)&server, 0x00, sizeof(server));

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(global_server_address.c_str());
	server.sin_port = htons(SERVER_TCP_PORT);

	int flag = 1;
	int ret = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));
	if (ret == -1)
	{
		Logger::log("socket Setoption. Error!");
		return false;
	}

	static int reuseFlag = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuseFlag, sizeof reuseFlag) != 0) 
	{
		Logger::log("[%s] setsockopt(SO_REUSEADDR) error: ", __FUNCTION__);
		close(sock);
		return false;
	}

/*
	int curFlags = fcntl(sock, F_GETFL, 0);
	if (fcntl(sock, F_SETFL, curFlags | O_NONBLOCK) < 0)
	{
		Logger::log("Set nonblock");
		close(sock);
		return false;
	}
*/

	return true;
}

void TCP_Socket::destroy()
{
	if (sock != -1)
	{
		close(sock);
		sock = -1;
	}
}

bool TCP_Socket::connect()
{

	int ret, err;
	fd_set set;
	FD_ZERO(&set);
	timeval tvout = { 3, 0 };  // timeout  
	FD_SET(sock, &set);

	if ((ret = ::connect(sock, (struct sockaddr*)&server, sizeof(server))) != 0)
	{
		err = errno;
		if (err != EINPROGRESS && err != EWOULDBLOCK) 
		{
			printf("connect() failed : %d\n", err);
			return false;
		}

		if (select(sock + 1, NULL, &set, NULL, &tvout) <= 0) 
		{
			printf("select/connect() failed : %d\n", errno);
			return false;
		}

		err = 0;
		socklen_t len = sizeof(err);
		if (getsockopt(sock, SOL_SOCKET, SO_ERROR, (char*)&err, &len) < 0 || err != 0) 
		{
			printf("getsockopt() error: %d\n", err);
			return false;
		}
	}

	Logger::log("connected %s : %d", global_server_address.c_str(), SERVER_TCP_PORT);

	int curFlags = fcntl(sock, F_GETFL, 0);
	if (fcntl(sock, F_SETFL, curFlags | O_NONBLOCK) < 0)
	{
		Logger::log("Set nonblock error");
		close(sock);
		return false;
	}

	return true;
}

int TCP_Socket::update()
{
	fd_set read_flags, write_flags;
	struct timeval waitd;          // the max wait time for an event
	int sel;

	waitd.tv_sec = 0;
	waitd.tv_usec = 1000;		// micro second
	FD_ZERO(&read_flags);
	FD_ZERO(&write_flags);
	FD_SET(sock, &read_flags);

	sel = select(sock + 1, &read_flags, &write_flags, (fd_set*)0, &waitd);
	if (sel < 0) 
		return true;	// 아무것도 없다!

	// 읽을것이 있으면 read
	if (FD_ISSET(sock, &read_flags))
	{
		FD_CLR(sock, &read_flags);

		char in[TCP_BUFFER];
		memset(&in, 0, sizeof(in));

		int recvsize = ::recv(sock, in, sizeof(in), 0);
		if (recvsize <= 0)
		{
			Logger::log("Socket recv. Error!");
			close(sock);
			return false;
		}
		else
		{
			memcpy(recvbuffer.buffer, in, recvsize);
			recvbuffer.totalsize = recvsize;

			SocketBuffer buffer;
			buffer.totalsize = recvbuffer.totalsize;
			memcpy(buffer.buffer, recvbuffer.buffer, buffer.totalsize);
			recvbufferlist.push_back(buffer);
		}
	}

	// 보낼것이 있으면 보낸다로 설정
	if (sendbuffer.totalsize > 0)
		FD_SET(sock, &write_flags);

	// 보냄
	if (FD_ISSET(sock, &write_flags))
	{
		FD_CLR(sock, &write_flags);
		int sendsize = ::send(sock, sendbuffer.buffer + sendbuffer.currentsize, sendbuffer.totalsize - sendbuffer.currentsize, 0);
		if (sendbuffer.totalsize == sendbuffer.currentsize + sendsize)
		{
			senddone();
		}
		else
		{
			sendbuffer.currentsize += sendsize;
		}
	}

	return 1;
}


int TCP_Socket::send(char* buf)
{
	int totalsend = 0;
	while (true)
	{
		int n = ::send(sock, buf + totalsend, TCP_BUFFER - totalsend, 0);
		totalsend += n;
		if (totalsend >= TCP_BUFFER)
		{
			//printf("TCP Send : %d\n", totalsend);
			break;
		}
	}

	return totalsend;
//	return ::send(sock, buf, TCP_BUFFER, 0);
}

int TCP_Socket::recv(char* buf)
{
	int totalrecv = 0;
	while (true)
	{
		int n = ::recv(sock, buf+totalrecv, TCP_BUFFER - totalrecv, 0);
		totalrecv += n;
		if (totalrecv >= TCP_BUFFER)
		{
			//printf("TCP Recv : %d\n", totalrecv);
			break;
		}
	}

	return totalrecv;
	//return ::recv(sock, buf, TCP_BUFFER, 0);
}

void TCP_Socket::addsendpacket(char* packet)
{
	if (sendbuffer.totalsize > 0)
	{
		// 전송 중이라 쌓아 놓는다
		SocketBuffer buf;
		buf.totalsize = TCP_BUFFER;
		memcpy(buf.buffer, packet, TCP_BUFFER);
		sendbufferlist.push_back(buf);
	}
	else
	{
		// 바로 전송할거로 이동
		sendbuffer.totalsize = TCP_BUFFER;
		sendbuffer.currentsize = 0;
		memcpy(sendbuffer.buffer, packet, TCP_BUFFER);
	}
}

void	TCP_Socket::senddone()
{
	if (!sendbufferlist.empty())
	{
		sendbuffer.totalsize = sendbufferlist[0].totalsize;
		sendbuffer.currentsize = 0;
		memcpy(sendbuffer.buffer, sendbufferlist[0].buffer, sendbufferlist[0].totalsize);
		sendbufferlist.pop_front();
	}
	else
	{
		sendbuffer.totalsize = -1;
		sendbuffer.currentsize = 0;
		memset(sendbuffer.buffer, 0, TCP_BUFFER);
	}
}


bool	TCP_Socket::getrecvpacket(char* buffer)
{
	if (!recvbufferlist.empty())
	{
		memcpy(buffer, recvbufferlist[0].buffer, TCP_BUFFER);
		recvbufferlist.pop_front();
		return true;
	}

	return false;
}
