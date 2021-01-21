#pragma once

#include "predef.h"

/*
	이 스레드에서는 GPIO / 카메라 컨트롤(From network) 명령들을 처리
*/


class CameraThread
{
public :
	CameraThread();
	~CameraThread();

	bool Initialize(int number);
	void Wait();

	static mutex _lock;
	static condition_variable wakeupEvent;

private :
	void ParseCommand(int cameralocalnumber);
	void WorkThread(int cameralocalnumber);

	int	cameranumber;
	CAMERA_MANUFACTURER	camera_manufacturer;
	thread	_thread;

	GPIO *gpio[MAX_CAMERA];


};