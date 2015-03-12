/*	GameList.h -- Online Pinochle Game -- Lucas Weisensee November 2014
		
		Stores list of active games

		Allows server to add, kill and communicate with games and get list of current games

		Maintains access to the active game object ('CardGame') through which it accesses and uses the shared memory space

		TO DO:
			-interprocess communication with child game manager process:
				-name change
				-Client 1 change/quit
				-# of Clients changes 

*/
#define WIN32_LEAN_AND_MEAN
#pragma once
#include <list>
#include <time.h>
#include <algorithm>

#include "GameListNode.h"
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Library\Client.h"
#include "C:\Users\Pookey\OneDrive\Projects\PinochleGame\Desktop Server\CardGame.h"

class GameList
{	
public:
	//**************************************************
	// ::CONSTRUCTORS AND DESTRUCTORS::
	//**************************************************
	GameList(int MAX_LENGTH);	//constructs a game list of linked nodes with max length MAX_LENGTH
	GameList();					//constructs a game list with MAX_LENGTH = 10 (see GameList.cpp)
	~GameList();				//destructor

	//**************************************************
	// ::DATA MANIPULATION::
	//**************************************************
	int addClient(int gameID, Client * newClient);	//add specified Client to specified game
	bool add(CardGame * toAdd);							// add toAdd to list of active games
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
	int nextGameId();			// returns next available game id
	std::list<CardGame*> games;

	// Variables::
	int max;			//total active games
	bool currentListReady;			//true if the current list is ready to be sent
	std::string * currentList;		//current active list if recently generated
	LogFile * lLog;					// log file

	// ::DEFAULTS::
	int DEFAULT_MAX = 20;
};