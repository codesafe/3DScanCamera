#include "predef.h"
#include "csvread.h"
#include "utils.h"

//==============================================================================================================================

// 카메라 커맨드 : CANON, NIKON 순서
// string global_cmd_iso[] = { "ISO Speed", "ISO Speed" };
// string global_cmd_aperture[] = { "Aperture", "F-Number" };
// string global_cmd_shutterspeed[] = { "Shutter Speed", "Shutter Speed" };
// string global_cmd_imageformat[] = { "Image Format", "Image Quality" };

// 사용가능한 GPIO번호
int global_CAMERA_GPIO[] = { 17, 18, 27, 23, 24, 25, 5, 6, 12, 13, 22 };

map<string, vector<string>> global_parameter_iso;
map<string, vector<string>> global_parameter_aperture;
map<string, vector<string>> global_parameter_shutterspeed;
map<string, vector<string>> global_parameter_captureformat;

map<string, string> global_parameter_iso_command;
map<string, string> global_parameter_aperture_command;
map<string, string> global_parameter_shutterspeed_command;
map<string, string> global_parameter_captureformat_command;

// 카메라 파라메터 읽기
bool ReadCameraParameter(string path, string key)
{
#if 1
	FILE* fp = fopen(path.c_str(), "rt");
	if (fp == NULL)
		return (cout << "File not found" << endl) && 0;

	int count = 0;
	string keys[3];
	string commands[3];
	std::vector<string> params[3];

	while (!feof(fp))
	{
		char keybuffer[256] = { 0, };
		fgets(keybuffer, sizeof(keybuffer), fp);
		Utils::clearString(keybuffer, 256);
		
		string keystr(keybuffer);
		std::vector<std::string> row = cvsread::getInstance()->csv_read_row(keystr, ',');
		if (row.empty()) break;

		if (count == 0)
		{
			keys[0] = row[0];
			keys[1] = row[1];
			keys[2] = row[2];
		}
		else if (count == 1)
		{
			commands[0] = row[0];
			commands[1] = row[1];
			commands[2] = row[2];
		}
		else
		{
			// x 가 모두 아니면 쓸수있는 파라메터
			if (row[0] != "x" && row[1] != "x" && row[2] != "x")
			{
				params[0].push_back(row[0]);
				params[1].push_back(row[1]);
				params[2].push_back(row[2]);
			}
		}

		for (int i = 0, leng = row.size(); i < leng; i++)
			cout << "[" << row[i] << "]" << "\t";
		cout << endl;
		count++;
	}
	fclose(fp);

#else
	std::ifstream in(path);
	if (in.fail()) 
		return (cout << "File not found" << endl) && 0;

	int count = 0;

	string keys[3];
	string commands[3];
	std::vector<string> params[3];

	while (in.good())
	{
		std::vector<std::string> row = cvsread::getInstance()->csv_read_row(in, ',');
		if( row.empty() ) break;

		if (count == 0)
		{
			keys[0] = row[0];
			keys[1] = row[1];
			keys[2] = row[2];
		}
		else if (count == 1)
		{
			commands[0] = row[0];
			commands[1] = row[1];
			commands[2] = row[2];
		}
		else
		{
			// x 가 모두 아니면 쓸수있는 파라메터
			if (row[0] != "x" && row[1] != "x" && row[2] != "x")
			{
				params[0].push_back(row[0]);
				params[1].push_back(row[1]);
				params[2].push_back(row[2]);
			}
		}

		for (int i = 0, leng = row.size(); i < leng; i++)
			cout << "[" << row[i] << "]" << "\t";
		cout << endl;
		count++;
	}
	in.close();
#endif
	if (key == "iso")
	{
		global_parameter_iso.clear();
		global_parameter_iso.insert(std::make_pair(keys[0], params[0]));
		global_parameter_iso.insert(std::make_pair(keys[1], params[1]));
		global_parameter_iso.insert(std::make_pair(keys[2], params[2]));

		global_parameter_iso_command.clear();
		global_parameter_iso_command.insert(std::make_pair(keys[0], commands[0]));
		global_parameter_iso_command.insert(std::make_pair(keys[1], commands[1]));
		global_parameter_iso_command.insert(std::make_pair(keys[2], commands[2]));
	}
	else if (key == "aperture")
	{
		global_parameter_aperture.clear();
		global_parameter_aperture.insert(std::make_pair(keys[0], params[0]));
		global_parameter_aperture.insert(std::make_pair(keys[1], params[1]));
		global_parameter_aperture.insert(std::make_pair(keys[2], params[2]));

		global_parameter_aperture_command.clear();
		global_parameter_aperture_command.insert(std::make_pair(keys[0], commands[0]));
		global_parameter_aperture_command.insert(std::make_pair(keys[1], commands[1]));
		global_parameter_aperture_command.insert(std::make_pair(keys[2], commands[2]));
	}
	else if (key == "shutterspeed")
	{
		global_parameter_shutterspeed.clear();
		global_parameter_shutterspeed.insert(std::make_pair(keys[0], params[0]));
		global_parameter_shutterspeed.insert(std::make_pair(keys[1], params[1]));
		global_parameter_shutterspeed.insert(std::make_pair(keys[2], params[2]));

		global_parameter_shutterspeed_command.clear();
		global_parameter_shutterspeed_command.insert(std::make_pair(keys[0], commands[0]));
		global_parameter_shutterspeed_command.insert(std::make_pair(keys[1], commands[1]));
		global_parameter_shutterspeed_command.insert(std::make_pair(keys[2], commands[2]));
	}
	else if (key == "imageformat")
	{
		global_parameter_captureformat.clear();
		global_parameter_captureformat.insert(std::make_pair(keys[0], params[0]));
		global_parameter_captureformat.insert(std::make_pair(keys[1], params[1]));
		global_parameter_captureformat.insert(std::make_pair(keys[2], params[2]));

		global_parameter_captureformat_command.clear();
		global_parameter_captureformat_command.insert(std::make_pair(keys[0], commands[0]));
		global_parameter_captureformat_command.insert(std::make_pair(keys[1], commands[1]));
		global_parameter_captureformat_command.insert(std::make_pair(keys[2], commands[2]));
	}

	return true;
}

//==============================================================================================================================

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

