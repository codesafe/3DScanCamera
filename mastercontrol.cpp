
#include "predef.h"
#include "mastercontrol.h"
#include "command.h"
#include "camerathread.h"
#include "utils.h"

// �̰� Slave Rasp���� �۵��Ǿ���Ѵ�

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
	// ���� RASP���� �Ѿ�� Shoot ��ȣ ĸ�� --> �� ī�޶�� �۵��ϰ� Signal �߻�
#ifdef _ENABLE_GPIO
	gpio = new GPIO(MASTERCONTROL_GPIO);
	gpio->setDirection(GPIO_DIRECTION::INPUT);
	gpio->setEdgeType(GPIO_EDGE::RISING);
	gpio->waitForEdge(&WaitInput);
#endif
	return true;
}

// Shoot Master GPIO�� ��ȣ�� (Shoot�ߴ�)
int MasterControl::WaitInput(int i)
{
	Logger::log("--> %s", Utils::getCurrentDateTime().c_str());

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

