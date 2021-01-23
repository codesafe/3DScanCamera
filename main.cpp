#include "predef.h"
#include "config.h"
#include "networkthread.h"
#include "camerathread.h"
#include "tcpsocket.h"
#include "mastercontrol.h"
#include "utils.h"

///////////////////////////////////////////////////////////////////////////////////////////// 

void RecieveServerInfo()
{
	int sock;
	struct sockaddr_in broadcastAddr;
	unsigned short broadcastPort;
	char recvString[UDP_BUFFER] = { 0, };
	int recvStringLen;

	Logger::log("Start Recieve Broadcasted server address..");

	broadcastPort = SERVER_UDP_BROADCASTPORT;

	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		Logger::log("socket() failed");
		return;
	}

	memset(&broadcastAddr, 0, sizeof(broadcastAddr));
	broadcastAddr.sin_family = AF_INET;
	broadcastAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	broadcastAddr.sin_port = htons(broadcastPort);


	static int reuseFlag = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuseFlag, sizeof reuseFlag) != 0)
	{
		Logger::log("[%s] RecieveServerInfo setsockopt(SO_REUSEADDR) error: ", __FUNCTION__);
		close(sock);
		return;
	}

	if (bind(sock, (struct sockaddr*)&broadcastAddr, sizeof(broadcastAddr)) < 0)
	{
		Logger::log("RecieveServerInfo bind() failed");
		return;
	}

	if ((recvStringLen = recvfrom(sock, recvString, UDP_BUFFER, 0, NULL, 0)) < 0)
	{
		Logger::log("RecieveServerInfo recvfrom() failed");
		return;
	}
	//recvString[recvStringLen] = '\0';

	char vbuff[UDP_BUFFER] = { 0, };
	sprintf(vbuff, "%d.%d.%d.%d", (unsigned char)recvString[1], (unsigned char)recvString[2],
		(unsigned char)recvString[3], (unsigned char)recvString[4]);

	//Logger::log("Received: %d : %s\n", recvString[0], vbuff);

	if (recvString[0] == UDP_BROADCAST_PACKET)
	{
		global_server_address = string(vbuff);
		Logger::log("Recv server address : %s", global_server_address.c_str());
	}

	close(sock);
}

// 카메라 검색후 등록
int EnumCameraList()
{
	global_Camerainfo.clear();
	auto cameraList = gphoto2pp::autoDetectAll();
	for (int i = 0; i < cameraList.count(); ++i)
	{
		CameraInfo camerainfo;
		camerainfo.modelname = cameraList.getName(i);
		camerainfo.port = cameraList.getValue(i);
		camerainfo.manufacture = Utils::getManufacture(camerainfo.modelname);

		Logger::log("Detect Camera %d : %s : %s", i, camerainfo.modelname.c_str(), camerainfo.port.c_str());
		global_Camerainfo.push_back(camerainfo);
	}

	return cameraList.count();
}


void DisplayVersion()
{
	string gphoto2version = gphoto2pp::LibraryVersion(true);
	Logger::log("Gphoto2 %s ", gphoto2version.c_str());
}


int main(void)
{
	DisplayVersion();

	// Load config.txt
	Config::getInstance()->Initialize();

	global_ismaster = Config::getInstance()->GetBool("MASTER");
	global_server_address = Config::getInstance()->GetString("SERVERIP");
	global_machine_name = Config::getInstance()->GetString("NAME");
	global_ftp_id = Config::getInstance()->GetString("FTP_ID");
	global_ftp_passwd = Config::getInstance()->GetString("FTP_PASSWD");

	// Check All Cameras
	int cameranum = EnumCameraList();
// 	if (cameranum == 0)
// 	{
// 		Logger::log("No Camera Detected. Exit.");
// 		return -1;
// 	}

	// Recieve Server Address
	//RecieveServerInfo(); 

	// Start Camera Thread
	//int cameranum = 3;

	CameraThread cameraThread;
	cameraThread.Initialize();	//  (카메라 만큼)

	// Start TCP Thread
	NetworkThread::getInstance()->Initialize();

	// Master GPIO Listener
	if(global_ismaster == false)
	{
		MasterControl mastercontrol;
		mastercontrol.Initialize();
	}

	while (true)
	{
		Utils::Sleep(1);
	}

	cameraThread.Wait();
	NetworkThread::getInstance()->Wait();

	return 0;
}