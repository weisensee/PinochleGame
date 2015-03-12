/*	Query -- Lucas Weisensee February 2015

	manages user queries at the terminal
*/

#pragma once
#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>

class Query
{
public:
	Query();
	~Query();

	int iQuery(char * Query);	// Queries user for integer
	char cQuery(char * Query, char * answers);	// Queries user for char answer
	bool bQuery(char * Query);					// Queries user for true/false answer
	std::string * sQuery(char * Query);			// Queries user for string answer
	void printAcceptableAns(char * answers);	//prints which char answers are acceptable

	/* data */
};

