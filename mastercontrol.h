#pragma once

#include "gpio/gpio.h"

class MasterControl
{
public :
	MasterControl();
	~MasterControl();

	bool Initialize();

private:
	GPIO* gpio;
};