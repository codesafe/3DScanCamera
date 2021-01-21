#pragma once

#include "predef.h"

/*
	�� �����忡���� GPIO / ī�޶� ��Ʈ��(From network) ��ɵ��� ó��
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