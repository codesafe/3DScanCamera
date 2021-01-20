#include "camerathread.h"
#include "command.h"

mutex CameraThread::_lock;
condition_variable CameraThread::wakeupEvent;


CameraThread::CameraThread()
{
	gpio = nullptr;
}

CameraThread::~CameraThread()
{
	delete gpio;
	gpio = nullptr;
}

bool CameraThread::Initialize(int number)
{
	cameranumber = number;
	_thread = thread(&CameraThread::WorkThread, this, cameranumber);

	// 카메라 Shoot용
	gpio = new GPIO(CAMERA_GPIO[cameranumber]);
	gpio->setDirection(GPIO_DIRECTION::OUTPUT);

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



