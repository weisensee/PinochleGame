/*	gameListNode.h -- Online Pinochle Game -- Lucas Weisensee November 2014

	stores game items for active game list

*/
#ifndef HEADER_GAMELISTNODE_H
#define HEADER_GAMELISTNODE_H

#include <string.h>

class gameListNode
{
public:
	gameListNode(unsigned short ngameID, char * ngameName, char * nplayerName, HANDLE * ngameHandle, char nstatus, unsigned char nplayers);
	~gameListNode();
	std::string getInfoString();				//returns game information as string
	bool updateStatus(char newStatus);	//updates current game with new status
	bool addPlayer(char * name);

	gameListNode * next;
	HANDLE gameHandle;				//thread handles for active games array
	char * playerNames;				//first player/creating player name array
	char * gameName;				//game name array
	unsigned short * gameID;		//ID number for specific Pinochle Game
	char status;					//current status of game
	unsigned char players;			//number of players in game
};

#endif