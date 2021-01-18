#pragma once

#include <queue>

enum CommandType
{
	NETWORK_COMMAND,
	GPIO_COMMAND,
};


struct _Command
{
	CommandType	type;
	char buffer[8];
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

		_Command GetCommand();
		void AddCommand(_Command command);

	private:
		CommandQueue();
		~CommandQueue();

		static CommandQueue* _instance;

		std::queue< _Command> command_queue;
};
