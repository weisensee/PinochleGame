// ServerException.h Lucas Weisensee February 2015
//
// exception class for game server

#pragma once
#include <iostream>
#include <exception>

class ServerException : public std::exception {
public:
	int error;
	ServerException(int n);
	const char* sendEr() const throw();
};