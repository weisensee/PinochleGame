/*	gameListNode.cpp -- Online Pinochle Game -- Lucas Weisensee November 2014

	functions for game list node items

*/

#include "gameListNode.h"


// Default constructor
gameListNode::gameListNode()
{
	gameID = 0;
	// default construction code
}

gameListNode::gameListNode(_PROCESS_INFORMATION * processInfo, client * curClient)	//Creates a game list node with the passed in attributes
{
	process = *processInfo;		// copy process info
	gameID = getGameID();		// get new game ID
	players = 1;
	gameName = "UNKNOWN";
	*playerNames = new std::string[max_players];
}

gameListNode::~gameListNode() {		//deallocate reserved memory
	for (int i = 0; i < max_players; ++i)	//check each client name *
	{
		if(playerNames[i])
			delete playerNames[i];	//delete it
		else
			i += 100;
	}
}
bool gameListNode::addPlayer(char * name) {		// adds player to current node
	std::string * temp = new std::string(name);		// set it
	return addPlayer(temp);
}
int gameListNode::playerCount() {			// returns player count
	return players;
}
bool gameListNode::addPlayer(std::string * name) {	// adds player to current node
	players++;

	for (int i = 0; i < max_players; i++) {								// For each possible player
		if (playerNames[i] == NULL || playerNames[i]->length() < 1) {	// if the player name is undefined
			playerNames[i] = name;						// set it
			i = max_players;											// then quit checking and 
			return true;												// return success
		}
	}
	return false;
}
std::string gameListNode::getInfoString()				// returns game information as string see "server packet specifications.txt"
{														// X^gameID^gameName^gamecreator^clienta^clientb^clientn

	std::string * temp = new std::string(status + '^' + gameID + '^' + gameName.c_str() + '^' + *playerNames[0]->c_str());

	//append other player names if available
	if (players > 1)
		for (int i = 1; i < players; ++i)
			temp += '^' + *playerNames[i]->c_str();

	// return info string
	return temp->c_str();
}

bool gameListNode::updateStatus(char newStatus)	//updates current game with new status returns true if success
{
	if (newStatus)
		status = newStatus;
	else
		return false;
	return true;
}

int gameListNode::getID() {				// Returns game's ID
	return gameID;
}
int gameListNode::getGameID() {
	return rand();
}
