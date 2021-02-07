#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <istream>

#include "csvread.h"

cvsread* cvsread::_instance = nullptr;

cvsread::cvsread()
{
}

cvsread::~cvsread()
{
}

/*
int main(int argc, char* argv[])
{
	std::ifstream in("input.csv");
	if (in.fail()) return (cout << "File not found" << endl) && 0;
	while (in.good())
	{
		std::vector<std::string> row = csv_read_row(in, ',');
		for (int i = 0, leng = row.size(); i < leng; i++)
			cout << "[" << row[i] << "]" << "\t";
		cout << endl;
	}
	in.close();

	std::string line;
	in.open("input.csv");
	while (getline(in, line) && in.good())
	{
		std::vector<std::string> row = csv_read_row(line, ',');
		for (int i = 0, leng = row.size(); i < leng; i++)
			cout << "[" << row[i] << "]" << "\t";
		cout << endl;
	}
	in.close();

	return 0;
}
*/

/*

std::vector<std::string> cvsread::csv_read_row(std::string& line, char delimiter)
{
	std::stringstream ss(line);
	return csv_read_row(ss, delimiter);
}

std::vector<std::string> cvsread::csv_read_row(std::istream& in, char delimiter)
{
	std::stringstream ss;
	bool inquotes = false;
	std::vector<std::string> row;//relying on RVO
	while (in.good())
	{
		char c = in.get();
		if (!inquotes && c == '"') //beginquotechar
		{
			inquotes = true;
		}
		else if (inquotes && c == '"') //quotechar
		{
			if (in.peek() == '"')//2 consecutive quotes resolve to 1
			{
				ss << (char)in.get();
			}
			else //endquotechar
			{
				inquotes = false;
			}
		}
		else if (!inquotes && c == delimiter) //end of field
		{
			row.push_back(ss.str());
			ss.str("");
		}
		else if (!inquotes && (c == '\r' || c == '\n'))
		{
			if (in.peek() == '\n') { in.get(); }
			row.push_back(ss.str());
			return row;
		}
		else
		{
			ss << c;
		}
	}
}
*/

std::vector<std::string> cvsread::csv_read_row(std::string& in, char delimiter)
{
	return split(in, delimiter);
}

std::vector<std::string> cvsread::split(std::string str, char delimiter)
{
	uint64_t start_pos = 0;
	uint64_t search_pos = 0;
	std::vector<std::string> result;

	while (start_pos < str.size())
	{
		search_pos = str.find_first_of(delimiter, start_pos);
		//printf("%lld\n", search_pos);
		std::string tmp_str;

		if (search_pos == std::string::npos)
		{
			// for last token
			search_pos = str.size();
			tmp_str = str.substr(start_pos, search_pos - start_pos);
			result.push_back(tmp_str);
			break;
		}
		tmp_str = str.substr(start_pos, search_pos - start_pos);
		result.push_back(tmp_str);
		start_pos = search_pos + 1;
	}

	return result;
}
