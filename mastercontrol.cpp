
#include "predef.h"
#include "mastercontrol.h"
#include "command.h"
#include "camerathread.h"


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
	gpio = new GPIO(MASTERCONTROL_GPIO);
	gpio->setDirection(GPIO_DIRECTION::INPUT);
	gpio->setEdgeType(GPIO_EDGE::_RISING);
	gpio->waitForEdge(&WaitInput);

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
	CameraThread::wakeupEvent.notify_all();

	return i;
}

