#include "utils.h"

using namespace std;

void currentDate(string &date, string &hour)
{
	struct tm *timeStruct;
	time_t time_ptr;
	time(&time_ptr);
	timeStruct = localtime(&time_ptr);
	int currentMonth = timeStruct->tm_mon + 1;
	int currentDay   = timeStruct->tm_mday;
	int currentYear  = timeStruct->tm_year + 1900;
	char currentDate[30];
	char currentHour[30];
	sprintf(currentDate, "%02d/%02d/%d", currentDay, currentMonth, currentYear);
	sprintf(currentHour, "%02d/%02d/%02d", timeStruct->tm_hour, timeStruct->tm_min, timeStruct->tm_sec);

	date = currentDate;
	hour = currentHour;

}

string getFileName(string &strPath, const char *sep)
{
	string filename = strPath.substr( strPath.find_last_of(sep) + 1 );
	return filename.substr(0, filename.find_last_of("."));
}


string float2scientific(float d, int nb_digit)
{
	char buffer[30];
	double param, fractpart, intpart;

	float exposant = 0;
	float x = d;
	string signe ="+";

	//get exposant
	if(d <= 1.0)
	{
		do
		{
			x = d*pow(10, ++exposant);
		}while(x <= 1.0);

		signe = "-";
	}
	else
	{
		exposant = -1;
		do
		{
			x = d*pow(10, --exposant);
		}while(x >= 10.0);

		exposant = -exposant;
	}

	param = x;
	fractpart = modf (param , &intpart);
	ostringstream ss;
	ss <<fractpart;
	string s(ss.str());

	string dec = s.substr(s.find_last_of(".")+1);

	dec = dec.append(nb_digit-dec.size(),'0');

	sprintf(buffer, "%d.%sE%s%.02d", (int)intpart ,dec.c_str(), signe.c_str(), (int)exposant);

	return buffer;
}

istream & safegetline( istream & os, string & line ) 
{
    string myline;
    if ( getline( os, myline ) ) 
    {
       if ( myline.size() && myline[myline.size()-1] == '\r' ) 
       {
           line = myline.substr( 0, myline.size() - 1 );
       }
       else 
       {
           line = myline;
       }
    }
    return os;
}