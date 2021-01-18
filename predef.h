#ifndef _PREDEF_
#define _PREDEF_

#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wmisleading-indentation"


#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/usbdevice_fs.h>

#include <stdlib.h>
#include <stdio.h>
#include <utime.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>

#include <memory>
#include <string>
#include <ctime>
#include <deque>
#include <vector>

//////////////////////////////////////////////////////////////////////////

#include <gphoto2/gphoto2-camera.h>
#include <gphoto2/gphoto2-abilities-list.h>
#include <gphoto2/gphoto2-context.h>
#include <gphoto2/gphoto2-port-log.h>
#include <gphoto2/gphoto2-setting.h>
#include <gphoto2/gphoto2-filesys.h>
#include <gphoto2/gphoto2-port-result.h>

//////////////////////////////////////////////////////////////////////////

#include <gphoto2pp/helper_camera_wrapper.hpp>
#include <gphoto2pp/camera_wrapper.hpp>
#include <gphoto2pp/camera_file_wrapper.hpp>
#include <gphoto2pp/camera_capture_type_wrapper.hpp>
#include <gphoto2pp/exceptions.hpp>
#include <gphoto2pp/helper_gphoto2.hpp>
#include <gphoto2pp/camera_list_wrapper.hpp>

//Widget types
#include <gphoto2pp/window_widget.hpp>
#include <gphoto2pp/toggle_widget.hpp>
#include <gphoto2pp/date_widget.hpp>
#include <gphoto2pp/range_widget.hpp>
#include <gphoto2pp/menu_widget.hpp>
#include <gphoto2pp/radio_widget.hpp>
#include <gphoto2pp/text_widget.hpp>

#include <gphoto2pp/camera_file_wrapper.hpp>
#include <gphoto2pp/camera_file_type_wrapper.hpp>
#include <gphoto2pp/camera_file_path_wrapper.hpp>
#include <gphoto2pp/camera_event_type_wrapper.hpp>
#include <gphoto2pp/log.h>

#include "logger.h"
#include "utils.h"
#include "gpio/gpio.h"


//#include "tcpsocket.h"
//#include "udpsocket.h"

using namespace gphoto2pp;
using namespace std;
using namespace exploringRPi;


//////////////////////////////////////////////////////////////////////////

enum CAMERA_MANUFACTURER
{
	CAMERA_CANON,
	CAMERA_NIKON,
};


typedef struct _GPParams GPParams;
struct _GPParams
{
	std::string name;
	Camera* camera;
	GPContext* context;
	GPPortInfoList* portinfo_list;
	CameraAbilitiesList* _abilities_list;

	_GPParams()
	{
		camera = NULL;
		context = NULL;
		portinfo_list = NULL;
		_abilities_list = NULL;
	}
};

enum wait_type
{
	WAIT_TIME,
	WAIT_EVENTS,
	WAIT_FRAMES,
	WAIT_STRING,
};

enum download_type { DT_NO_DOWNLOAD, DT_DOWNLOAD };
struct waitparams
{
	union
	{
		int milliseconds;
		int events;
		int frames;
		char* str;
	} u;
	enum wait_type type;
	enum download_type downloadtype;
};

//////////////////////////////////////////////////////////////////////////

#include <curl/curl.h>

struct WriteThis
{
	int camnum;
	char* readptr;
	size_t totalsize;
	size_t sizeleft;

	WriteThis()
	{
		camnum = -1;
		readptr = NULL;
		totalsize = 0;
		sizeleft = 0;
	}
};


//////////////////////////////////////////////////////////////////////////

#define CAPTURE_TO_RAM			0 // "Internal RAM"
#define CAPTURE_TO_SDCARD		1 // "Memory card"


// ī�޶� �ִ� 10��
#define MAX_CAMERA	10

// ī�޶� ����
enum CAMERA_STATE
{
	STATE_NONE,
	STATE_STARTCONNECT,
	STATE_CONNECTION,
	STATE_CONNECT_ERROR,
	STATE_READY,
	STATE_FOCUSING,
	STATE_SHOT,
	STATE_UPLOAD,
	STATE_UPLOADING,
};



//////////////////////////////////////////////////////////////////////////


#define SERVER_TCP_PORT					8888
#define SERVER_UDP_PORT					11000
#define SERVER_UDP_BROADCASTPORT		9999
#define SOCKET_BUFFER		4096
#define TCP_BUFFER			32
#define UDP_BUFFER			32

//////////////////////////////////////////////////////////////////////////	network packet

// response packet

#define RESPONSE_OK				0x07
#define RESPONSE_FAIL			0x08

// Log packet
#define CLIENT_LOG_INFO		0x0a
#define CLIENT_LOG_WARN		0x0b
#define CLIENT_LOG_ERR		0x0c


// Packet

#define	PACKET_MACHINE_INFO		0x08	// �ӽ� ����
#define PACKET_CAMERA_NAME		0x09
#define	PACKET_SHOT				0x10	// shot picture
#define PACKET_HALFPRESS		0x20	// auto focus
#define PACKET_HALFRELEASE		0x21	// auto focus cancel

#define PACKET_SET_PARAMETER	0x30

#define PACKET_FORCE_UPLOAD		0x40	// for test
#define PACKET_UPLOAD_PROGRESS	0x41
#define PACKET_UPLOAD_DONE		0x42

#define PACKET_SETPARAMETER_RESULT	0x50
#define PACKET_AUTOFOCUS_RESULT		0x51
#define PACKET_SHOT_RESULT			0x52

#define UDP_BROADCAST_PACKET	0xa0

//////////////////////////////////////////////////////////////////////////

enum CAMERA_PARAM
{
	ISO,
	SHUTTERSPEED,
	APERTURE,
	CAPTURE_FORMAT
};

#define ISO_VALUE				3	// 400
#define SHUTTERSPEED_VALUE		36	// 1 / 100
//#define SHUTTERSPEED_VALUE	46	// 1/1000
#define APERTURE_VALUE			5	// 9
#define CAPTURE_FORMAT_VALUE	0

/*

#define ISO_VALUE				"400"
#define SHUTTERSPEED_VALUE		"1/100"
//#define SHUTTERSPEED_VALUE	46	// 1/1000
#define APERTURE_VALUE			"9"
*/

extern string iso;
extern string aperture;
extern string shutterspeed;
extern string captureformat;

extern string apertureString[];
extern string isoString[];
extern string shutterspeedString[];
extern string captureformatString[];

// read from local config.txt
extern bool ismaster;
extern bool recieved_serveraddress;
extern string server_address;
extern string machine_name;
extern string capturefile_ext;			// ĸ������ Ȯ����

// recv from server
extern string ftp_path;
extern string camera_id;
extern string ftp_id;
extern string ftp_passwd;

extern class TCP_Socket tcp_socket;
//extern class UDP_Socket udp_socket;

#define MASTERCONTROL_GPIO		26 // GPIO 26�� �����Ϳ�
extern int CAMERA_GPIO[];



#endif
