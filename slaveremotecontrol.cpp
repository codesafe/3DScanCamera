
#include "predef.h"
#include "slaveremotecontrol.h"
#include "command.h"
#include "camerathread.h"
#include "utils.h"


/*

// Shoot Master GPIO에 신호옴 (Shoot했다)
int WaitInput(int i)
{
	Logger::log("GPIO WaitInput --> %s", Utils::getCurrentDateTime().c_str());

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

*/

SlaveRemoteControl* SlaveRemoteControl::_instance = nullptr;

// 이건 Slave Rasp에만 작동되어야한다

SlaveRemoteControl::SlaveRemoteControl()
{
	gpio = nullptr;
}

SlaveRemoteControl::~SlaveRemoteControl()
{
	delete gpio;
	gpio = nullptr;
}

bool SlaveRemoteControl::Initialize()
{
	// 이전 Master RASP에서 넘어온 Shoot 신호 캡쳐 --> 각 카메라들 작동하게 Signal 발생
#ifdef _ENABLE_GPIO
	Logger::log("Create Slave Remote GPIO %d", MASTERCONTROL_GPIO);

	gpio = new GPIO(MASTERCONTROL_GPIO);
	gpio->setDirection(GPIO_DIRECTION::INPUT);
 	//gpio->setEdgeType(GPIO_EDGE::RISING);
	gpio->setEdgeType(GPIO_EDGE::FALLING);
 	gpio->waitForEdge(&WaitInput);
#endif
	return true;
}

// Shoot Master GPIO에 신호옴 (Shoot했다)
int SlaveRemoteControl::WaitInput(int i)
{
	Logger::log("GPIO WaitInput --> %s", Utils::getCurrentDateTime().c_str());

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
