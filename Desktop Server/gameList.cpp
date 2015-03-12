/*	GameList.cpp -- Online Pinochle Game -- Lucas Weisensee November 2014
		
		Functions for GameList.h : 
			Manages and stores list of active games
			Responds to queries about active games




NOTES::
nodes no longer delete all following nodes, git test

*/

#include "GameList.h"

//**************************************************
// ::SEARCH ASSIST PREDICATE::
//**************************************************
int gameIdToFind;		// gameID to search for, global variable for searching predicate function
struct idMatcher {		// returns true if toMatch matches the global code: 'predicateCode' which must be set before hand, helper function for list removal
	bool operator() (CardGame* current) {
		return current->getGameID() == gameIdToFind;
	}
	bool operator() (CardGame current) {
		return current.getGameID() == gameIdToFind;
	}
	bool isMatch(CardGame* current) {
		return this->operator()(current);
	}
};


//**************************************************
// ::CONSTRUCTORS AND DESTRUCTORS::
//**************************************************
GameList::GameList(int MAX_GAMES)		//constructs a game list of linked nodes with max length MAX_LENGTH
{
	// Initiate variables
	max = MAX_GAMES;
	currentList = NULL;
}
GameList::GameList() {		//default: constructs a game list with MAX_LENGTH = 10
	GameList(DEFAULT_MAX);
}
GameList::~GameList() {			//destructor
	delete lLog;
}
//**************************************************
// ::DATA MANIPULATION::
//**************************************************
int GameList::addClient(int gameID, Client * newClient) {	//add specified Client to specified game, returns count of players in game or -1 if no list or game found
	// find game specified
	gameIdToFind = gameID;
	CardGame * current = std::find_if(games.front(), games.back(), idMatcher());

	// check that search completed successfully
	if (idMatcher()(current)) {	// if the back of the list was returned check that it is the game we were searching for
		current->connectClient(newClient);
		return current->playerCount();
	}
	// if the game was not found
	else
		return -1;
}
bool GameList::add(CardGame * toAdd)							//add ngame to list of active games
{
	// If the list is full: 
	if (games.size() >= (unsigned)max)	//return an error
		return 0;

	// Otherwise add toAdd to games list
	games.push_back(toAdd);

	// assign toAdd a gameID
	int newID = nextGameId();
	DEBUG_IF(true)
		printf("\nAdding retrieved new ID: %i, adding to game...", newID);
	toAdd->setGameID(newID);
	return true;
}
void GameList::remove(int gameID) {	// remove matching game from list
	// remove matching game from games list
	// set global matching predicate
	gameIdToFind = gameID;

	// remove it
	games.remove_if(idMatcher());
}
//**************************************************
// ::GETTERS AND SETTERS::
//**************************************************
int GameList::nextGameId() {			// returns next available game id
	srand(time(NULL));
	return rand()%100;
}
std::string GameList::getCurrent()		// copies formatted list of current active games to sendList, returns true if list existed
{
	// Initiate pointers
	std::string toSend; 

	// if there are current active games
	if (games.size() > 0) {
		// for each element in the games list
		for (std::list<CardGame*>::iterator current = games.begin(); current != games.end(); ++current)
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
int GameList::total_games()						//returns total active games in list
{
	return (signed)games.size();
}

