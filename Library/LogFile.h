/*	log.h -- Online Pinochle Game -- Lucas Weisensee November 2014


*/
#pragma once
#include "string"
#include "stdio.h"
#include <iostream>
#include <iomanip>
#include <ctime>

class LogFile {
public:
	LogFile();
	LogFile(char * type, char * directory);
	LogFile(char * type);
	void setLogFileName();									// Sets LOG_FILE_NAME with date and PID info
	void setLogFileName(char * type, char * directory);		// Sets LOG_FILE_NAME with date, PID info and given type in directory specified
	void setLogFileName(char * type);						// Sets LOG_FILE_NAME with date, PID info and given type

	void writetolog(char * report);							//WRAPPER::writes report to log file
	void writetolog(char * report, char * error);			//WRAPPER::writes report to log file with error appened
	void writetolog(std::string * report);					//writes report to log file
	void writetolog(char * report, int error);				//WRAPPER::writes error report to log file
	void writetolog(std::string * report, int error);				//WRAPPER::writes error report to log file
	std::string* addTimeString(std::string * report);

private:
	std::wstring* LOG_FILE_NAME;			// Log file's name
	std::string DEFAULT_LOCATION;
};