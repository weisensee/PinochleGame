/*	gameListNode.h -- Online Pinochle Game -- Lucas Weisensee November 2014

	stores game items for active game list

	TO DO::
		-Implement iterative/distinct game ID in "getGameId()"

*/
#define WIN32_LEAN_AND_MEAN
#pragma once
#include <string>
#include <windows.h>
#include "player.h"


class gameListNode
{
public:
	gameListNode();
	gameListNode(_PROCESS_INFORMATION * processInfo, player * curClient);
	gameListNode(int ngameID, std::string * ngameName, std::string * nplayerName, HANDLE * ngameHandle, char nstatus, int nplayers);
	~gameListNode();
	std::string getInfoString();				//returns game information as string
	bool updateStatus(char newStatus);	//updates current game with new status
	bool addPlayer(char * name);
	int getID();				// Returns game's ID

	// DATA:
	gameListNode * next;

private:
	int gameListNode::getGameID();	// returns new game ID for current game
	_PROCESS_INFORMATION process;	// thread handles for active games array
	std::string **playerNames;		// first player/creating player name array
	std::string gameName;			// game name array
	int gameID;						// ID number for specific Pinochle Game
	char status;					// current status of game
	int players;					// number of players in game
	int max_players = 6;			// max possible players
};