#pragma once

#include <iostream>
#include <string>
#include <sstream> 
#include <vector>

class Util
{
public:
    Util(void);
    ~Util(void);
	static int low_pass(const int previous_output, const int input);

	static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
    static std::vector<std::string> split(const std::string &s, char delim);

	static bool file_exists(const std::string& name)
	{
		struct stat buffer;
		return (stat(name.c_str(), &buffer) == 0);
	}
};
