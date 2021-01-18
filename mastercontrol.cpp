
#include "predef.h"
#include "mastercontrol.h"


MasterControl::MasterControl()
{
	gpio = nullptr;
}

MasterControl::~MasterControl()
{
	delete gpio;
	gpio = nullptr;
}

bool MasterControl::Initialize()
{
	gpio = new GPIO(MASTERCONTROL_GPIO);

	return true;
}