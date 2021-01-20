
#include "predef.h"
#include "command.h"

CommandQueue* CommandQueue::_instance = nullptr;

CommandQueue::CommandQueue()
{
	ClearCommand();
}

CommandQueue::~CommandQueue()
{
}

void CommandQueue::ClearCommand()
{
	unique_lock<mutex> lock(_lock);
	command.type = COMMAND_NONE;
	memset(command.buffer, 0, COMMAND_BUFFERSIZE);
}

void CommandQueue::AddCommand(_Command &_command)
{
	//command_queue.push(command);

	unique_lock<mutex> lock(_lock);
	command.type = _command.type;
	memcpy(command.buffer, _command.buffer, COMMAND_BUFFERSIZE);

}

void CommandQueue::GetCommand(_Command &_command)
{
	//_Command command = command_queue.front();
	//command_queue.pop();

	unique_lock<mutex> lock(_lock);
	_command.type = command.type;
	memcpy(_command.buffer, command.buffer, COMMAND_BUFFERSIZE);
}

