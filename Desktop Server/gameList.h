/*	gameList.h -- Online Pinochle Game -- Lucas Weisensee November 2014
		
		Stores list of active games

		Allows server to add & remove games and get list of current games

		TO DO:
			-Fix process killing
				"Handles in PROCESS_INFORMATION must be closed with CloseHandle when they are no longer needed."
			-interprocess communication with child game manager process:
				-name change
				-player 1 change/quit
				-# of players changes 

*/
#define WIN32_LEAN_AND_MEAN
#pragma once
#include "gameListNode.h"
#include "player.h"

class gameList
{	
public:
	gameList(int MAX_LENGTH);	//constructs a game list of linked nodes with max length MAX_LENGTH
	gameList();					//constructs a game list with MAX_LENGTH = 10 (see gameList.cpp)
	~gameList();				//destructor
	int addPlayer(int gameID, player * newPlayer);	//add specified player to specified game
	bool add(gameListNode * ngame);							// add ngame to list of active games
	bool addGame(_PROCESS_INFORMATION * processInfo, player * curClient);					// add new process/game and client to list
	std::string getCurrent();	//copies formatted list of current active games to sendList, returns true if list existed
	int total_games();					//returns total active games in list
	bool updateStatus(int gameID, char status);		//updates the lists current status for a specific game


	bool remove(int gameID);	//remove matching game from list
	// bool remove(HANDLE threadHandle);

private:
	gameListNode * find_preceeding(int gameID);		// Returns a pointer to the node proceeding gameID: -1 if failed, 0 if head, * to node otherwise

	// Variables::
	int total, max;			//total active games
	gameListNode * head, * tail;		//list of game objects
	bool currentListReady;				//true if the current list is ready to be sent
	std::string * currentList;					//current active list if recently generated

};