// serverException.h Lucas Weisensee February 2015
//
// exception class for game server

#pragma once
#include <iostream>
#include <exception>

class serverException : public std::exception {
public:
	int error;
	serverException(int n);
	const char* sendEr() const throw();
};