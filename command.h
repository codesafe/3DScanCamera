#pragma once

#include <queue>

enum CommandType
{
	COMMAND_NONE,
	COMMAND_NETWORK,
	COMMAND_GPIO,
};

#define COMMAND_BUFFERSIZE	8

struct _Command
{
	CommandType	type;
	char buffer[COMMAND_BUFFERSIZE];
};


class CommandQueue
{
	public :
		static CommandQueue* getInstance()
		{
			if (_instance == nullptr)
				_instance = new CommandQueue();
			return _instance;
		}

		void ClearCommand();
		void GetCommand(_Command &command);
		void AddCommand(_Command &command);

	private:
		CommandQueue();
		~CommandQueue();

		static CommandQueue* _instance;
		mutex _lock;
		_Command command;

		//std::queue<_Command> command_queue;
};
