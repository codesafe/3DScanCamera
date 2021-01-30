
#include "predef.h"
#include "slaveremotecontrol.h"
#include "command.h"
#include "camerathread.h"
#include "utils.h"


/*

// Shoot Master GPIO�� ��ȣ�� (Shoot�ߴ�)
int WaitInput(int i)
{
	Logger::log("GPIO WaitInput --> %s", Utils::getCurrentDateTime().c_str());

	// ������ ī�޶����� ��ȣ --> ����
	// 1. Command --> ����
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

// �̰� Slave Rasp���� �۵��Ǿ���Ѵ�

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
	// ���� Master RASP���� �Ѿ�� Shoot ��ȣ ĸ�� --> �� ī�޶�� �۵��ϰ� Signal �߻�
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

// Shoot Master GPIO�� ��ȣ�� (Shoot�ߴ�)
int SlaveRemoteControl::WaitInput(int i)
{
	Logger::log("GPIO WaitInput --> %s", Utils::getCurrentDateTime().c_str());

	// ������ ī�޶����� ��ȣ --> ����
	// 1. Command --> ����
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
