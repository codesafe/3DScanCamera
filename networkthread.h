#pragma once

#pragma once

#include "predef.h"

class TCP_Socket;

class NetworkThread
{
public:
	NetworkThread();
	~NetworkThread();
	bool Initialize();
	void Wait();

private:
	void ParseCommand();
	void WorkThread();


	void ResetBuffer();

	thread	_thread;
	mutex _lock;

	TCP_Socket	*tcp_socket;
	char buffer[TCP_BUFFER];
};