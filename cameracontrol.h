#pragma once

#include "predef.h"

class CameraControl
{
public :
	CameraControl();
	~CameraControl();

	bool Initialize(string modelname, string port);
	void SetParam(CAMERA_PARAM param, string value);
	bool ApplyParam();
	bool AutoFocus();
	bool ReleaseButton();

	int GetFilefromCamera(const char* filename);

	void StartListeningForEvents();
	void StopListeningForEvents();

private :

	bool SetRadioWidgetName(string param, string value);
	bool SetRadioWidget(string param, string value);
	CameraWrapper* cameraWrapper;

	string params[CAMERA_PARAM::PARAM_MAX];
};
