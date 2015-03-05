/*	query -- Lucas Weisensee February 2015

	manages user queries at the terminal
*/

#pragma once
#include <stdio.h>
#include <iostream>
#include <string>

class query
{
public:
	query();
	~query();

	int iQuery(char * query);	// Queries user for integer
	char cQuery(char * query, char * answers);	// Queries user for char answer
	std::string * sQuery(char * query);			// Queries user for string answer
	void printAcceptableAns(char * answers);	//prints which char answers are acceptable

	/* data */
};

