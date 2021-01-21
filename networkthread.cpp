#include "networkthread.h"
#include "command.h"
#include "tcpsocket.h"

NetworkThread::NetworkThread()
{
	tcp_socket = new TCP_Socket();
	ResetBuffer();
}

NetworkThread::~NetworkThread()
{
	delete tcp_socket;
	tcp_socket = nullptr;
}

bool NetworkThread::Initialize()
{
	_thread = thread(&NetworkThread::WorkThread, this);
	return tcp_socket->init();
}


void NetworkThread::WorkThread()
{
	bool loop = true;
	while (loop)
	{
//		unique_lock<mutex> lock(_lock);
		tcp_socket->recv(buffer);
		ParseCommand();
	}

}

void NetworkThread::Wait()
{
	_thread.join();
}

void NetworkThread::ParseCommand()
{
	// TODO. parse recv buffer


	// TODO. Add command --> wakeup camera thread
	_Command command;
	command.type = CommandType::COMMAND_NETWORK;
	CommandQueue::getInstance()->AddCommand(command);

	ResetBuffer();
}

void NetworkThread::ResetBuffer()
{
	memset(buffer, 0, TCP_BUFFER);
}

