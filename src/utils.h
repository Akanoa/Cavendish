#include <sstream>
#include <string>
#include <iomanip>
#include <time.h>
#include <stdio.h>
#include <cmath>

#define NB_DIGIT 11

void currentDate(std::string &date, std::string &hour);
std::string getFileName(std::string &strPath);

template<typename T>
std::string num2string(T x)
{
	std::string s;
	std::stringstream ss;
	ss << x;
	s = ss.str();
	return s;
}


std::string float2scientific(float d, int nb_digit=NB_DIGIT);
