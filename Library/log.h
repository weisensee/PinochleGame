/*	log.h -- Online Pinochle Game -- Lucas Weisensee November 2014


*/

char * LOG_FILE_NAME;			// Log file's name
void setLogFileName();			// Sets LOG_FILE_NAME with date and PID info
void writetolog(char * report);							//WRAPPER::writes report to log file
void writetolog(std::string * treport);					//writes report to log file
void writetolog(char * report, int error);				//WRAPPER::writes errror report to log file
std::string* addTimeString(std::string * treport);
