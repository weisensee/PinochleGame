/*	pinochleHand.cpp  -- Online Pinochle Game -- Lucas Weisensee November 2014

		funtions for pinochleRound object which stores one round/hand of a pinochle game

		see pinochleRound.h for more information

		Functions:
		pinochleRound();
		~pinochleRound();
		int playcard(char card);
		
		Data:
		char handdealt;
		short bid;
		char bidwinner[2];
		short * meld;
		char cardsplayed[48];
*/

#include "pinochleRound.h"

pinochleRound::pinochleRound() {	// Default Constructor
	bid = 0;
}

int pinochleRound::playcard(char card) {	// Play a card during a trick
	printf("\nrunning playcard() function with card: %c", card);
	return true;
}
