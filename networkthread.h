#pragma once

#pragma once

#include "predef.h"

class TCP_Socket;

class NetworkThread
{
public:
	static NetworkThread* getInstance()
	{
		if (_instance == nullptr)
			_instance = new NetworkThread();
		return _instance;
	}

	bool Initialize();
	void Wait();

	void Send(char* buf);

private:
	NetworkThread();
	~NetworkThread();

	void Update();
	void ParseCommand();
	void WorkThread();
	void ResetBuffer();

	thread	_thread;
	mutex _lock;

	TCP_Socket	*tcp_socket;
	char buffer[TCP_BUFFER];

	static NetworkThread *_instance;
};