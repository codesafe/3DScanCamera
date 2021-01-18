#include "camerathread.h"



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
	gpio->setDirection(GPIO_DIRECTION::_OUTPUT);

	return true;
}


void CameraThread::WorkThread(int cameranumber)
{
	printf("create thread %d\n", cameranumber);

	bool loop = true;
	while (loop)
	{
		unique_lock<mutex> lock(_lock);
		//printf("Wait %d\n", threadnum);
		sEvent.wait(lock);

		// 카메라를 Shoot하고 Download 용


		//printf("Pass %d : %s\n", cameranumber, getCurrentDateTime().c_str());
	}

}

void CameraThread::Wait()
{
	_thread.join();
}