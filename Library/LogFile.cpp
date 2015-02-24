/*	log.cpp -- Online Pinochle Game -- Lucas Weisensee November 2014

	functions for the log writing structure

*/
#include "LogFile.h"
#define DEFAULT_LOCATION "C:\\Users\\Pookey\\OneDrive\\Projects\\PinochleGame\\logs\\misc\\"	// automatic storage location if nothing else is specified
#define LOG_DIRECTORY "C:\\Users\\Pookey\\OneDrive\\Projects\\PinochleGame\\logs\\"				// directory for specific storage types

LogFile::LogFile() {
	setLogFileName();
}
LogFile::LogFile(char * type, char * directory) {
	setLogFileName(type, directory);
}
LogFile::LogFile(char * type) {
	setLogFileName(type);
}
void LogFile::writetolog(std::string * report)					//writes report to log file
{
	//generate report
	report = addTimeString(report);

	//open log file
	/*
	const wchar_t a = 'a';
	FILE * logFile = _wfopen(LOG_FILE_NAME->c_str(), &a);
	*/

	FILE * logFile = fopen("\\log_temp.txt", "a");
	if (logFile == NULL)
		fprintf(stderr, "Log File Open Error: %s", strerror(errno));			// check that file opened successfully
	else {
		//write string to file
		fprintf(logFile, "%s\n", report);

		//close log file
		fclose(logFile);
	}
	delete report;		//deallocate report string
}
void LogFile::writetolog(char * report, char * error) {			//WRAPPER::writes report to log file with error appened
	std::string * tempstr = new std::string(report);
	tempstr->append(error);
	writetolog(tempstr);
}
void LogFile::writetolog(char * report)	{						//WRAPPER::writes report to log file
	std::string * tempstr = new std::string(report);
	writetolog(tempstr);
}
	// Adds error to report and sends to be written to logfile
void LogFile::writetolog(char * report, int error) {		//writes errror report to log file
	// Concatenate to make new report
	std::string catReport = report;
	char n[128];
	itoa(error, n, 10);
	catReport.append(n);

	// Send new report
	writetolog(&catReport);						//send concatenated report
}
void LogFile::writetolog(std::string * report, int error) {				//WRAPPER::writes error report to log file
	*report += error;
	writetolog(report);
}

void LogFile::setLogFileName() {							//sets log file name with time, date and PID info
	setLogFileName("log file_");					// setup logfile with default type/title descriptor
}

void LogFile::setLogFileName(char * type, char * directory) {		// Sets LOG_FILE_NAME with date, PID info and given type in directory specified
	std::string * fileName = new std::string(directory);
	fileName->append(type);

	// Get time string
	std::string *temp = new std::string();
	temp = addTimeString(temp);
	fileName->append(*temp);
	fileName->append(".txt");

	// Set as log file name
	LOG_FILE_NAME = new std::wstring(fileName->begin(), fileName->end());
}
void LogFile::setLogFileName(char * type)	{					// Sets LOG_FILE_NAME with date, PID info and given type
	setLogFileName(type, DEFAULT_LOCATION);					//setup logfile with default storage location
}


// Prepends system time to string for log/error reporting
// report_string dayofweek SystemTime: MM/DD/YYYY HH:MM:SS
std::string* LogFile::addTimeString(std::string * report) {
	std::string *temp;
	char temp1[81];
	time_t t = std::time(nullptr);			// get time
	tm t2;
	localtime_s(&t2, &t);			// convert to local time
	strftime(temp1, 80, "%c: ", &t2);		// convert to string

	temp = new std::string(temp1);					// prepend to report
	if (report != NULL && report->length() > 0)
		*temp += *report;

	return temp;							// return new string
}
