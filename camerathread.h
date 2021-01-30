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

	static mutex _lock[MAX_CAMERA];
	static condition_variable wakeupEvent[MAX_CAMERA];

private :
	void ParseCommand(int cameralocalnumber);
	void CameraWorkThread(int cameralocalnumber);
	void GPIODummyThread(int dummypos);

	static bool StartUpload(int camnum);
	static size_t read_callback(void* ptr, size_t size, size_t nmemb, void* userp);
	static int upload_progress[MAX_CAMERA];
	static float delaytime[MAX_CAMERA];
	static WriteThis upload[MAX_CAMERA];

	int	cameranumber;
	CAMERA_MANUFACTURER	camera_manufacturer;
	thread	_thread[MAX_CAMERA];

	GPIO *gpio[MAX_CAMERA];
	CameraControl* cameracontrol[MAX_CAMERA];



};