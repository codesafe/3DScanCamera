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

private :
	int SetConfigAction(const char* name, const char* value);
	int _find_widget_by_name(GPParams* p, const char* name, CameraWidget** child, CameraWidget** rootconfig);

	bool SetRadioWidgetName(string param, string value);
	bool SetRadioWidget(string param, string value);
	bool SetToggleWidget(string param, string value);

	CameraWrapper* cameraWrapper;

	string params[CAMERA_PARAM::PARAM_MAX];
};
