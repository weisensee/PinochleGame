/*	gameList.h -- Online Pinochle Game -- Lucas Weisensee November 2014
		
		Stores list of active games


*/

#include "gameListItem.h"

class gameList
{
public:
	gameList(int n);		//constructs a game list of linked arrays of lenght n
	gameList();				//constructs a game list of linked arrays of length MAX_LENGTH (see gameList.cpp)
	~gameList();			//destructor
	int add(char * gamename, char * playerName, HANDLE threadHandle);		//add game with specified components to list if it's not already there
	void getCurrent(char * sendList);		//copies formatted list of current active games to sendList
	int total();						//returns total active games in list


	bool remove(char * gamename, char * playerName, HANDLE threadHandle);	//remove matching game from list
	bool remove(char * gamename, HANDLE threadHandle);
	bool remove(HANDLE threadHandle);
	bool remove(char * gamename);
private:
	int total;							//total active games
	gameListItem * games;				//array of game objects
	gameList * next;					//next array of game objects

};