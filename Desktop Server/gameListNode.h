/*	gameListNode.h -- Online Pinochle Game -- Lucas Weisensee November 2014

	stores game items for active game list

	TO DO::
		-Implement iterative/distinct game ID in "getGameId()"

*/
#define WIN32_LEAN_AND_MEAN
#pragma once
#include <string>
#include <windows.h>
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\client.h"


class gameListNode
{
public:
	gameListNode();
	gameListNode(_PROCESS_INFORMATION * processInfo, client * curClient);
	~gameListNode();
	std::string getInfoString();				//returns game information as string
	bool updateStatus(char newStatus);	// updates current game with new status
	bool addPlayer(char * name);		// adds player to current node
	bool addPlayer(std::string * name);	// adds player to current node
	int getID();				// Returns current game's ID
	int playerCount();			// returns player count

	// DATA:
	gameListNode * next;

private:
	int gameListNode::getGameID();	// returns **NEW** game ID for current game
	_PROCESS_INFORMATION process;	// thread handles for active games array
	std::string **playerNames;		// first player/creating player name array
	std::string gameName;			// game name array
	int gameID;						// ID number for specific Pinochle Game
	char status;					// current status of game
	int players;					// number of players in game
	int max_players = 6;			// max possible players
};