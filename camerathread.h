#pragma once

#include "predef.h"

/*
	이 스레드에서는 GPIO / 카메라 컨트롤(From network) 명령들을 처리
*/

class CameraControl;

class CameraThread
{
public :
	CameraThread();
	~CameraThread();

	bool Initialize();
	void Wait();

	static mutex _lock;
	static condition_variable wakeupEvent;

private :
	void ParseCommand(int cameralocalnumber);
	void WorkThread(int cameralocalnumber);

	int	cameranumber;
	CAMERA_MANUFACTURER	camera_manufacturer;
	thread	_thread[MAX_CAMERA];

	float delaytime[MAX_CAMERA];
	GPIO *gpio[MAX_CAMERA];
	CameraControl* cameracontrol[MAX_CAMERA];


};