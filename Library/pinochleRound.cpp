/*	pinochleHand.cpp  -- Online Pinochle Game -- Lucas Weisensee November 2014

		funtions for pinochleRound object which stores one round/hand of a pinochle game

		see pinochleRound.h for more information

		Functions:
		pinochleRound();
		~pinochleRound();
		int playCard(char Card);
		
		Data:
		char handdealt;
		short bid;
		char bidwinner[2];
		short * meld;
		char Cardsplayed[48];
*/

#include "pinochleRound.h"

pinochleRound::pinochleRound() {	// Default Constructor
	bid = 0;
}

int pinochleRound::playCard(char Card) {	// Play a Card during a trick
	printf("\nrunning playCard() function with Card: %c", Card);
	return true;
}
bool inPlay() {				// returns true if the game is in play	
	return false;
}
