/*	gameList.cpp -- Online Pinochle Game -- Lucas Weisensee November 2014
		
		Functions for gameList.h : 
			Manages and stores list of active games
			Responds to queries about active games




NOTES::
nodes no longer delete all following nodes, git test

*/

#include "gameList.h"

//**************************************************
// ::SEARCH ASSIST PREDICATE::
//**************************************************
int gameIdToFind;		// gameID to search for, global variable for searching predicate function
struct idMatcher {		// returns true if toMatch matches the global code: 'predicateCode' which must be set before hand, helper function for list removal
	bool operator() (cardGame* current) {
		return current->getGameID() == gameIdToFind;
	}
	bool operator() (cardGame current) {
		return current.getGameID() == gameIdToFind;
	}
	bool isMatch(cardGame* current) {
		return this->operator()(current);
	}
};


//**************************************************
// ::CONSTRUCTORS AND DESTRUCTORS::
//**************************************************
gameList::gameList(int MAX_GAMES)		//constructs a game list of linked nodes with max length MAX_LENGTH
{
	// Initiate variables
	max = MAX_GAMES;
	currentList = NULL;
}
gameList::gameList() {		//default: constructs a game list with MAX_LENGTH = 10
	gameList(DEFAULT_MAX);
}
gameList::~gameList() {			//destructor
	delete lLog;
}
//**************************************************
// ::DATA MANIPULATION::
//**************************************************
int gameList::addclient(int gameID, client * newClient) {	//add specified client to specified game, returns count of players in game or -1 if no list or game found
	// find game specified
	gameIdToFind = gameID;
	cardGame * current = std::find_if(games.front(), games.back(), idMatcher());

	// check that search completed successfully
	if (idMatcher()(current)) {	// if the back of the list was returned check that it is the game we were searching for
		current->connectClient(newClient);
		return current->playerCount();
	}
	// if the game was not found
	else
		return -1;
}
bool gameList::add(cardGame * toAdd)							//add ngame to list of active games
{
	// If the list is full: 
	if (games.size() >= (unsigned)max)	//return an error
		return 0;

	// Otherwise add toAdd to games list
	games.push_back(toAdd);
	return true;
}
void gameList::remove(int gameID) {	// remove matching game from list
	// remove matching game from games list
	// set global matching predicate
	gameIdToFind = gameID;

	// remove it
	games.remove_if(idMatcher());
}
//**************************************************
// ::GETTERS AND SETTERS::
//**************************************************
std::string gameList::getCurrent()		// copies formatted list of current active games to sendList, returns true if list existed
{
	// Initiate pointers
	std::string toSend; 

	// if there are current active games
	if (games.size() > 0) {
		// for each element in the games list
		for (std::list<cardGame*>::iterator current = games.begin(); current != games.end(); ++current)
			// Request string from each game in list and add it to the sendList
			toSend += (*current)->getInfoString() + "/";	// add its info to send plus game delimiter

		toSend += "*";			// append closing symbol
	}
	// if there are no active games
	else {
		toSend.assign("\0");
	}

	// If the list doesn't exist return empty string
	return toSend.c_str();			// returns success if list existed
}
int gameList::total_games()						//returns total active games in list
{
	return (signed)games.size();
}

