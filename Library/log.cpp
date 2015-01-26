/*	log.cpp -- Online Pinochle Game -- Lucas Weisensee November 2014

	functions for the log writing structure

*/
#include "log.h"

void writetolog(std::string * treport)					//writes report to log file
{
	//generate report
	std::string * report = addTimeString(treport);

	//open log file
	FILE * logFile = fopen(LOG_FILE_NAME, "a");

	//write string to file
	fprintf(logFile, "%s\n", report);

	//close log file
	fclose(logFile);
	delete treport;		//deallocate report string
}

void writetolog(char * report)							//WRAPPER::writes report to log file
{
	std::string tempstr = report;
	writetolog(&tempstr);
}
	

// Adds error to report and sends to be written to logfile
void writetolog(char * report, int error)		//writes errror report to log file
{
	// Concatenate to make new report
	std::string catReport = report;
	catReport += error;

	// Send new report
	writetolog(&catReport);						//send concatenated report
}

// Appends system time to string for log/error reporting
// report_string dayofweek SystemTime: MM/DD/YYYY HH:MM:SS
std::string* addTimeString(std::string * treport)
{	


	/*
	// Initate variables
	char * temp0[8];
	for (int i = 0; i < 8; ++i)
		temp0[i] = new char [24];
	SYSTEMTIME timeN;
	LPSYSTEMTIME timeNptr = &timeN;
	GetSystemTime(timeNptr);

	// Convert time values from int to string:
	itoa(timeN.wDayOfWeek, temp0[0], 10);
	itoa(timeN.wMonth, temp0[1], 10);
	itoa(timeN.wDay, temp0[2], 10);
	itoa(timeN.wYear, temp0[3], 10);
	itoa(timeN.wHour, temp0[4], 10);
	itoa(timeN.wMinute, temp0[5], 10);
	itoa(timeN.wSecond, temp0[6], 10);
	itoa(timeN.wMilliseconds, temp0[7], 10);

	// Add strings together::
	std::string temp = " Day: " ;
	temp += temp0[0] + " SysTime: ";
	temp += temp0[1] + "/" ;
	temp += temp0[2] + "/" ;
	temp += temp0[3] + " " ;
	temp += temp0[4] + ":";
	temp += temp0[5] + ":" ;
	temp += temp0[6] + ":";
	temp += temp0[7] ;
	treport += temp;


	delete [] temp0;
	
	*/

	*treport += "[system time]";

	return treport;
}
