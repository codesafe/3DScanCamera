#include "networkthread.h"
#include "command.h"
#include "tcpsocket.h"
#include "camerathread.h"
#include "utils.h"

NetworkThread* NetworkThread::_instance = nullptr;

NetworkThread::NetworkThread()
{
	tcp_socket = new TCP_Socket();
	ResetBuffer();
}

NetworkThread::~NetworkThread()
{
	delete tcp_socket;
	tcp_socket = nullptr;
}

bool NetworkThread::Initialize()
{
	if(tcp_socket->init() == false) return false;
	if(tcp_socket->connect() == false) return false;
	
// 	// Master / Slave 정보 보냄
// 	char buffer[TCP_BUFFER] = { 0 };
// 	buffer[0] = PACKET_MACHINE_INFO;
// 	buffer[1] = global_ismaster ? RASP_MASTER : RASP_SLAVE;
// 	tcp_socket->send(buffer);

	_thread = thread(&NetworkThread::WorkThread, this);

	return true;
}


void NetworkThread::WorkThread()
{
	bool loop = true;
	while (loop)
	{
		Update();
	}
}

void NetworkThread::Update()
{
	lock_guard<mutex> lt(_lock);

	tcp_socket->update();
	//tcp_socket->recv(buffer);

	// 받은것이 있으면 처리
	if (tcp_socket->getrecvpacket(buffer))
		ParseCommand();
}

void NetworkThread::Wait()
{
	Logger::log("## NetworkThread Joined");
	_thread.join();
}

void NetworkThread::ParseCommand()
{
	// parse recv buffer
	bool sendcommand = false;
	_Command command;
	char packet = buffer[0];
	switch (packet)
	{
		// Server에서 RASP 머신에게 번호를 주었다
		case PACKET_MACHINE_NUMBER:
			{
				global_raspmachine_id = (int)buffer[1];
				int cameraN = global_Camerainfo.size();

				// slave/master, 카메라댓수, 머신이름을 전송 
				char buf[TCP_BUFFER] = { 0, };
				buf[0] = PACKET_MACHINE_INFO;
				buf[1] = global_ismaster ? RASP_MASTER : RASP_SLAVE;
				buf[2] = (char)cameraN;
				strcpy(buf+3, global_machine_name.c_str());
				tcp_socket->send(buf);

				// 카메라 이름 전송
				for (int i = 0; i < cameraN; i++)
				{
					string name = global_Camerainfo[i].modelname;
					buf[0] = PACKET_CAMERA_NAME;
					buf[1] = i;
					strcpy(buf + 2, name.c_str());
					tcp_socket->send(buf);
				}
			}
			break;

		case PACKET_AUTOFOCUS:			// 카메라 자동 포커스
		case PACKET_SET_PARAMETER:		// 카메라 옵션 변경
			{
				command.type = CommandType::COMMAND_NETWORK;
				memcpy(command.buffer, buffer, TCP_BUFFER);
				sendcommand = true;
			}
			break;

		case PACKET_AUTOFOCUS_TOGGLE:	// 카메라 자동 포커스 토글
			{
				// dummy thread로
				Logger::log("Packet autofocus toggle");
				command.type = CommandType::COMMAND_NETWORK;
				memcpy(command.buffer, buffer, TCP_BUFFER);
				sendcommand = true;
			}
			break;

		case PACKET_UPLOAD_PATH :
			{
				global_ftp_path = (char*)(buffer + 1);
				Logger::log("Get Upload Path : %s", global_ftp_path.c_str());
			}
			break;

		// Master 머신에게 찍으라 명령
		case PACKET_SHOT :
			{
				if (global_ismaster == true)
				{
					// 마스터이기 때문에 현재 머신의 GPIO 모두 셋
					command.type = CommandType::COMMAND_GPIO;
					memcpy(command.buffer, buffer, TCP_BUFFER);
					sendcommand = true;
				}
				else
				{
					// Slave인데 쏘라 했다
					Logger::log("Error PACKET_SHOT on Slave");
				}
			}
			break;
	}

	// Add command --> wakeup camera thread
	if( sendcommand )
	{
		std::string  date = Utils::getCurrentDateTime();
		Logger::log("notify_all --> %s", date.c_str());

		CommandQueue::getInstance()->AddCommand(command);
		for (int i=0; i<MAX_CAMERA; i++)
		{
			CameraThread::wakeupEvent[i].notify_one();
		}

	}

	ResetBuffer();
}

void NetworkThread::ResetBuffer()
{
	memset(buffer, 0, TCP_BUFFER);
}

void NetworkThread::Send(char* buf)
{
	lock_guard<mutex> lt(_lock);
	tcp_socket->addsendpacket(buf);

}
