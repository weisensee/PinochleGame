/* EuchreAI.cpp Lucas Weisensee 

	Euchre AI Class -- CS441 Final Project

	EuchreAI -- FUNCTIONS

	This class is the functional AI for a euchre card game

	In general it returns a next play when queried


*/


#include "..\Library\EuchreAI.h"


EuchreAI::EuchreAI() {		// default constructor
}
EuchreAI::EuchreAI(EuchreRound toMake){			// default constructor

}
EuchreAI::EuchreAI(std::string genes) {	// construct an AI with the given genes
}
EuchreAI::~EuchreAI() {
}

card EuchreAI::requestPlay() {	// Request a play from the AI

	return card(0);
}