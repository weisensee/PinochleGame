/*	gameListNode.h -- Online Pinochle Game -- Lucas Weisensee November 2014

	stores game items for active game list

*/
#ifndef HEADER_GAMELISTNODE_H
#define HEADER_GAMELISTNODE_H

#include <string>
#include <windows.h>

class gameListNode
{
public:
	gameListNode();
	gameListNode(int ngameID, std::string * ngameName, std::string * nplayerName, HANDLE * ngameHandle, char nstatus, int nplayers);
	~gameListNode();
	std::string getInfoString();				//returns game information as string
	bool updateStatus(char newStatus);	//updates current game with new status
	bool addPlayer(char * name);

	gameListNode * next;
	HANDLE gameHandle;				//thread handles for active games array
	std::string ** playerNames;				//first player/creating player name array
	std::string * gameName;				//game name array
	int gameID;		//ID number for specific Pinochle Game
	char status;					//current status of game
	int players;			//number of players in game
};

#endif