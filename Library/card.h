/*	card.h -- Card Game 'card' class 
	Lucas Weisensee 2015

	stores and operates on a single card

	Card values:
		the card value is the exact card in the deck that the card represents
		the values are 1-24 such that
			1,2,...6 = A,10,K,Q,J,9 = value%6
				and
			0,1,2,3 = Spades,Clubs,Diamonds,Hears = value/6
*/
#pragma once

class card {
public:
	card(char current);
	card();
	~card();
	char chr();			// returns the char value of the current card
	int suit();			// returns the suit of the current card, integer 0 - 3
	int suit(char n);	// returns the suit of card n, integer 0 - 3
	char * str();		// returns the string corresponding to the cards face (Kspads, J diamonds, etc...)
	char * str(char n);	// returns the string corresponding to n's face (Kspads, J diamonds, etc...)
	bool operator==(card &toCompare);	// returns true if toCompare matches current card, false otherwise
	bool operator==(char &toCompare);	// returns true if toCompare matches current card, false otherwise
	bool operator==(int toCompare);	// returns true if toCompare matches current card, false otherwise
	bool operator<(card toCompare);		// returns true if toCompare is greater than this
	bool operator>(card toCompare);		// returns true if toCompare is less than this
	bool operator<(int toCompare);		// returns true if toCompare is greater than this
	bool operator>(int toCompare);		// returns true if toCompare is less than this
	void operator=(int toSet);			// sets the current card value to toSet
	char value;		// numerical char value of the current card
};