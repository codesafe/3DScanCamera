#include "camerathread.h"
#include "command.h"
#include "cameracontrol.h"
#include "networkthread.h"
#include "utils.h"

mutex CameraThread::_lock[MAX_CAMERA];
condition_variable CameraThread::wakeupEvent[MAX_CAMERA];
int CameraThread::upload_progress[MAX_CAMERA] = { 0, };
float CameraThread::delaytime[MAX_CAMERA] = { 0.f, };
WriteThis CameraThread::upload[MAX_CAMERA];


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
		gpio[i]->setValue(LOW);
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
		unique_lock<mutex> lock(_lock[cameralocalnumber]);
		Logger::log("Wait camera thread : %d", cameralocalnumber);
		wakeupEvent[cameralocalnumber].wait(lock);

		string  date = Utils::getCurrentDateTime();
		Logger::log("Wakeup camera thread : %d : %s", cameralocalnumber, date.c_str());

		ParseCommand(cameralocalnumber);
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

			Utils::Sleep(2);

			gpio[cameralocalnumber]->setValue(LOW);
			cameracontrol[cameralocalnumber]->ReleaseButton();


			string name = Utils::format_string("%s-%d.%s", global_machine_name.c_str(), cameralocalnumber,
				global_capturefile_ext.c_str());
			cameracontrol[cameralocalnumber]->GetFilefromCamera(name.c_str());

			Utils::Sleep(1);

			// Upload to FTP
			StartUpload(cameralocalnumber);
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
		else if (command.buffer[0] == PACKET_AUTOFOCUS)
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

bool CameraThread::StartUpload(int cameraNum)
{
	upload_progress[cameraNum] = 0;

	CURL* curl;
	CURLcode res;
	//struct WriteThis upload;
	string name = Utils::format_string("%s-%d.%s", global_machine_name.c_str(), cameraNum, global_capturefile_ext.c_str());

	Logger::log(cameraNum, "--------------------------------------------------------");
	Logger::log(cameraNum, "Start Upload FTP : %s", name.c_str());
	Logger::log(cameraNum, "--------------------------------------------------------");

	char* inbuf = NULL;
	int len = 0;

	FILE* fp = NULL;
	fp = fopen(name.c_str(), "rb");
	if (fp == NULL)
	{
		Logger::log(cameraNum, "%s file not found.", name.c_str());
		return false;
	}

	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	inbuf = new char[len];
	fread(inbuf, 1, len, fp);
	fclose(fp);

	upload[cameraNum].camnum = cameraNum;
	upload[cameraNum].readptr = inbuf;
	upload[cameraNum].totalsize = len;
	upload[cameraNum].sizeleft = len;

	Logger::log(cameraNum, "filesize : %d", len);


	curl = curl_easy_init();
	if (curl)
	{
		string url = "ftp://" + global_server_address + "/" + global_ftp_path + "/" + name;
		string ftpstr = global_ftp_id + ":" + global_ftp_passwd;

		Logger::log("FTP full path : %s (%s)", url.c_str(), ftpstr.c_str());

		//string url = "ftp://192.168.29.103/" + name;
		//curl_easy_setopt(curl, CURLOPT_URL, "ftp://192.168.29.103/2.jpg");
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_USERPWD, ftpstr.c_str());
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
		curl_easy_setopt(curl, CURLOPT_READDATA, &upload[cameraNum]);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);
		curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)upload[cameraNum].sizeleft);

		// 전송!
		res = curl_easy_perform(curl);

		if (res != CURLE_OK)
		{
			Logger::log(cameraNum, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}
		curl_easy_cleanup(curl);
	}

	// send finish packet
	char buf[TCP_BUFFER] = { 0, };
	buf[0] = PACKET_UPLOAD_DONE;
	buf[1] = cameraNum;
	NetworkThread::getInstance()->Send(buf);

	Logger::log(cameraNum, "Upload complete");

	// 끝
	delete[] inbuf;
	return true;
}

size_t CameraThread::read_callback(void* ptr, size_t size, size_t nmemb, void* userp)
{
	struct WriteThis* upload = (struct WriteThis*)userp;
	size_t max = size * nmemb;

	if (max < 1)
		return 0;

	if (upload->sizeleft)
	{
		size_t copylen = max;
		if (copylen > upload->sizeleft)
			copylen = upload->sizeleft;
		memcpy(ptr, upload->readptr, copylen);
		upload->readptr += copylen;
		upload->sizeleft -= copylen;

		int progress = (int)(((float)(upload->totalsize - upload->sizeleft) / upload->totalsize) * 100.f);
		int p = (progress / 10);

		if (upload_progress[upload->camnum] != p)
		{
			upload_progress[upload->camnum] = p;

			Logger::log(upload->camnum, "upload_progress %d", upload_progress[upload->camnum]);

			//char data[32];
			//memcpy(data, &upload_progress[upload->camnum], sizeof(int));
			//network[upload->camnum].write(PACKET_UPLOAD_PROGRESS, data, 32);
			//network[upload->camnum].update();

			char buf[TCP_BUFFER] = { 0, };
			if (p == 10)
			{
				buf[0] = PACKET_UPLOAD_PROGRESS;
				buf[1] = (char)upload->camnum;
				buf[2] = (char)upload_progress[upload->camnum];
			}
			else
			{
				buf[0] = PACKET_UPLOAD_PROGRESS;
				buf[1] = (char)upload->camnum;
				buf[2] = (char)upload_progress[upload->camnum];
			}

			// progress send to server
			NetworkThread::getInstance()->Send(buf);
		}
		return copylen;
	}

	return 0;                          /* no more data left to deliver */
}

