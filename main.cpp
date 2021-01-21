#include "predef.h"
#include "config.h"
#include "networkthread.h"
#include "tcpsocket.h"
//#include "udpsocket.h"
#include "camerathread.h"


///////////////////////////////////////////////////////////////////////////////////////////// 

void RecieveServerInfo()
{
	int sock;
	struct sockaddr_in broadcastAddr;
	unsigned short broadcastPort;
	char recvString[UDP_BUFFER] = { 0, };
	int recvStringLen;

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
		server_address = string(vbuff);
		Logger::log("Recv server address : %s", server_address.c_str());
	}

	close(sock);
}


int main(void)
{
	// Load config.txt
	Config::getInstance()->Initialize();

	// Recieve Server Address
	RecieveServerInfo();

	// TODO. Check Camera

	// TODO. Start Camera Thread
	CameraThread cameraThread;
	cameraThread.Initialize(3);	//  (카메라 만큼)

	// TODO. Start TCP Thread
	NetworkThread network;
	network.Initialize();



	while (true)
	{
		Utils::Sleep(1);
	}

	cameraThread.Wait();
	network.Wait();

	return 0;
}