#pragma once

#include "gpio/gpio.h"

class SlaveRemoteControl
{
public :
	static SlaveRemoteControl* getInstance()
	{
		if (_instance == nullptr)
			_instance = new SlaveRemoteControl();
		return _instance;
	}
	bool Initialize();

private:
	SlaveRemoteControl();
	~SlaveRemoteControl();

	static int WaitInput(int i);

	static SlaveRemoteControl* _instance;
	GPIO* gpio;
};