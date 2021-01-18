#pragma once

#include <string>
#include <map>

using namespace std;

class Config
{
private :
	Config();
	~Config();

public:
	static Config* getInstance()
	{
		if (_instance == nullptr)
			_instance = new Config();
		return _instance;
	}

	bool Initialize();
	string GetString(string key);
	bool GetBool(string key);

private:
	static Config* _instance;
	std::map<string, string>	configlist;
};
