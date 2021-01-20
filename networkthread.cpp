#include "networkthread.h"
#include "command.h"


NetworkThread::NetworkThread()
{
}

NetworkThread::~NetworkThread()
{
}

bool NetworkThread::Initialize()
{
	_thread = thread(&NetworkThread::WorkThread, this);

	return true;
}


void NetworkThread::WorkThread()
{
	bool loop = true;
	while (loop)
	{
		unique_lock<mutex> lock(_lock);
	}

}

void NetworkThread::Wait()
{
	_thread.join();
}

void NetworkThread::ParseCommand()
{
}



