#include "camerathread.h"
#include "command.h"

mutex CameraThread::_lock;
condition_variable CameraThread::wakeupEvent;


CameraThread::CameraThread()
{
	for (int i = 0; i < MAX_CAMERA; i++)
		gpio[i] = nullptr;
}

CameraThread::~CameraThread()
{
	for (int i = 0; i < MAX_CAMERA; i++)
	{
		if (gpio[i] != nullptr)
			delete gpio[i];
		gpio[i] = nullptr;
	}
}

bool CameraThread::Initialize(int number)
{
	for (int i = 0; i < number; i++)
	{
		_thread = thread(&CameraThread::WorkThread, this, i);

		// 카메라 Shoot용
		gpio[i] = new GPIO(CAMERA_GPIO[cameranumber]);
		gpio[i]->setDirection(GPIO_DIRECTION::OUTPUT);
	}

	return true;
}


void CameraThread::WorkThread(int cameralocalnumber)
{
	printf("create thread %d\n", cameralocalnumber);

	bool loop = true;
	while (loop)
	{
		unique_lock<mutex> lock(_lock);
		//printf("Wait %d\n", threadnum);
		wakeupEvent.wait(lock);

		// 카메라를 Shoot하고 Download 용
		ParseCommand(cameralocalnumber);


		//printf("Pass %d : %s\n", cameranumber, getCurrentDateTime().c_str());
	}

}

void CameraThread::Wait()
{
	_thread.join();
}

void CameraThread::ParseCommand(int cameralocalnumber)
{
	_Command command;
	CommandQueue::getInstance()->GetCommand(command);

	if (command.type == CommandType::COMMAND_GPIO)
	{

	}
	else if (command.type == CommandType::COMMAND_NETWORK)
	{

	}
}



