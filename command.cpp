
#include "command.h"

CommandQueue* CommandQueue::_instance = nullptr;

CommandQueue::CommandQueue()
{

}

CommandQueue::~CommandQueue()
{

}

void CommandQueue::AddCommand(_Command command)
{
	command_queue.push(command);
}

_Command CommandQueue::GetCommand()
{
	_Command command = command_queue.front();
	command_queue.pop();
	return command;
}

