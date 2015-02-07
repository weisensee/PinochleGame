/*	gameList.h -- Online Pinochle Game -- Lucas Weisensee November 2014
		
		Stores list of active games


*/
#ifndef HEADER_GAMELIST_H
#define HEADER_GAMELIST_H

#include "gameListNode.h"

class gameList
{	
public:
	gameList(int MAX_LENGTH);	//constructs a game list of linked nodes with max length MAX_LENGTH
	gameList();					//constructs a game list with MAX_LENGTH = 10 (see gameList.cpp)
	~gameList();				//destructor
	int addGame(char * gamename, char * playerName, HANDLE threadHandle);		//add game with specified components to list if it's not already there
	bool addPlayer(int gameID, char * playerName, SOCKET clientSocket, int playerNum);	//add specified player to specified game
	int add(gameListNode * ngame);							//add ngame to list of active games
	int addGame(int ngameID, std::string *ngameName, std::string *nplayerName, HANDLE * ngameHandle, char nstatus, int nplayers);
	bool getCurrent(std::string*sendList);	//copies formatted list of current active games to sendList, returns true if list existed
	int total_games();					//returns total active games in list
	bool updateStatus(int gameID, char status);		//updates the lists current status for a specific game


	bool remove(unsigned short gameID);	//remove matching game from list
	// bool remove(char * gamename, HANDLE threadHandle);
	// bool remove(HANDLE threadHandle);
	// bool remove(char * gamename);
private:
	int total, max;			//total active games
	gameListNode * head, * tail;		//list of game objects

	bool currentListReady;				//true if the current list is ready to be sent
	std::string * currentList;					//current active list if recently generated

};

#endif