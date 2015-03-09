/*	gameList.h -- Online Pinochle Game -- Lucas Weisensee November 2014
		
		Stores list of active games

		Allows server to add, kill and communicate with games and get list of current games

		Maintains access to the active game object ('cardGame') through which it accesses and uses the shared memory space

		TO DO:
			-interprocess communication with child game manager process:
				-name change
				-client 1 change/quit
				-# of clients changes 

*/
#define WIN32_LEAN_AND_MEAN
#pragma once
#include <list>
#include <algorithm>

#include "gameListNode.h"
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\client.h"
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Desktop Server\cardGame.h"

class gameList
{	
public:
	//**************************************************
	// ::CONSTRUCTORS AND DESTRUCTORS::
	//**************************************************
	gameList(int MAX_LENGTH);	//constructs a game list of linked nodes with max length MAX_LENGTH
	gameList();					//constructs a game list with MAX_LENGTH = 10 (see gameList.cpp)
	~gameList();				//destructor

	//**************************************************
	// ::DATA MANIPULATION::
	//**************************************************
	int addclient(int gameID, client * newclient);	//add specified client to specified game
	bool add(cardGame * toAdd);							// add toAdd to list of active games
	void remove(int gameID);	//remove matching game from list

	//**************************************************
	// ::GETTERS AND SETTERS::
	//**************************************************
	std::string getCurrent();			//returns formatted list of current active games to sendList
	int total_games();					//returns total active games in list

private:
	//**************************************************
	// ::SUPPORTING FUNCTIONS::
	//**************************************************

	// List structure
	std::list<cardGame*> games;

	// Variables::
	int max;			//total active games
	bool currentListReady;			//true if the current list is ready to be sent
	std::string * currentList;		//current active list if recently generated
	LogFile * lLog;					// log file

	// ::DEFAULTS::
	int DEFAULT_MAX = 20;
};