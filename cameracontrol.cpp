#include "predef.h"
#include "cameracontrol.h"
#include "utils.h"
#include <iostream>
#include <functional>


CameraControl::CameraControl()
{
	cameraWrapper = nullptr;
}


CameraControl::~CameraControl()
{
	if(cameraWrapper != nullptr)
		delete cameraWrapper;
}

bool CameraControl::Initialize(string modelname, string port)
{
	cameraWrapper = new gphoto2pp::CameraWrapper(modelname, port);
	return true;
}

void CameraControl::SetParam(CAMERA_PARAM param, string value)
{
	params[param] = value;
}

bool CameraControl::ReleaseButton()
{
	if (SetRadioWidgetName("eosremoterelease", "Release 3") == false)
		return false;

	if (SetRadioWidgetName("eosremoterelease", "Release Full") == false)
		return false;

	return true;
}

bool CameraControl::AutoFocus()
{
	// 이렇게해야 재대로 먹음
 	if (SetRadioWidgetName("eosremoterelease", "Press 1") == false)
 		return false;

	Utils::Sleep(1);

	// 화면 풀림
 	if (SetRadioWidgetName("eosremoterelease", "Release 1") == false)
 		return false;

	Logger::log("AutoFocus");

//  	if (SetRadioWidgetName("eosremoterelease", "Press Half") == false) 
//  		return false;

// 	if (SetRadioWidgetName("eosremoterelease", "Release Half") == false)
// 		return false;

	return true;
}

bool CameraControl::ApplyParam()
{
	if (SetRadioWidget("ISO Speed", params[CAMERA_PARAM::ISO]) == false) return false;
	if (SetRadioWidget("Aperture", params[CAMERA_PARAM::APERTURE]) == false) return false;
	if (SetRadioWidget("Shutter Speed", params[CAMERA_PARAM::SHUTTERSPEED]) == false) return false;
	if (SetRadioWidget("Image Format", params[CAMERA_PARAM::CAPTURE_FORMAT]) == false) return false;

	Logger::log("ApplyParam");
	return true;
}

bool  CameraControl::SetRadioWidget(string param, string value)
{
	auto rootWidget = cameraWrapper->getConfig();
	auto radioWidget = rootWidget.getChildByLabel<gphoto2pp::RadioWidget>(param.c_str());

// 	std::cout << "\t   label:\t" << radioWidget.getLabel() << std::endl;
// 	std::cout << "\t   info:\t " << radioWidget.getInfo() << std::endl;
// 	std::cout << "\t   type:\t" << static_cast<int>(radioWidget.getType()) << std::endl;
	std::cout << "\t   value:\t" << radioWidget.getValue() << std::endl;
// 	std::cout << "\t   count:\t" << radioWidget.countChoices() << std::endl;
// 	std::cout << "\t   choices:\t" << radioWidget.choicesToString(" ") << std::endl;

	try
	{
		radioWidget.setValue(value.c_str());
		cameraWrapper->setConfig(rootWidget);
	}
	catch (gphoto2pp::exceptions::gphoto2_exception& e)
	{
		std::cout << "\tError: couldn't change the SetRadioWidget" << std::endl;
		return false;
	}
	catch (gphoto2pp::exceptions::ValueOutOfLimits& e)
	{
		std::cout << "\tError: " << e.what() << std::endl;
		return false;
	}

	return true;
}

