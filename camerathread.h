#pragma once

#include "predef.h"

class CameraThread
{
public :
	CameraThread();
	~CameraThread();

	bool Initialize(int number);

	static mutex _lock;
	static condition_variable wakeupEvent;

private :
	void ParseCommand(int cameralocalnumber);

	void WorkThread(int cameralocalnumber);
	void Wait();

	int	cameranumber;
	CAMERA_MANUFACTURER	camera_manufacturer;
	thread	_thread;

	GPIO *gpio;


};