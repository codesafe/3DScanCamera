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
#include "gpio/gpio.h"


using namespace gphoto2pp;
using namespace std;
using namespace exploringRPi;

#define _ENABLE_GPIO

//////////////////////////////////////////////////////////////////////////

enum CAMERA_MANUFACTURER
{
	CAMERA_UNKNOWN,
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

struct CameraInfo
{
	string	modelname;
	string	port;
	CAMERA_MANUFACTURER manufacture;
};

extern std::vector<CameraInfo> global_Camerainfo;

//////////////////////////////////////////////////////////////////////////

#define CAPTURE_TO_RAM			0 // "Internal RAM"
#define CAPTURE_TO_SDCARD		1 // "Memory card"

// 카메라 최대 10개
#define MAX_CAMERA	10

//////////////////////////////////////////////////////////////////////////


#define SERVER_TCP_PORT					8888
#define SERVER_UDP_PORT					11000
#define SERVER_UDP_BROADCASTPORT		9999

#define TCP_BUFFER			32
#define UDP_BUFFER			32

//////////////////////////////////////////////////////////////////////////	network packet

// slave / master 라즈베리
#define RASP_SLAVE		0
#define RASP_MASTER		1

// response packet
#define RESPONSE_OK				0x07
#define RESPONSE_FAIL			0x08

// Log packet
#define CLIENT_LOG_INFO		0x0a
#define CLIENT_LOG_WARN		0x0b
#define CLIENT_LOG_ERR		0x0c


// Packet
#define	PACKET_MACHINE_NUMBER	0x07	// 머신 번호 ( Server -> Rasp )
#define	PACKET_MACHINE_INFO		0x08	// 머신 정보
#define PACKET_CAMERA_NAME		0x09
#define	PACKET_SHOT				0x10	// shot picture
#define PACKET_AUTOFOCUS		0x20	// auto focus
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
	ISO = 0,
	SHUTTERSPEED,
	APERTURE,
	CAPTURE_FORMAT,
	PARAM_MAX
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

extern string global_iso;
extern string global_aperture;
extern string global_shutterspeed;
extern string global_captureformat;

extern string global_apertureString[];
extern string global_isoString[];
extern string global_shutterspeedString[];
extern string global_captureformatString[];

// read from local config.txt
extern bool global_ismaster;
extern bool global_recieved_serveraddress;
extern string global_server_address;
extern string global_machine_name;
extern string global_capturefile_ext;			// 캡쳐파일 확장자

// recv from server
extern int global_raspmachine_id;
extern string global_ftp_path;
extern string global_camera_id;
extern string global_ftp_id;
extern string global_ftp_passwd;

#define MASTERCONTROL_GPIO		26 // GPIO 26은 마스터용
extern int global_CAMERA_GPIO[];



#endif
