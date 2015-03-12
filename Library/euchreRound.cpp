/*	EuchreRound.h -- Online Card Game
Desktop Application
Lucas Weisensee
March 2015

Stores and Manages one hand of Euchre

*/

#include "..\Library\EuchreRound.h"


// **************************************************************
// CardMatcher predicate helper class for Card list matcher
//***************************************************************
char CardValue;
struct CardMatcher {		// returns true if toMatch matches the global code: 'predicateCode' which must be set before hand, helper function for list removal
public:
	bool operator() (Card toMatch) {
		return (toMatch.chr() == CardValue);
	}
	void operator= (char code) {
		CardValue = code;
	}
};

//***********************************************
// ::CONSTRUCTORS AND DESTRUCTORS::
//***********************************************
EuchreRound::EuchreRound(){	// construct and deal a new round of euchre
	// add Cards
	char n=0;
	for (auto it = deck.begin(); it != deck.end(); ++it) {
		*it = n;
		n++;
	}

	//**************DEBUG::*************** print all Cards
	Card temp;
	for (auto it = deck.begin(); it != deck.end(); ++it) {
		printf(" %s ", temp.str(*it));			// use temp to convert char to Card name
	}

	// shuffle deck four times
	std::random_shuffle(deck.begin(), deck.end());
	std::random_shuffle(deck.begin(), deck.end());
	std::random_shuffle(deck.begin(), deck.end());
	std::random_shuffle(deck.begin(), deck.end());

	// save and deal out Cards to each player
	for (int i = 0; i < 20; i++) 
		hands[i / 5].push_back(Card(deck.at(i)));	// add five Cards to each player's hand

	// deal to kitty
	for (int i = 20; i < 25; i++)
		hands[5].push_back(Card(deck.at(i)));		// add four Cards to the kitty

	// initiate position and leader variable
	leader = 0;
	pos = 0;

}
EuchreRound::~EuchreRound(){}		//destructor

//***********************************************
// ::GETTERS AND SETTERS::
//***********************************************
char * EuchreRound::handDealt(int n) {	// returns a char array pointing to the hand player n was dealt
	char * hand = new char[6];
	
	// copy out Card values
	for (int i = 0; i < 5; i++) 
		hand[i] = deck.at(5 * n + i);	// copy out hand that was dealt to player n

	// return array pointer
	return hand;
}
Card EuchreRound::flipKitty() {			// returns the top Card on the deck to be ordered up next, or 0 if no Cards remain
	// if the kitty is empty, return 0
	if (hands[5].size() == 0)
		return 0;

	// else, copy, pop and return the Card
	trumpCard = hands[5].front();	// copy
	hands[5].pop_front();			// delete
	return trumpCard;				// return

}
void EuchreRound::orderUp(int n){		// records that player n ordered up the top Card, their team is also the makers
	makers = (n == 0 || n == 2);		// team number of the makers, team 0 (players 0 & 2) and team 1 (players 1 & 3)
}
bool EuchreRound::inPlay() {				// return true is in player, false if round is over
	return played.size() < 20;
}

//***********************************************
// ::GAMEPLAY::
//***********************************************
bool EuchreRound::playCard(Card toPlay) {			// plays the current Card as next on the table, returns true if play is legal, cancels play and returns false if not
	// player next up to play
	int curPlayer = (pos % 4 + leader) % 4;

	// if the player doesn't have the Card in question, reject
	if (!hasCard(curPlayer, toPlay))
		return false;

	// if the Card lead the trick, accept it
	if (pos % 4 != 0) {
		// the leading Card is always the winning Card starting out
		leader = curPlayer;
		winning = curPlayer;
		lead = toPlay;
		winner = toPlay;
	}

	// if the player didn't match suit and had a Card in suit
	else if (doesntFollowSuit(toPlay) && outOfLeadSuit(curPlayer))
		return false;

	// The Card was legal, add to play, remove from hand, and accept
	played.at(pos) = toPlay.value;		// add played Card to stack of played Cards
	pos++;								// increment play position
	CardValue = toPlay.chr();
	hands[curPlayer].remove_if(CardMatcher());	// remove from players hand
	
	// check if toPlay beat lead
	updateLead(curPlayer, toPlay);

	// return success
	return true;
}
int EuchreRound::wonLastTrick() {					// returns the player number (0-max) of the winner of the most recent trick
	return leader;
}
//***********************************************
// ::PLAY MANAGEMENT::
//***********************************************
bool EuchreRound::hasCard(int curPlayer, Card toPlay) {	// returns true is curPlayer has Card toPlay
	// if the Card matches a Card in the players hand
	for (auto it = hands[curPlayer].begin(); it != hands[curPlayer].end(); ++it) {
		if (toPlay == *it)
			return true;
	}

	// if the Card is not in the players hand
	return false;
}
void EuchreRound::updateLead(int curPlayer, Card toPlay) {	// checks to see if toPlay beats the current leading Card and updates variables as necessary
	if (takesLead(toPlay)) {
		winner = toPlay;
		winner = curPlayer;
	}
}
bool EuchreRound::takesLead(Card toPlay) {		// returns true if toPlay beats the current winning Card
	// if both Cards are the same suit, return highest
	if (toPlay.suit() == winner.suit())
		return toPlay > winner;

	// if the suits don't match, the new Card only wins if it's trump
	if (toPlay.suit() == trumpCard.suit())
		return true;

	// if the played Card isn't trump, or higher in the same suit as the previous winner, it doesnt take the lead
	return false;
}
bool EuchreRound::outOfLeadSuit(int curPlayer) {			// returns true if curPlayer is out of lead suit
	int leadSuit = lead.suit();

	// if the suit lead matches any suit in the player's hand
	for (auto it = hands[curPlayer].begin(); it != hands[curPlayer].end(); ++it) {
		if (leadSuit == it->suit())
			return false;	// return false
	}

	// if the suit is not in the players hand
	return true;

}
bool EuchreRound::doesntFollowSuit(Card toPlay) {			// returns true if toPlay doesnt follow suit of lead Card
	return toPlay.suit() != lead.suit();
}