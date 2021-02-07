#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <istream>

using std::cout;
using std::endl;

class cvsread
{
public :
	static cvsread* getInstance()
	{
		if (_instance == nullptr)
			_instance = new cvsread();
		return _instance;
	}

	//std::vector<std::string> csv_read_row(std::istream& in, char delimiter);
	//std::vector<std::string> csv_read_row(std::string& in, char delimiter);
	std::vector<std::string> csv_read_row(std::string& in, char delimiter);

private :
	cvsread();
	~cvsread();
	std::vector<std::string> split(std::string str, char delimiter);

	static cvsread* _instance;
};