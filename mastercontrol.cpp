
#include "predef.h"
#include "mastercontrol.h"
#include "command.h"
#include "camerathread.h"
#include "utils.h"

// 이건 Slave Rasp에만 작동되어야한다

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
	// 이전 RASP에서 넘어온 Shoot 신호 캡쳐 --> 각 카메라들 작동하게 Signal 발생
#ifdef _ENABLE_GPIO
	gpio = new GPIO(MASTERCONTROL_GPIO);
	gpio->setDirection(GPIO_DIRECTION::INPUT);
	gpio->setEdgeType(GPIO_EDGE::RISING);
	gpio->waitForEdge(&WaitInput);
#endif
	return true;
}

// Shoot Master GPIO에 신호옴 (Shoot했다)
int MasterControl::WaitInput(int i)
{
	Logger::log("--> %s", Utils::getCurrentDateTime().c_str());

	// 각각의 카메라깨우기 신호 --> 찍어라
	// 1. Command --> 찍어라
	_Command command;
	command.type = CommandType::COMMAND_GPIO;
	command.buffer[0] = PACKET_SHOT;
	CommandQueue::getInstance()->AddCommand(command);

	// 2. Thread Wake
	for (int i = 0; i < MAX_CAMERA; i++)
	{
		CameraThread::wakeupEvent[i].notify_one();
	}

	return i;
}

