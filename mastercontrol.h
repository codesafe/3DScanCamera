#pragma once

#include "gpio/gpio.h"

class MasterControl
{
public :
	MasterControl();
	~MasterControl();

	bool Initialize();

private:
	static int WaitInput(int i);

	GPIO* gpio;
};