bool  CameraControl::SetRadioWidgetName(string param, string value)
{
	auto rootWidget = cameraWrapper->getConfig();
	auto radioWidget = rootWidget.getChildByName<gphoto2pp::RadioWidget>(param.c_str());

// 	std::cout << "\t   label:\t" << radioWidget.getLabel() << std::endl;
// 	std::cout << "\t   info:\t " << radioWidget.getInfo() << std::endl;
// 	std::cout << "\t   type:\t" << static_cast<int>(radioWidget.getType()) << std::endl;
// 	std::cout << "\t   value:\t" << radioWidget.getValue() << std::endl;
// 	std::cout << "\t   count:\t" << radioWidget.countChoices() << std::endl;
// 	std::cout << "\t   choices:\t" << radioWidget.choicesToString(" ") << std::endl;

	try
	{
		radioWidget.setValue(value.c_str());
		cameraWrapper->setConfig(rootWidget);
	}
	catch (gphoto2pp::exceptions::gphoto2_exception& e)
	{
		std::cout << "\tError: couldn't change the SetRadioWidgetName" << std::endl;
		return false;
	}
	catch (gphoto2pp::exceptions::ValueOutOfLimits& e)
	{
		std::cout << "\tError: " << e.what() << std::endl;
		return false;
	}

	return true;
}

void CameraControl::StartListeningForEvents()
{
	// 뭔가 안됨. 왜안되는지 몰라
	auto myhandler = [=](const gphoto2pp::CameraFilePathWrapper& cameraFilePath, const std::string& data)
	{
		std::cout << "File Added: " << cameraFilePath.Name << std::endl;
		std::cout << "Downloading... " << std::endl;
		auto cameraFile = cameraWrapper->fileGet(cameraFilePath.Folder, cameraFilePath.Name, gphoto2pp::CameraFileTypeWrapper::Normal);
		cameraFile.save("example10_" + cameraFilePath.Name);
		std::cout << "Done!" << std::endl;
	};

	cameraWrapper->subscribeToCameraEvent(gphoto2pp::CameraEventTypeWrapper::FileAdded, myhandler);
	cameraWrapper->startListeningForEvents();
}


void CameraControl::StopListeningForEvents()
{
	cameraWrapper->stopListeningForEvents();
}

int CameraControl::GetFilefromCamera(const char* filename)
{
	Camera* _camera = (Camera*)cameraWrapper->getCamera();
	GPContext* _context = (GPContext*)cameraWrapper->getContext();

	void* data = NULL;
	CameraEventType	event;
	CameraFilePath* fn;
	int ret;
	bool loop = true;

	while (loop)
	{
		int leftoverms = 1000;
		ret = gp_camera_wait_for_event(_camera, leftoverms, &event, &data, _context);
		if (ret != GP_OK)
			return ret;

		switch (event)
		{
			case GP_EVENT_UNKNOWN:
			case GP_EVENT_TIMEOUT:
			case GP_EVENT_FOLDER_ADDED:
			case GP_EVENT_FILE_CHANGED:
				break;

			case GP_EVENT_FILE_ADDED:
			{
				fn = (CameraFilePath*)data;

				CameraFile* file;
				CameraFileInfo info;
				ret = gp_camera_file_get_info(_camera, fn->folder, fn->name, &info, _context);
				if (ret != GP_OK)
				{
					Logger::log("gp_camera_file_get_info %s : %d Error", filename, ret);

					free(data);
					return ret;
				}
				else
				{
					Logger::log("gp_camera_file_get_info %s : %s success", fn->folder, fn->name);
				}

				int fd;
				fd = open(filename, O_CREAT | O_RDWR, 0644);
				ret = gp_file_new_from_fd(&file, fd);
				if (ret != GP_OK)
				{
					Logger::log("gp_file_new_from_fd %s Error", filename);
					gp_file_unref(file);
					free(data);
					return ret;
				}

				ret = gp_camera_file_get(_camera, fn->folder, fn->name, GP_FILE_TYPE_NORMAL, file, _context);
				if (ret != GP_OK)
				{
					Logger::log("gp_camera_file_get %s : %d Error", filename, ret);
					gp_file_unref(file);
					free(data);
					return ret;
				}
				else
				{
					Logger::log("gp_camera_file_get %s : %s success", fn->folder, fn->name);
				}

				//gp_file_unref(file);
				gp_file_free(file);
				//loop = false;
			}
			break;

			case GP_EVENT_CAPTURE_COMPLETE:
			{
				loop = false;
				Logger::log("Capture %s Done!", filename);
			}
			break;
		}
		free(data);
	}

	return true;
}


