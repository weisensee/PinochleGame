// serverException.cpp Lucas Weisensee February 2015
//
// exception class functions for game server

#include "serverException.h"

serverException::serverException(int n) {
	error = n;
}

  const char* serverException::sendEr() const throw()			// returns "sending error: " + e
  {
	  std::string * temp = new std::string("sending Error: ");
	  char * buf;
	  temp->append(itoa(error, buf, 10));
	  return temp->c_str();
  }