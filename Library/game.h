/*	game.h -- Online Pinochle Game -- Lucas Weisensee November 2014
	
	Stores a complete Pinochle Game Record

	The Game Class stores and manages all pertinent information for a live pinochle game:
	-game name
	-number of players
	-player names
	-player sockets
	-player scores
	-winning/target score
	-current round data
	
	-past round data? ...TBD
*/

#include "pinochleRound.h"

class game
{
public:
	game(SOCKET ClientSocket, char * gamename, char * playerName, unsigned char maxPlayers);	//single player constructor
	game();								//default constructor
	~game();							//deallocates current game
	int addPlayer(int i, char * pname, SOCKET playerSocket);	//add's player with pname to spot i, returns result info
	bool initiated();					//returns true if game was initiated successfully
	void run();								//starts the game running

private:
	int playcard(char card);			//attempts to play next card on current round
	bool ready();						//returns true if game is full and players are ready
	void quit(int type);				//quits the current game
	int getCurrentPos();				//returns current position of game

	char * gameName;
	short players;
	char ** playerNames;
	short * scores;
	short target;
	int * gameID;						//ID number for specific Pinochle Game

};
