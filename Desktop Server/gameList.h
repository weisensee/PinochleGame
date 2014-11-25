/*	gameList.h -- Online Pinochle Game -- Lucas Weisensee November 2014
		
		Stores list of active games


*/
#ifndef HEADER_GAMELIST_H
#define HEADER_GAMELIST_H

#include "gameListNode.h"
#include <string.h>


class gameList
{
public:
	gameList(unsigned short MAX_LENGTH);	//constructs a game list of linked nodes with max length MAX_LENGTH
	gameList();					//constructs a game list with MAX_LENGTH = 10 (see gameList.cpp)
	~gameList();				//destructor
	int add(char * gamename, char * playerName, HANDLE threadHandle);		//add game with specified components to list if it's not already there
	int add(game * ngame);							//add ngame to list of active games
	bool getCurrent(char * sendList);	//copies formatted list of current active games to sendList
	int total_games();					//returns total active games in list
	bool updateStatus(unsigned short gameID, char status);		//updates the lists current status for a specific game


	bool remove(unsigned short gameID);	//remove matching game from list
	// bool remove(char * gamename, HANDLE threadHandle);
	// bool remove(HANDLE threadHandle);
	// bool remove(char * gamename);
private:
	unsigned short total, max;			//total active games
	gameListNode * head, tail;		//list of game objects

	bool currentListReady;				//true if the current list is ready to be sent
	char * currentList;					//current active list if recently generated

};

#endif