/*	euchreRound.h -- Online Card Game
Desktop Application
Lucas Weisensee
March 2015

Stores and Manages one hand of Euchre

*/

#pragma once
#include <array>
#include <list>

#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\card.h"


class euchreRound {
public:
	// ::CONSTRUCTORS AND DESTRUCTORS::
	euchreRound();	// construct and deal a new round of euchre
	~euchreRound();		//destructor

	// ::GETTERS AND SETTERS::
	char * handDealt(int n);	// returns a char array pointing to the hand player n was dealt
	card flipKitty();			// returns the top card on the deck to be ordered up next, or 0 if no cards remain
	void orderUp(int n);		// records that player n ordered up the top card, their team is also the makers
	bool inPlay();				// return true is in player, false if round is over

	// ::GAMEPLAY::
	bool playCard(card toPlay);			// plays the current card as next on the table, returns true if play is legal, cancels play and returns false if not
	int wonLastTrick();					// returns the player number (0-max) of the winner of the most recent trick
private:
	// ::PLAY MANAGEMENT::
	bool hasCard(int curPlayer, card toPlay);	// returns true is curPlayer has card toPlay
	void updateLead(int curPlayer, card toPlay);// checks to see if toPlay beats the current leading card and updates variables as necessary
	bool takesLead(card toPlay);				// returns true if toPlay beats the current winning card
	bool outOfLeadSuit(int curPlayer);			// returns true if curPlayer is out of lead suit
	bool doesntFollowSuit(card toPlay);			// returns true if toPlay doesn't follow suit of lead card

	card trumpCard, lead, winner;	// this rounds trump card, and current trick's leading and winning card
	bool makers;					// team number of the makers, team 0 (players 0 & 2) and team 1 (players 1 & 3)
	std::array<char, 24> deck;		// hands dealt out to each player, in original order
	std::array<char, 20> played;	// cards played in tricks phase in order
	std::list<card> hands[5];		// current hand lists for each player
	int pos, leader, winning;		// position in tricks phase and leader of current trick

};