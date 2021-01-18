#pragma once

#include "predef.h"

class CameraThread
{
public :
	CameraThread();
	~CameraThread();

	bool Initialize(int number);

private :
	void WorkThread(int cameranumber);
	void Wait();

	int	cameranumber;
	CAMERA_MANUFACTURER	camera_manufacturer;
	thread	_thread;

	GPIO *gpio;

	mutex _lock;
	condition_variable sEvent;
};