#include "camerathread.h"
#include "command.h"
#include "cameracontrol.h"
#include "networkthread.h"
#include "utils.h"

mutex CameraThread::_lock;
condition_variable CameraThread::wakeupEvent;


CameraThread::CameraThread()
{
	for (int i = 0; i < MAX_CAMERA; i++)
	{
		cameracontrol[i] = nullptr;
		gpio[i] = nullptr;
	}
}

CameraThread::~CameraThread()
{
	for (int i = 0; i < MAX_CAMERA; i++)
	{
		if (cameracontrol[i] != nullptr)
			delete cameracontrol[i];

		if (gpio[i] != nullptr)
			delete gpio[i];

		cameracontrol[i] = nullptr;
		gpio[i] = nullptr;
	}
}

bool CameraThread::Initialize()
{
	cameranumber = global_Camerainfo.size();

	for (int i = 0; i < cameranumber; i++)
	{
		_thread[i] = thread(&CameraThread::WorkThread, this, i);

#ifdef _ENABLE_GPIO
		// 카메라 Shoot용 GPIO
 		gpio[i] = new GPIO(global_CAMERA_GPIO[i]);
 		gpio[i]->setDirection(GPIO_DIRECTION::OUTPUT);
#endif
		// Camera wrapper
		string modelname = global_Camerainfo[i].modelname;
		string port = global_Camerainfo[i].port;
		cameracontrol[i] = new CameraControl();
		cameracontrol[i]->Initialize(modelname, port);
	}

	return true;
}


void CameraThread::WorkThread(int cameralocalnumber)
{
	printf("create thread %d\n", cameralocalnumber);

	bool loop = true;
	while (loop)
	{
		unique_lock<mutex> lock(_lock);
		Logger::log("Wait camera thread : %d", cameralocalnumber);
		wakeupEvent.wait(lock);
		Logger::log("Wakeup camera thread : %d", cameralocalnumber);

		// 카메라를 Shoot하고 Download 용
		ParseCommand(cameralocalnumber);

		//printf("Pass %d : %s\n", cameranumber, getCurrentDateTime().c_str());
	}

}

void CameraThread::Wait()
{
	for(int i=0; i< cameranumber;i++)
		_thread[i].join();
}

void CameraThread::ParseCommand(int cameralocalnumber)
{
	_Command command;
	CommandQueue::getInstance()->GetCommand(command);

	if (command.type == CommandType::COMMAND_GPIO)
	{
		if (command.buffer[0] == PACKET_SHOT)
		{
			if (delaytime[cameralocalnumber] > 0)
			{
				Logger::log("Shot delay enabled %d", delaytime[cameralocalnumber]);
				Utils::Sleep(delaytime[cameralocalnumber]);
			}

			// ftp path 읽어야함
			global_ftp_path = (char*)(command.buffer + 1);

			// GPIO에서 Shoot 신호들어옴
			gpio[cameralocalnumber]->setValue(HIGH);

			std::string  date = Utils::getCurrentDateTime();
			Logger::log("GPIO --> %s", date.c_str());

			string name = Utils::format_string("%s-%d.%s", global_machine_name.c_str(), cameralocalnumber,
				global_capturefile_ext.c_str());
			cameracontrol[cameralocalnumber]->GetFilefromCamera(name.c_str());

			Utils::Sleep(1);

			cameracontrol[cameralocalnumber]->ReleaseButton();
			gpio[cameralocalnumber]->setValue(LOW);
		}
	}
	else if (command.type == CommandType::COMMAND_NETWORK)
	{
		if (command.buffer[0] == PACKET_SET_PARAMETER)
		{
			// 카메라 파라메터 조정
			unsigned char i = command.buffer[1];	// iso
			unsigned char s = command.buffer[2];	// shutterspeed
			unsigned char a = command.buffer[3];	// aperture
			unsigned char f = command.buffer[4];	// format

			unsigned long temp = (command.buffer[8] & 0xff) << 24;
			temp += (command.buffer[7] & 0xff) << 16;
			temp += (command.buffer[6] & 0xff) << 8;
			temp += (command.buffer[5] & 0xff);
			float delay = *((float*)&temp);// *(float*)(buf + 5);
			delaytime[i] = delay;
			Logger::log("PACKET_SET_PARAMETER %d : %d : %d : %d %f", i, s, a, f, delay);

			cameracontrol[cameralocalnumber]->SetParam(CAMERA_PARAM::ISO, global_isoString[i]);
			cameracontrol[cameralocalnumber]->SetParam(CAMERA_PARAM::SHUTTERSPEED, global_shutterspeedString[s]);
			cameracontrol[cameralocalnumber]->SetParam(CAMERA_PARAM::APERTURE, global_apertureString[a]);
			cameracontrol[cameralocalnumber]->SetParam(CAMERA_PARAM::CAPTURE_FORMAT, global_captureformatString[f]);
			bool ret = cameracontrol[cameralocalnumber]->ApplyParam();

			char buf[TCP_BUFFER] = { 0, };
			buf[0] = PACKET_SETPARAMETER_RESULT;
			buf[1] = (char)cameralocalnumber;
			buf[2] = ret ? RESPONSE_OK : RESPONSE_FAIL;
			NetworkThread::getInstance()->Send(buf);
		}
		else if (command.buffer[0] = PACKET_AUTOFOCUS)
		{
			// 자동 포커스
			bool ret = cameracontrol[cameralocalnumber]->AutoFocus();

			char buf[TCP_BUFFER] = { 0, };
			buf[0] = PACKET_AUTOFOCUS_RESULT;
			buf[1] = (char)cameralocalnumber;
			buf[2] = ret ? RESPONSE_OK : RESPONSE_FAIL;
			NetworkThread::getInstance()->Send(buf);
		}
	}
}



