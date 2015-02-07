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
	//Creates a game list node with the passed in attributes
gameListNode::gameListNode(int ngameID, std::string *ngameName, std::string *nplayerName, HANDLE * ngameHandle, char nstatus, int nplayers)
{
	if(ngameID && ngameHandle && ngameName && nplayerName) {	//if the values were passed correctly
		gameHandle = *ngameHandle;		
		//thread handles for active games array
		status = nstatus;
		gameID = ngameID;
		players = 1;

		//allocate array space
		playerNames = new std::string*[nplayers];				//player name array
		playerNames[0] = new std::string(*nplayerName);			//first player/creating player name copy
		gameName = new std::string(*ngameName);					//game name array allocation
	}
}

gameListNode::~gameListNode()		//deallocate reserved memory
{
	if(gameName)				//if memory was used
		delete gameName;			//delete it
	for (int i = 0; i < 10; ++i)	//check each player name *
	{
		if(playerNames[i])
			delete playerNames[i];	//delete it
		else
			i += 10;
	}
}

std::string gameListNode::getInfoString()				// returns game information as string see "server packet specifications.txt"
{														// X^gameID^gameName^gamecreator^playera^playerb^playern

	std::string * temp = new std::string(status + '^' + gameID + '^' + gameName->c_str() + '^' + *playerNames[0]->c_str());

	//append other player names if available
	if (players > 1)
		for (int i = 1; i < players; ++i)
			temp += '^' + *playerNames[i]->c_str();
}

bool gameListNode::updateStatus(char newStatus)	//updates current game with new status returns true if success
{
	if (newStatus)
		status = newStatus;
	else
		return false;
	return true;
}

