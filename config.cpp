#include "predef.h"
#include "config.h"

Config* Config::_instance = nullptr;

Config::Config()
{
}

Config::~Config()
{
}

bool Config::Initialize()
{
	Logger::log("Load Config.");

	FILE* fp = fopen("config.txt", "rt");
	if (fp != NULL)
	{
		bool key = true;
		while (!feof(fp))
		{
			char keybuffer[32] = { 0, };
			fgets(keybuffer, sizeof(keybuffer), fp);
			Utils::clearString(keybuffer);

			string keystr(keybuffer);
			keystr.erase(std::remove(keystr.begin(), keystr.end(), '['), keystr.end());
			keystr.erase(std::remove(keystr.begin(), keystr.end(), ']'), keystr.end());

			char valuebuffer[32] = { 0, };
			fgets(valuebuffer, sizeof(valuebuffer), fp);
			Utils::clearString(valuebuffer);
			string valuestr(valuebuffer);

			configlist.insert(make_pair(keystr, valuestr));
			Logger::log("%s : %s", keystr.c_str(), valuestr.c_str());
		}

		fclose(fp);
	}

	return true;
}

string Config::GetString(string key)
{
	std::map<string, string>::iterator it = configlist.find(key);

	if (it != configlist.end())
	{
		return it->second;
	}

	return string("");
}

bool Config::GetBool(string key)
{
	std::map<string, string>::iterator it = configlist.find(key);
	if (it != configlist.end())
		return (it->second == "true") ? true : false;

	return false;
}


