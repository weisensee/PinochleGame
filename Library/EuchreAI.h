/* EuchreAI.h Lucas Weisensee 

	Euchre AI Class -- CS441 Final Project

	EuchreAI:

	This class is the functional AI for a euchre card game

	In general it returns a next play when queried


*/
#pragma once
#include <string>
#include "..\Library\EuchreRound.h"


class EuchreAI : public EuchreRound{
public:
	EuchreAI();			// default constructor
	EuchreAI(EuchreRound toMake);			// default constructor
	EuchreAI(std::string genes);	// construct an AI with the given genes
	~EuchreAI();

	card requestPlay();	// Request a play from the AI
};