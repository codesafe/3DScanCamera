#include "predef.h"


//==============================================================================================================================


// 사용가능한 GPIO번호
int global_CAMERA_GPIO[] = { 17, 18, 27, 22, 23, 24, 25, 5, 6, 12, 13 };


// Aperture
string global_apertureString[] = { "5", "5.6", "6.3", "7.1", "8", "9", "10", "11", "13", "14",  "16", "18", "20", "22", "25", "29", "32" };

// ISO
string global_isoString[] = { "Auto", "100", "200", "400", "800", "1600", "3200", "6400" };

// Shutter Speed
string global_shutterspeedString[] = {	
	"bulb", "30", "25", "20", "15", "13", "10", 
	"8", "6", "5", "4", "3.2", "2.5", "2", "1.6", 
	"1.3", "1", "0.8", "0.6", "0.5", "0.4", "0.3", 
	"1/4", "1/5", "1/6", "1/8", "1/10", "1/13", "1/15", 
	"1/20", "1/25", "1/30", "1/40", "1/50", "1/60", 
	"1/80", "1/100", "1/125", "1/160", "1/200", "1/250",
	"1/320", "1/400", "1/500", "1/640", "1/800", "1/1000",
	"1/1250", "1/1600", "1/2000", "1/2500", "1/3200", "1/4000" 
};

// Capture format
string global_captureformatString[] = { 
	"Large Fine JPEG", 
	"Large Normal JPEG", 
	"Medium Fine JPEG", 
	"Medium Normal JPEG", 
	"Small Fine JPEG",
	"Small Normal JPEG", 
	"Smaller JPEG", 
	"Tiny JPEG", 
	"RAW + Large Fine JPEG", 
	"RAW" 
};


//==============================================================================================================================


string global_iso = global_isoString[ISO_VALUE];
string global_aperture = global_apertureString[APERTURE_VALUE];
string global_shutterspeed = global_shutterspeedString[SHUTTERSPEED_VALUE];
string global_captureformat = global_captureformatString[CAPTURE_FORMAT_VALUE];

bool global_recieved_serveraddress = false;
string global_server_address = "";// SERVER_ADD;
string global_machine_name = "";
string global_capturefile_ext = "jpg";

int global_raspmachine_id = -1;
string global_ftp_path = "";
string global_ftp_id = "";
string global_ftp_passwd = "";
string global_camera_id = "";

bool global_ismaster = false;

std::vector<CameraInfo> global_Camerainfo;
