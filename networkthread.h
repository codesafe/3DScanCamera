#pragma once

#pragma once

#include "predef.h"

class NetworkThread
{
public:
	NetworkThread();
	~NetworkThread();
	bool Initialize();

private:
	void ParseCommand();
	void WorkThread();
	void Wait();

	thread	_thread;
	mutex _lock;
};