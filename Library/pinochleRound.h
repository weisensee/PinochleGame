/*	pinochleHand.h -- Online Pinochle Game -- Lucas Weisensee November 2014

		Complete Pinochle Round/Hand Record

		This object holds a complete round or hand of a pinochle game
		
		-starting hands
		-winning bid
		-bid winner
		-meld earned
		-cards played
*/

#include <iostream>
#include "..\Library\Card.h"

class pinochleRound
{
public:
	pinochleRound();
	//~pinochleRound();
	int playcard(char card);
	bool inPlay();				// returns true if the game is in play	
private:
	char handdealt;
	short bid;
	char bidwinner[2];
	short * meld;
	char cardsplayed[48];
};