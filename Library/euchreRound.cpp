/*	euchreRound.h -- Online Card Game
Desktop Application
Lucas Weisensee
March 2015

Stores and Manages one hand of Euchre

*/

#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\euchreRound.h"


// **************************************************************
// cardMatcher predicate helper class for card list matcher
//***************************************************************
char cardValue;
struct cardMatcher {		// returns true if toMatch matches the global code: 'predicateCode' which must be set before hand, helper function for list removal
public:
	bool operator() (card toMatch) {
		return (toMatch.chr() == cardValue);
	}
	void operator= (char code) {
		cardValue = code;
	}
};

//***********************************************
// ::CONSTRUCTORS AND DESTRUCTORS::
//***********************************************
euchreRound::euchreRound(){	// construct and deal a new round of euchre
	// add cards
	char n=0;
	for (auto it = deck.begin(); it != deck.end(); ++it) {
		*it = n;
		n++;
	}

	//**************DEBUG::*************** print all cards
	card temp;
	for (auto it = deck.begin(); it != deck.end(); ++it) {
		printf(" %s ", temp.str(*it));			// use temp to convert char to card name
	}

	// shuffle deck four times
	std::random_shuffle(deck.begin(), deck.end());
	std::random_shuffle(deck.begin(), deck.end());
	std::random_shuffle(deck.begin(), deck.end());
	std::random_shuffle(deck.begin(), deck.end());

	// save and deal out cards to each player
	for (int i = 0; i < 20; i++) 
		hands[i / 5].push_back(card(deck.at(i)));	// add five cards to each player's hand

	// deal to kitty
	for (int i = 20; i < 25; i++)
		hands[5].push_back(card(deck.at(i)));		// add four cards to the kitty

	// initiate position and leader variable
	leader = 0;
	pos = 0;

}
euchreRound::~euchreRound(){}		//destructor

//***********************************************
// ::GETTERS AND SETTERS::
//***********************************************
char * euchreRound::handDealt(int n) {	// returns a char array pointing to the hand player n was dealt
	char * hand = new char[6];
	
	// copy out card values
	for (int i = 0; i < 5; i++) 
		hand[i] = deck.at(5 * n + i);	// copy out hand that was dealt to player n

	// return array pointer
	return hand;
}
card euchreRound::flipKitty() {			// returns the top card on the deck to be ordered up next, or 0 if no cards remain
	// if the kitty is empty, return 0
	if (hands[5].size() == 0)
		return 0;

	// else, copy, pop and return the card
	trumpCard = hands[5].front();	// copy
	hands[5].pop_front();			// delete
	return trumpCard;				// return

}
void euchreRound::orderUp(int n){		// records that player n ordered up the top card, their team is also the makers
	makers = (n == 0 || n == 2);		// team number of the makers, team 0 (players 0 & 2) and team 1 (players 1 & 3)
}
bool euchreRound::inPlay() {				// return true is in player, false if round is over
	return played.size() < 20;
}

//***********************************************
// ::GAMEPLAY::
//***********************************************
bool euchreRound::playCard(card toPlay) {			// plays the current card as next on the table, returns true if play is legal, cancels play and returns false if not
	// player next up to play
	int curPlayer = (pos % 4 + leader) % 4;

	// if the player doesn't have the card in question, reject
	if (!hasCard(curPlayer, toPlay))
		return false;

	// if the card lead the trick, accept it
	if (pos % 4 != 0) {
		// the leading card is always the winning card starting out
		leader = curPlayer;
		winning = curPlayer;
		lead = toPlay;
		winner = toPlay;
	}

	// if the player didn't match suit and had a card in suit
	else if (doesntFollowSuit(toPlay) && outOfLeadSuit(curPlayer))
		return false;

	// The card was legal, add to play, remove from hand, and accept
	played.at(pos) = toPlay.value;		// add played card to stack of played cards
	pos++;								// increment play position
	cardValue = toPlay.chr();
	hands[curPlayer].remove_if(cardMatcher());	// remove from players hand
	
	// check if toPlay beat lead
	updateLead(curPlayer, toPlay);

	// return success
	return true;
}
int euchreRound::wonLastTrick() {					// returns the player number (0-max) of the winner of the most recent trick
	return leader;
}
//***********************************************
// ::PLAY MANAGEMENT::
//***********************************************
bool euchreRound::hasCard(int curPlayer, card toPlay) {	// returns true is curPlayer has card toPlay
	// if the card matches a card in the players hand
	for (auto it = hands[curPlayer].begin(); it != hands[curPlayer].end(); ++it) {
		if (toPlay == *it)
			return true;
	}

	// if the card is not in the players hand
	return false;
}
void euchreRound::updateLead(int curPlayer, card toPlay) {	// checks to see if toPlay beats the current leading card and updates variables as necessary
	if (takesLead(toPlay)) {
		winner = toPlay;
		winner = curPlayer;
	}
}
bool euchreRound::takesLead(card toPlay) {		// returns true if toPlay beats the current winning card
	// if both cards are the same suit, return highest
	if (toPlay.suit() == winner.suit())
		return toPlay > winner;

	// if the suits don't match, the new card only wins if it's trump
	if (toPlay.suit() == trumpCard.suit())
		return true;

	// if the played card isn't trump, or higher in the same suit as the previous winner, it doesnt take the lead
	return false;
}
bool euchreRound::outOfLeadSuit(int curPlayer) {			// returns true if curPlayer is out of lead suit
	int leadSuit = lead.suit();

	// if the suit lead matches any suit in the player's hand
	for (auto it = hands[curPlayer].begin(); it != hands[curPlayer].end(); ++it) {
		if (leadSuit == it->suit())
			return false;	// return false
	}

	// if the suit is not in the players hand
	return true;

}
bool euchreRound::doesntFollowSuit(card toPlay) {			// returns true if toPlay doesnt follow suit of lead card
	return toPlay.suit() != lead.suit();
}